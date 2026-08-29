#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#include "user_i2c.h"
#include "clocks.h"
#include "device.h"
#include "ti_sdk_dl_config.h"

#define I2C_READ_BUFFER_COUNT   (1024U)
#define I2C_WRITE_BUFFER_COUNT  (1024U)
#define I2C_MAX_TRANSFER_COUNT  (32U)

#define I2C_TRANSFER_TYPE_TX_SINGLE_BYTE            (1U << 0)
#define I2C_TRANSFER_TYPE_TX_MULTIPLE_BYTES         (1U << 1)
#define I2C_TRANSFER_TYPE_TX_ONLY                   (1U << 2)
#define I2C_TRANSFER_TYPE_RX_SINGLE_BYTE            (1U << 3)
#define I2C_TRANSFER_TYPE_RX_MULTIPLE_BYTES         (1U << 4)
#define I2C_TRANSFER_TYPE_RX_ONLY                   (1U << 5)

 struct user_i2c_config
{
    int mode;
    int address;
};

struct user_i2c
{
    uint8_t* read_buffer;
    uint8_t* write_buffer;
    int read_count;
    int write_count;
    uint32_t status;
    int consecutive_transfer_count;
    uint32_t timeout_us;
};

struct user_i2c_transfer
{
    uint8_t* read_buffer;
    uint8_t* write_buffer;
    size_t read_count;
    size_t write_count;
    uint32_t status;
    clock_t pause_time_us;
};


struct i2c_transaction_internal
{
    uint8_t* tx_bytes;
    uint8_t* rx_bytes;
    int tx_size;
    int rx_size;
    int tx_count;
    int rx_count;
    int status;
    uint32_t timeout_us;
    uint8_t address;
};

struct user_i2c_config user_i2c_config;
struct user_i2c user_i2c;
uint8_t I2C_write_buffer[I2C_WRITE_BUFFER_COUNT];
uint8_t I2C_read_buffer[I2C_READ_BUFFER_COUNT];
struct user_i2c_transfer user_i2c_transfers[I2C_MAX_TRANSFER_COUNT];
struct i2c_transaction_internal user_i2c_internal_transaction;

static uint32_t I2C_transfer_internal(uint8_t* write_buffer, uint8_t* read_buffer, size_t write_count, \
                                        size_t read_count, uint32_t* status);

static void I2C_configure()
{
    DL_I2CC_resetTransfer(USER_I2C_INST);
    DL_I2CC_setTimerPeriod(USER_I2C_INST, (user_i2c_config.mode == I2C_MODE_FAST) ? 24 : 99);
    DL_I2CC_setTXFIFOThreshold(USER_I2C_INST, DL_I2CC_TX_FIFO_LEVEL_1_2_EMPTY);
    DL_I2CC_setRXFIFOThreshold(USER_I2C_INST, DL_I2CC_RX_FIFO_LEVEL_1_2_FULL);
    DL_I2CC_enableClockStretching(USER_I2C_INST);

    /* Enable module */
    DL_I2CC_enable(USER_I2C_INST);
}

void I2C_set_transfer_timeout_us(uint32_t timeout_us)
{
    user_i2c.timeout_us = timeout_us;
}

void I2C_init()
{
    I2C_configure();

    user_i2c.read_buffer = I2C_read_buffer;
    user_i2c.write_buffer = I2C_write_buffer;
    user_i2c.status = 0;
    user_i2c.read_count = 0;
    user_i2c.write_count = 0;
    user_i2c.timeout_us = 1000000;
}

void I2C_set_mode(int mode)
{
    user_i2c_config.mode = mode;
    I2C_configure();
}

void I2C_set_address(int address)
{
    user_i2c_config.address = (address & 0x7F);
}
 
void I2C_set_read_count(int read_count)
{
    user_i2c.read_count = read_count;
}

void I2C_set_write_count(int write_count)
{
    user_i2c.write_count = write_count;
}

void I2C_write_buffer_put_byte(uint8_t byte, int index)
{
    user_i2c.write_buffer[index] = byte;
}

