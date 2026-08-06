
#include "clocks.h"

#include <string.h>
#include <user_uart.h>

#include "device.h"
#include "ti_sdk_dl_config.h"

#define UART_READ_BUFFER_SIZE           (1024U)
#define UART_WRITE_BUFFER_SIZE          (1024U)
#define UART_MAX_CONSECUTIVE_TRANSFER   (8U)

#define UART_INT_MASK       (DL_UART_INTERRUPT_RX | \
                             DL_UART_INTERRUPT_OVERRUN_ERROR | \
                             DL_UART_INTERRUPT_FRAMING_ERROR | \
                             DL_UART_INTERRUPT_PARITY_ERROR)

struct uart
{
    int baudrate;
    int parity;
    int stop_bits;
    uint8_t* read_buffer;
    uint8_t* write_buffer;
    volatile unsigned int read_buffer_index;
    unsigned int read_start_index;
    int read_stop_index;
    int write_count;
    volatile int read_count;
    int user_read_buffer_index;
    clock_t timeout;
    clock_t read_time;
    uint32_t status;
    volatile uint8_t term_char;
    bool term_char_found;
    uint8_t current_rx_byte;
    int consecutive_transfer_count;
    int current_consecutive_transfer;
    int consecutive_transfer_termination_mode;
};

struct uart_consecutive_transfer
{
    uint8_t* read_buffer;
    uint8_t* write_buffer;
    int read_count;
    int write_count;
    clock_t timeout;
    clock_t transfer_time;
    clock_t pause_time;
    uint32_t status;
    uint32_t target_read_count;
};

static uint8_t UART_read_buffer[UART_READ_BUFFER_SIZE];
static uint8_t UART_write_buffer[UART_WRITE_BUFFER_SIZE];

static struct uart user_uart;
static struct uart_consecutive_transfer uart_consecutive_transfers[UART_MAX_CONSECUTIVE_TRANSFER];

void USER_UART_INT_Handler(void)
{
    uint32_t int_status;

    int_status = DL_UART_getRawInterruptStatus(USER_UART_INST, 0xFFFFFFFFU);

    if(int_status & DL_UART_INTERRUPT_RX)
    {
        if(user_uart.read_buffer_index < UART_READ_BUFFER_SIZE)
        {
            user_uart.current_rx_byte = (uint8_t)DL_UART_receiveDataBlocking(USER_UART_INST);
            user_uart.read_buffer[user_uart.read_buffer_index] = user_uart.current_rx_byte;

            if(user_uart.current_rx_byte == user_uart.term_char)
            {
                user_uart.term_char_found = true;
            }

            user_uart.read_buffer_index++;
        }
    }

    if(int_status & DL_UART_INTERRUPT_OVERRUN_ERROR)
    {
        user_uart.status = UART_STATUS_OVERRUN_ERROR;
    }

    if(int_status & DL_UART_INTERRUPT_PARITY_ERROR)
    {
        user_uart.status = UART_STATUS_PARITY_ERROR;
    }

    if(int_status & DL_UART_INTERRUPT_FRAMING_ERROR)
    {
        user_uart.status = UART_STATUS_FRAMING_ERROR;
    }

    DL_UART_clearInterruptStatus(USER_UART_INST, int_status);
}

static const DL_UART_ClockConfig USER_UART_ClockConfig = {
    .clockSel    = DL_UART_CLOCK_BUSCLK,
    .divideRatio = DL_UART_CLOCK_DIVIDE_RATIO_1
};

static DL_UART_Config USER_UART_Config = {
    .mode        = DL_UART_MODE_NORMAL,
    .direction   = DL_UART_DIRECTION_TX_RX,
    .flowControl = DL_UART_FLOW_CONTROL_NONE,
    .parity      = DL_UART_PARITY_NONE,
    .wordLength  = DL_UART_WORD_LENGTH_8_BITS,
    .stopBits    = DL_UART_STOP_BITS_ONE
};

