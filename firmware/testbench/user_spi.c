
#include <stddef.h>

#include "device.h"
#include "ti_sdk_dl_config.h"

#include "user_spi.h"
#include "clocks.h"

#define SPI_MAX_TRANSFER_COUNT          (1024U)
#define SPI_MAX_CONSECUTIVE_TRANSFERS   (32U)

static uint8_t SPI_tx_buffer[SPI_MAX_TRANSFER_COUNT];
static uint8_t SPI_rx_buffer[SPI_MAX_TRANSFER_COUNT];

struct spi
{
    uint8_t* tx_buffer;
    uint8_t* rx_buffer;
    int transfer_count;
    int mode;
    bool msb_first;
    int frequency;
    int consecutive_transfer_count;
};

struct spi_consecutive_transfer
{
    int transfer_count;
    uint8_t* tx_buffer;
    uint8_t* rx_buffer;
    clock_t pause_time_us;
};

static struct spi user_spi;
static struct spi_consecutive_transfer user_spi_consecutive_transfers[SPI_MAX_CONSECUTIVE_TRANSFERS];

void SPI_transfer_internal(uint8_t* tx_buffer, uint8_t* rx_buffer, size_t count)
{
    uint32_t rx_data;
    uint32_t tx_data;

    int rx_count;
    int tx_count;
    bool put;

    rx_count = (int)count;
    tx_count = (int)count;

    DL_GPIO_clearPins(SPI_GPIO_GROUP_PORT, SPI_GPIO_GROUP_SPI_CS_USER_0_PIN);

    while(rx_count)
    {
        put = true;

        while(( tx_count > 0 ) && put)
        {
            tx_data = (uint32_t)(*tx_buffer);

            if(!DL_SPI_isTXFIFOFull(SPI_COMMON_INST))
            {
                DL_SPI_transmitData8(SPI_COMMON_INST, tx_data);
                tx_buffer++;
                tx_count--;
                put = true;
            }
            else
            {
                put = false;
            }
        }

        rx_data = DL_SPI_receiveDataBlocking8(SPI_COMMON_INST);

        *rx_buffer = rx_data;
         rx_buffer++;
         rx_count--;
   }

    while(DL_SPI_isBusy(SPI_COMMON_INST));

    DL_GPIO_setPins(SPI_GPIO_GROUP_PORT, SPI_GPIO_GROUP_SPI_CS_USER_0_PIN);
}

static DL_SPI_Config USER_SPI_config = {
    .mode        = DL_SPI_MODE_CONTROLLER,
    .frameFormat = DL_SPI_FRAME_FORMAT_MOTO3_POL0_PHA0,
    .parity      = DL_SPI_PARITY_NONE,
    .dataSize    = DL_SPI_DATA_SIZE_8,
    .bitOrder    = DL_SPI_BIT_ORDER_MSB_FIRST,
};

static DL_SPI_ClockConfig USER_SPI_clockConfig = {
    .clockSel    = DL_SPI_CLOCK_BUSCLK,
    .divideRatio = DL_SPI_CLOCK_DIVIDE_RATIO_1
};

void SPI_configure()
{
    uint32_t scr;

    if(user_spi.mode == SPI_MODE0)
    {
        USER_SPI_config.frameFormat = DL_SPI_FRAME_FORMAT_MOTO3_POL0_PHA0;
    }
    else if(user_spi.mode == SPI_MODE1)
    {
        USER_SPI_config.frameFormat = DL_SPI_FRAME_FORMAT_MOTO3_POL0_PHA1;
    }
    else if(user_spi.mode == SPI_MODE2)
    {
        USER_SPI_config.frameFormat = DL_SPI_FRAME_FORMAT_MOTO3_POL1_PHA0;
    }
    else if(user_spi.mode == SPI_MODE3)
    {
        USER_SPI_config.frameFormat = DL_SPI_FRAME_FORMAT_MOTO3_POL1_PHA1;
    }

    if(user_spi.msb_first)
    {
        USER_SPI_config.bitOrder = DL_SPI_BIT_ORDER_MSB_FIRST;
    }
    else
    {
        USER_SPI_config.bitOrder = DL_SPI_BIT_ORDER_LSB_FIRST;
    }

    DL_SPI_setClockConfig(SPI_COMMON_INST, &USER_SPI_clockConfig);

    DL_SPI_init(SPI_COMMON_INST, &USER_SPI_config);

    scr = (uint32_t)(((MCLK_FREQ_HZ/4))/(user_spi.frequency)) - 1;

    DL_SPI_setBitRateSerialClockDivider(SPI_COMMON_INST, scr);

    DL_SPI_setFIFOThreshold(SPI_COMMON_INST, DL_SPI_RX_FIFO_LEVEL_1_2_FULL, DL_SPI_TX_FIFO_LEVEL_1_2_EMPTY);

    DL_SPI_enable(SPI_COMMON_INST);
}