uint8_t I2C_read_buffer_get_byte(int index)
{
    return user_i2c.read_buffer[index];
}
 
void I2C_transfer_start()
{
    (void)I2C_transfer_internal( user_i2c.write_buffer, user_i2c.read_buffer, user_i2c.write_count,
                           user_i2c.read_count, &user_i2c.status);
}

uint32_t I2C_transfer_status()
{
    return user_i2c.status;
}

void I2C_consecutive_transfer_set_count(int count)
{
    user_i2c.consecutive_transfer_count = count;
}

void I2C_consecutive_transfer_set_read_count(int transfer_index, int count)
{
    user_i2c_transfers[transfer_index].read_count = count;
}


void I2C_consecutive_transfer_set_pause_time_us(int transfer_index, clock_t time)
{
    user_i2c_transfers[transfer_index].pause_time_us = time;
}

 

void I2C_consecutive_transfer_set_write_count(int transfer_index, int count)
{
    user_i2c_transfers[transfer_index].write_count = count;
}


void I2C_consecutive_transfer_write_buffer_put(int transfer_index, uint8_t byte, int byte_index)
{
    int start_index;
    int index;

    start_index = 0;

    for( index = 0 ; index < transfer_index ; index++ )
    {
        start_index += user_i2c_transfers[index].write_count;
    }

    I2C_write_buffer[start_index + byte_index] = byte;
}

 

uint8_t I2C_consecutive_transfer_read_buffer_get(int transfer_index, int byte_index)
{
    int start_index;
    int index;

    start_index = 0;

    for( index = 0 ; index < transfer_index ; index++ )
    {
        start_index += user_i2c_transfers[index].write_count;
    }

    return I2C_read_buffer[start_index + byte_index];
}

uint32_t I2C_consecutive_transfer_status(int index)
{
    return user_i2c_transfers[index].status;
}

void I2C_consecutive_transfer_start()
{
    int index;
    int transfer_index;
    int write_start_index;
    int read_start_index;

    for( transfer_index = 0 ; transfer_index < user_i2c.consecutive_transfer_count ; transfer_index++ )
    {
        write_start_index = 0;
        read_start_index = 0;

        for( index = 0 ; index < transfer_index ; index++ )
        {
            write_start_index += user_i2c_transfers[index].write_count;
            read_start_index += user_i2c_transfers[index].read_count;
        }

        user_i2c_transfers[transfer_index].read_buffer = &I2C_read_buffer[read_start_index];
        user_i2c_transfers[transfer_index].write_buffer = &I2C_write_buffer[write_start_index];
    }

    for( transfer_index = 0 ; transfer_index < user_i2c.consecutive_transfer_count ; transfer_index++ )
    {
        (void)I2C_transfer_internal( user_i2c_transfers[transfer_index].write_buffer,
                                     user_i2c_transfers[transfer_index].read_buffer,
                                     user_i2c_transfers[transfer_index].write_count,
                                     user_i2c_transfers[transfer_index].read_count,
                                     &user_i2c_transfers[transfer_index].status);

        Clocks_wait_us_precise( user_i2c_transfers[transfer_index].pause_time_us );
    }
}

#define I2C_TRANSACTION_START()         \
    i2c_internal_transaction_start_time = Clocks_get_time_us();

#define I2C_MONITOR_TRANSACTION()              \
    if( ( USER_I2C_INST->i2cc->CPU_INT.RIS & UNICOMMI2CC_CPU_INT_RIS_NACK_MASK )  ||    \
        ( ( USER_I2C_INST->i2cc->SR & UNICOMMI2CC_SR_ERR_MASK ) && \
        (( USER_I2C_INST->i2cc->SR & UNICOMMI2CC_SR_ADRACK_MASK ) || \
        ( USER_I2C_INST->i2cc->SR & UNICOMMI2CC_SR_DATACK_MASK ))  )  )   \
    {                                                                               \
        *status = I2C_STATUS_NACK;                                                  \
        transaction->status = *status;                                              \
        USER_I2C_INST->i2cc->CPU_INT.ICLR = ~0;                                     \
        return *status;                                                             \
    }                                                                               \
                                                                                    \
    if( ( Clocks_get_time_us() - (clock_t)i2c_internal_transaction_start_time ) > (clock_t)user_i2c.timeout_us )          \
    {                                                                                                   \
        *status = I2C_STATUS_TIMEOUT;                                                                   \
        transaction->status = *status;                                                                  \
        return *status;                                                                                 \
    }