static void UART_apply_configuration()
{
    float div;
    uint32_t ibrd;
    uint32_t fbrd;

    (void)div;
    (void)ibrd;
    (void)fbrd;
    (void)USER_UART_Config;
    (void)USER_UART_ClockConfig;

    if(user_uart.parity == UART_PARITY_NONE)
    {
        USER_UART_Config.parity = DL_UART_PARITY_NONE;
    }
    else if(user_uart.parity == UART_PARITY_ODD)
    {
        USER_UART_Config.parity = DL_UART_PARITY_ODD;
    }
    else if(user_uart.parity == UART_PARITY_EVEN)
    {
        USER_UART_Config.parity = DL_UART_PARITY_EVEN;
    }

    if(user_uart.stop_bits == UART_STOPBITS_ONE)
    {
        USER_UART_Config.stopBits = DL_UART_STOP_BITS_ONE;
    }
    else if(user_uart.stop_bits == UART_STOPBITS_TWO)
    {
        USER_UART_Config.stopBits = DL_UART_STOP_BITS_TWO;
    }

    div = (float)(MCLK_FREQ_HZ/2)/(float)( user_uart.baudrate );
    ibrd = (uint32_t)(div);
    fbrd = (uint32_t)(64.0f * (div - (float)ibrd) + 0.5f);

    DL_UART_reset(USER_UART_INST);

    DL_UART_setClockConfig(USER_UART_INST, (DL_UART_ClockConfig *) &USER_UART_ClockConfig);

    DL_UART_init(USER_UART_INST, (DL_UART_Config *) &USER_UART_Config);

    DL_UART_setOversampling(USER_UART_INST, DL_UART_OVERSAMPLING_RATE_16X);
    DL_UART_setBaudRateDivisor(USER_UART_INST, ibrd, fbrd);

    /* Configure Interrupts */
    DL_UART_enableInterrupt(USER_UART_INST, DL_UART_INTERRUPT_RX | DL_UART_INTERRUPT_FRAMING_ERROR |
            DL_UART_INTERRUPT_OVERRUN_ERROR | DL_UART_INTERRUPT_PARITY_ERROR);

    /* Configure FIFOs */
    DL_UART_setRXFIFOThreshold(USER_UART_INST, DL_UART_RX_FIFO_LEVEL_NOT_EMPTY);
    DL_UART_setTXFIFOThreshold(USER_UART_INST, DL_UART_TX_FIFO_LEVEL_NOT_FULL);

    DL_UART_enable(USER_UART_INST);
}

void UART_init()
{
    user_uart.baudrate = 115200;
    user_uart.parity = UART_PARITY_NONE;
    user_uart.stop_bits = UART_STOPBITS_ONE;
    user_uart.read_buffer = UART_read_buffer;
    user_uart.write_buffer = UART_write_buffer;
    user_uart.read_buffer_index = 0;
    user_uart.status = UART_STATUS_OK;

    UART_apply_configuration();
}

void UART_set_baudrate(int baudrate)
{
    user_uart.baudrate = baudrate;
    UART_apply_configuration();
}

void UART_set_parity(int parity)
{
    user_uart.parity = parity;
    UART_apply_configuration();
}

void UART_set_stop_bits(int stop_bits)
{
    user_uart.stop_bits = stop_bits;
    UART_apply_configuration();
}

void UART_start()
{
    user_uart.read_buffer_index = 0;
    user_uart.status = UART_STATUS_OK;
    memset( user_uart.read_buffer, 0, UART_READ_BUFFER_SIZE );
}

void UART_stop()
{

}

void UART_receive_start()
{
    UART_start();
}

int UART_receive_stop()
{
    user_uart.read_count = user_uart.read_buffer_index - user_uart.read_stop_index;

    if(user_uart.status == UART_STATUS_TIMEOUT)
    {
        user_uart.status = UART_STATUS_OK;
    }

    return user_uart.read_count;
}

uint8_t* UART_get_read_buffer()
{
    return user_uart.read_buffer;
}

uint8_t* UART_get_write_buffer()
{
    return user_uart.write_buffer;
}

void UART_reset_write_buffer()
{
    user_uart.write_count = 0;
    memset(UART_write_buffer, 0, sizeof(UART_write_buffer));
}

void UART_increment_write_count()
{
    user_uart.write_count++;
}

void UART_put_write_buffer(uint8_t byte)
{
    user_uart.write_buffer[user_uart.write_count] = byte;
    user_uart.write_count++;
}

void UART_write()
{
    int index;

    for( index = 0 ; index < user_uart.write_count ; index++ )
    {
        DL_UART_transmitData(USER_UART_INST, user_uart.write_buffer[index]);
    }
}

void UART_set_read_timeout_us(clock_t timeout)
{
    user_uart.timeout = timeout;
}

void UART_read_till_count(size_t n_bytes)
{
    clock_t read_start_time;
    clock_t current_time;

    if(user_uart.read_buffer_index == UART_READ_BUFFER_SIZE)
    {
        user_uart.status = UART_STATUS_RECEIVE_BUFFER_FULL;
        return;
    }

    read_start_time = Clocks_get_time_us();

    user_uart.read_start_index = user_uart.read_buffer_index;

    user_uart.status = UART_STATUS_OK;

    while( ( user_uart.read_buffer_index - user_uart.read_start_index ) < ((unsigned int)n_bytes) )
    {
        current_time = Clocks_get_time_us();

        user_uart.read_time = (clock_t)(current_time - read_start_time);

        if( (clock_t)user_uart.read_time > (clock_t)user_uart.timeout )
        {
            user_uart.status = UART_STATUS_TIMEOUT;
            break;
        }
    }

    user_uart.read_stop_index = user_uart.read_buffer_index;
    user_uart.read_count = user_uart.read_stop_index - user_uart.read_start_index;
    user_uart.user_read_buffer_index = 0;
}