void SPI_init()
{
    user_spi.frequency = 100000;
    user_spi.mode = SPI_MODE0;
    user_spi.msb_first = true;
    user_spi.tx_buffer = SPI_tx_buffer;
    user_spi.rx_buffer = SPI_rx_buffer;

    SPI_configure();
}

void SPI_set_bitrate(int bitrate)
{
    user_spi.frequency = bitrate;
    SPI_configure();
}

void SPI_set_mode(int mode)
{
    user_spi.mode = mode;
    SPI_configure();
}

void SPI_set_bit_order(int bit_order)
{
    user_spi.msb_first = (bit_order == SPI_BITORDER_MSB_FIRST) ? true : false;
}

void SPI_start_transfer()
{
    SPI_transfer_internal( user_spi.tx_buffer, user_spi.rx_buffer, user_spi.transfer_count );
}

void SPI_write_buffer_put(uint8_t byte, int index)
{
    user_spi.tx_buffer[index] = byte;
}

uint8_t SPI_read_buffer_get(int index)
{
    return user_spi.rx_buffer[index];
}

void SPI_set_transfer_count(int count)
{
    user_spi.transfer_count = count;
}

void SPI_consecutive_transfer_set_count(int count)
{
    user_spi.consecutive_transfer_count = count;
}

extern void SPI_consecutive_transfer_set_transfer_count(int transfer_index, int count)
{
    user_spi_consecutive_transfers[transfer_index].transfer_count = count;
}

void SPI_consecutive_transfer_write_byte(int transfer_index, uint8_t byte, int byte_index)
{
    int index;
    int start_index;

    start_index = 0;

    for(index = 0 ; index < transfer_index ; index++ )
    {
        start_index += (int)user_spi_consecutive_transfers[index].transfer_count;
    }

    user_spi.tx_buffer[start_index + byte_index] = byte;
}

uint8_t SPI_consecutive_transfer_read_byte(int transfer_index, int byte_index)
{
    int index;
    int start_index;
    uint8_t byte;

    start_index = 0;

    for(index = 0 ; index < transfer_index ; index++ )
    {
        start_index += (int)user_spi_consecutive_transfers[index].transfer_count;
    }

    byte = user_spi.rx_buffer[start_index + byte_index];

    return byte;
}

void SPI_consecutive_transfer_set_pause_interval(int transfer_index, clock_t interval)
{
    user_spi_consecutive_transfers[transfer_index].pause_time_us = interval;
}

void SPI_consecutive_transfer_start()
{
    int transfer_index;
    int index;
    int start_index;

    for(transfer_index = 0 ; transfer_index < user_spi.consecutive_transfer_count ; transfer_index++)
    {
        start_index = 0;

        for(index = 0 ; index < transfer_index ; index++)
        {
            start_index += user_spi_consecutive_transfers[index].transfer_count;
        }

        user_spi_consecutive_transfers[transfer_index].tx_buffer = &user_spi.tx_buffer[start_index];
        user_spi_consecutive_transfers[transfer_index].rx_buffer = &user_spi.rx_buffer[start_index];
    }

    for(transfer_index = 0 ; transfer_index < user_spi.consecutive_transfer_count ; transfer_index++)
    {
        SPI_transfer_internal(user_spi_consecutive_transfers[transfer_index].tx_buffer,
                              user_spi_consecutive_transfers[transfer_index].rx_buffer,
                              user_spi_consecutive_transfers[transfer_index].transfer_count);

        Clocks_wait_us_precise((clock_t)user_spi_consecutive_transfers[transfer_index].pause_time_us);
    }
}