static clock_t i2c_internal_transaction_start_time; 

uint32_t I2C_transfer_internal(uint8_t* write_buffer, uint8_t* read_buffer, size_t write_count, size_t read_count, uint32_t* status)
{
    struct i2c_transaction_internal* transaction = &user_i2c_internal_transaction;
    uint32_t transfer_type = 0;
    bool is_first_transfer = true;
    int current_tx_count = 0;

    *status = I2C_STATUS_OK;

    transaction->status = I2C_STATUS_OK; 

    i2c_internal_transaction_start_time = (clock_t)0;

    transaction->tx_count = 0;
    transaction->rx_count = 0;
    transaction->tx_size = (int)write_count;
    transaction->rx_size = (int)read_count;
    transaction->address = user_i2c_config.address;
    transaction->timeout_us = user_i2c.timeout_us;
    transaction->tx_bytes = write_buffer;
    transaction->rx_bytes = read_buffer;


    if( transaction->tx_size == 1 )
    {
        transfer_type |= I2C_TRANSFER_TYPE_TX_SINGLE_BYTE;
    }
    else if( transaction->tx_size > 1 )
    {
        transfer_type |= I2C_TRANSFER_TYPE_TX_MULTIPLE_BYTES;
    }
    else if ( transaction->tx_size == 0 )
    {
        transfer_type |= I2C_TRANSFER_TYPE_RX_ONLY;
    }

    if( transaction->rx_size == 1 )
    {
        transfer_type |= I2C_TRANSFER_TYPE_RX_SINGLE_BYTE;
    }
    else if( transaction->rx_size > 1 )
    {
        transfer_type |= I2C_TRANSFER_TYPE_RX_MULTIPLE_BYTES;
    }
    else if( transaction->rx_size == 0 )
    {
        transfer_type |= I2C_TRANSFER_TYPE_TX_ONLY;
    }

    if( transfer_type & I2C_TRANSFER_TYPE_TX_SINGLE_BYTE )
    {
        DL_I2CC_fillTXFIFO( USER_I2C_INST, transaction->tx_bytes, 1U );

        DL_I2CC_startTransferAdvanced(  USER_I2C_INST,              \
                                        transaction->address,       \
                                        DL_I2CC_DIRECTION_TX,       \
                                        1,                          \
                                        DL_I2CC_START_ENABLE,           \
                                        ( transfer_type & I2C_TRANSFER_TYPE_TX_ONLY ) ?    \
                                        DL_I2CC_STOP_ENABLE : DL_I2CC_STOP_DISABLE,        \
                                        DL_I2CC_ACK_DISABLE);

        I2C_TRANSACTION_START();

        while( !( USER_I2C_INST->i2cc->CPU_INT.RIS & UNICOMMI2CC_CPU_INT_RIS_TXDONE_MASK ) )
        {
            I2C_MONITOR_TRANSACTION();
        }

        I2C_TRANSACTION_START();

        while( ( USER_I2C_INST->i2cc->SR & UNICOMMI2CC_SR_BUSY_MASK ) )
        {
            I2C_MONITOR_TRANSACTION();
        }

        transaction->tx_count++;
    }
    else if( transfer_type & I2C_TRANSFER_TYPE_TX_MULTIPLE_BYTES )
    {
        do{
            current_tx_count = 0;

            if( !DL_I2CC_isTXFIFOFull( USER_I2C_INST) && ( transaction->tx_size - transaction->tx_count ) > 0 )
            {
                current_tx_count = DL_I2CC_fillTXFIFO( USER_I2C_INST, transaction->tx_bytes, transaction->tx_size - transaction->tx_count );
                transaction->tx_bytes += current_tx_count;
                transaction->tx_count += current_tx_count;
            }

            if( !( DL_I2CC_getStatus(USER_I2C_INST) & DL_I2CC_STATUS_BUSY ) && ( current_tx_count > 0 ) )
            {
                DL_I2CC_startTransferAdvanced(  USER_I2C_INST,              \
                                            transaction->address,       \
                                            DL_I2CC_DIRECTION_TX,       \
                                            transaction->tx_size,                          \
                                            is_first_transfer ? DL_I2CC_START_ENABLE : DL_I2CC_START_DISABLE,       \
                                            ( transfer_type & I2C_TRANSFER_TYPE_TX_ONLY ) ?    \
                                            DL_I2CC_STOP_ENABLE : DL_I2CC_STOP_DISABLE,        \
                                            DL_I2CC_ACK_DISABLE);

                is_first_transfer = false;

                /* Wait until the TX FIFO is empty (transmission of all bytes are done) */
                while(!DL_I2CC_isTXFIFOEmpty( USER_I2C_INST ));
            }
        }while( transaction->tx_count < transaction->tx_size );

        I2C_TRANSACTION_START();

        /* Wait until transmission of all bytes is done. */
        while( !( USER_I2C_INST->i2cc->CPU_INT.RIS & UNICOMMI2CC_CPU_INT_RIS_TXDONE_MASK ) )
        {
            I2C_MONITOR_TRANSACTION();
        }

        /* Wait until all activity in the bus is done. */
        I2C_TRANSACTION_START();

        while( ( USER_I2C_INST->i2cc->SR & UNICOMMI2CC_SR_BUSY_MASK ) )
        {
            I2C_MONITOR_TRANSACTION();
        }
    }


    if( transfer_type & I2C_TRANSFER_TYPE_TX_ONLY )
    {
        return *status;
    }

    is_first_transfer = true;

    if( transfer_type & I2C_TRANSFER_TYPE_RX_SINGLE_BYTE )
    {
        DL_I2CC_startTransferAdvanced(  USER_I2C_INST,              \
                                        transaction->address,       \
                                        DL_I2CC_DIRECTION_RX,       \
                                        1,                          \
                                        DL_I2CC_START_ENABLE,       \
                                        DL_I2CC_STOP_ENABLE,        \
                                        DL_I2CC_ACK_DISABLE);

        while(DL_I2CC_isRXFIFOEmpty(USER_I2C_INST));

        transaction->rx_bytes[0] = DL_I2CC_receiveData(USER_I2C_INST);

        I2C_TRANSACTION_START();

        while( !(USER_I2C_INST->i2cc->SR & UNICOMMI2CC_SR_IDLE_MASK) )
        {
            I2C_MONITOR_TRANSACTION();
        }
    }
    else if( transfer_type & I2C_TRANSFER_TYPE_RX_MULTIPLE_BYTES)
    {
        DL_I2CC_startTransferAdvanced(  USER_I2C_INST,             \
                                        transaction->address,       \
                                        DL_I2CC_DIRECTION_RX,       \
                                        transaction->rx_size,       \
                                        DL_I2CC_START_ENABLE,       \
                                        DL_I2CC_STOP_ENABLE,        \
                                        DL_I2CC_ACK_DISABLE);

        do{
            while(!DL_I2CC_isRXFIFOEmpty(USER_I2C_INST))
            {
                transaction->rx_bytes[transaction->rx_count] = DL_I2CC_receiveData(USER_I2C_INST);
                transaction->rx_count++;
            }
        }while( transaction->rx_count < transaction->rx_size );

        I2C_TRANSACTION_START();

        while(!DL_I2CC_isRXFIFOEmpty(USER_I2C_INST))
        {
            I2C_MONITOR_TRANSACTION();
        }

        I2C_TRANSACTION_START();

        while( !(USER_I2C_INST->i2cc->SR & UNICOMMI2CC_SR_IDLE_MASK) )
        {
            I2C_MONITOR_TRANSACTION();
        }
    }

    return *status;
}