void UART_read_until_char(char term_char)
{
    clock_t read_start_time;
    clock_t current_time;

    read_start_time = Clocks_get_time_us();

    user_uart.term_char_found = false;
    user_uart.term_char = term_char;

    if(user_uart.read_buffer_index == UART_READ_BUFFER_SIZE)
    {
        user_uart.status = UART_STATUS_RECEIVE_BUFFER_FULL;
        return;
    }

    user_uart.read_start_index = user_uart.read_buffer_index;

    user_uart.status = UART_STATUS_OK;

    while( !user_uart.term_char_found )
    {
        current_time = Clocks_get_time_us();

        user_uart.read_time = (clock_t)(current_time - read_start_time);

        if( (clock_t)user_uart.read_time > (clock_t)user_uart.timeout )
        {
            user_uart.status = UART_STATUS_TIMEOUT;
            break;
        }
    }

    user_uart.read_stop_index = user_uart.read_buffer_index;
    user_uart.read_count = user_uart.read_stop_index - user_uart.read_start_index;
    user_uart.user_read_buffer_index = 0;
}

int UART_get_read_count()
{
    return user_uart.read_count;
}

uint32_t UART_get_status()
{
    return (uint32_t)user_uart.status;
}

uint8_t UART_get_byte_from_read_buffer()
{
    uint8_t rx_byte = 0;

    if( user_uart.user_read_buffer_index < user_uart.read_count )
    {
        rx_byte = user_uart.read_buffer[user_uart.read_start_index + user_uart.user_read_buffer_index];
        user_uart.user_read_buffer_index++;
    }

    return(rx_byte);
}

void UART_reset_read_buffer()
{
    user_uart.read_count = 0;
    user_uart.read_buffer_index = 0;
    user_uart.read_start_index = 0;
    user_uart.read_stop_index = 0;
    user_uart.status = UART_STATUS_OK;

    memset(user_uart.read_buffer, 0, UART_READ_BUFFER_SIZE);
}

void UART_transfer_till_read_count(size_t n_bytes)
{
    UART_write();
    UART_read_till_count(n_bytes);
}

void UART_transfer_until_char(char term_char)
{
   UART_write();
   UART_read_until_char(term_char);
}

void UART_consecutive_transfer_set_transfer_count(int count)
{
    user_uart.consecutive_transfer_count = count;
}

void UART_consecutive_transfer_set_termination_mode(int consecutive_transfer_termination_mode)
{
    user_uart.consecutive_transfer_termination_mode = consecutive_transfer_termination_mode;
}

void UART_consecutive_transfer_set_timeout(int transfer_index, clock_t timeout)
{
    uart_consecutive_transfers[transfer_index].timeout = timeout;
}

void UART_consecutive_transfer_set_write_count(int transfer_index, size_t count)
{
    uart_consecutive_transfers[transfer_index].write_count = count;
}

uint32_t UART_consecutive_transfer_get_read_count(int transfer_index)
{
    return uart_consecutive_transfers[transfer_index].read_count;
}

void UART_consecutive_transfer_set_target_read_count(int transfer_index, size_t count)
{
    uart_consecutive_transfers[transfer_index].target_read_count = count;
}

void UART_consecutive_transfer_write_byte(int transfer_index, uint8_t byte, int byte_index)
{
    int index;
    int start_index;

    start_index = 0;

    for(index = 0 ; index < transfer_index ; index++ )
    {
        start_index += (int)uart_consecutive_transfers[index].write_count;
    }

    user_uart.write_buffer[start_index + byte_index] = byte;
}

uint8_t UART_consecutive_transfer_read_byte(int transfer_index, int byte_index)
{
    return uart_consecutive_transfers[transfer_index].read_buffer[byte_index];
}

static void UART_consecutive_transfer_write(uint8_t* data, size_t size)
{
    size_t count = size;
    uint8_t* ptr;

    ptr = data;

    while(count)
    {
        DL_UART_transmitData(USER_UART_INST, *ptr);
        ptr++;
        count--;
    }
}

static void UART_consecutive_transfer_read_till_count(int transfer_index)
{
    clock_t read_start_time;
    clock_t current_time;

    struct uart_consecutive_transfer* transfer = &uart_consecutive_transfers[transfer_index];

    if(user_uart.read_buffer_index == UART_READ_BUFFER_SIZE)
    {
        user_uart.status = UART_STATUS_RECEIVE_BUFFER_FULL;
        transfer->status = UART_STATUS_RECEIVE_BUFFER_FULL;
        return;
    }

    read_start_time = Clocks_get_time_us();

    user_uart.read_start_index = user_uart.read_buffer_index;
    transfer->read_buffer = &UART_read_buffer[user_uart.read_start_index];

    user_uart.status = UART_STATUS_OK;
    transfer->status = UART_STATUS_OK;

    while( ( user_uart.read_buffer_index - user_uart.read_start_index ) < ((unsigned int)transfer->target_read_count) )
    {
        current_time = Clocks_get_time_us();

        user_uart.read_time = (clock_t)(current_time - read_start_time);
        transfer->transfer_time = user_uart.read_time;

        if( (clock_t)user_uart.read_time > (clock_t)transfer->timeout )
        {
            user_uart.status = UART_STATUS_TIMEOUT;
            transfer->status = UART_STATUS_TIMEOUT;
            break;
        }
    }

    user_uart.read_stop_index = user_uart.read_buffer_index;
    user_uart.read_count = user_uart.read_stop_index - user_uart.read_start_index;
    transfer->read_count = user_uart.read_count;
    user_uart.user_read_buffer_index = 0;
}

static void UART_consecutive_transfer_read_until_char(int transfer_index)
{
    clock_t read_start_time;
    clock_t current_time;

    struct uart_consecutive_transfer* transfer = &uart_consecutive_transfers[transfer_index];

    read_start_time = Clocks_get_time_us();

    user_uart.term_char_found = false;

    transfer->status = UART_STATUS_OK;

    if(user_uart.read_buffer_index == UART_READ_BUFFER_SIZE)
    {
        user_uart.status = UART_STATUS_RECEIVE_BUFFER_FULL;
        transfer->status = UART_STATUS_RECEIVE_BUFFER_FULL;
        return;
    }

    user_uart.read_start_index = user_uart.read_buffer_index;

    transfer->read_buffer = &UART_read_buffer[user_uart.read_start_index];

    user_uart.status = UART_STATUS_OK;

    while( !user_uart.term_char_found )
    {
        current_time = Clocks_get_time_us();

        user_uart.read_time = (clock_t)(current_time - read_start_time);
        transfer->transfer_time = user_uart.read_time;

        if( (clock_t)user_uart.read_time > (clock_t)transfer->timeout )
        {
            user_uart.status = UART_STATUS_TIMEOUT;
            transfer->status = UART_STATUS_TIMEOUT;
            break;
        }
    }

    user_uart.read_stop_index = user_uart.read_buffer_index;
    user_uart.read_count = user_uart.read_stop_index - user_uart.read_start_index;
    transfer->read_count = user_uart.read_count;
    user_uart.user_read_buffer_index = 0;
}

void UART_consecutive_transfer_set_termination_char(char term_char)
{
    user_uart.term_char = term_char;
}

void UART_consecutive_transfer_set_pause_time(int transfer_index, clock_t pause_time)
{
    uart_consecutive_transfers[transfer_index].pause_time = pause_time;
}

uint8_t UART_consecutive_transfer_get_status(uint8_t transfer_index)
{
    return uart_consecutive_transfers[transfer_index].status;
}

void UART_consecutive_transfer_start()
{
    int transfer_index;
    int start_index;
    int index;

    for( transfer_index = 0 ; transfer_index < user_uart.consecutive_transfer_count ; transfer_index++ )
    {
        start_index = 0;

        for( index = 0 ; index < transfer_index ; index++ )
        {
            start_index += (int)uart_consecutive_transfers[index].write_count;
        }

        uart_consecutive_transfers[transfer_index].write_buffer = &user_uart.write_buffer[start_index];
    }

    UART_reset_read_buffer();

    for( transfer_index = 0 ; transfer_index < user_uart.consecutive_transfer_count ; transfer_index++ )
    {
        uart_consecutive_transfers[transfer_index].read_count = 0;
        uart_consecutive_transfers[transfer_index].status = UART_STATUS_OK;

        UART_consecutive_transfer_write(uart_consecutive_transfers[transfer_index].write_buffer,
                                        (size_t)uart_consecutive_transfers[transfer_index].write_count);

        if(user_uart.consecutive_transfer_termination_mode == UART_CONSECUTIVE_TRANSFER_TERMINATION_MODE_READ_COUNT)
        {
            UART_consecutive_transfer_read_till_count(transfer_index);
        }
        else
        {
            UART_consecutive_transfer_read_until_char(transfer_index);
        }

        Clocks_wait_us_precise(uart_consecutive_transfers[transfer_index].pause_time);
    }
}

void UART_consecutive_transfer_stop()
{
    UART_stop();
}
