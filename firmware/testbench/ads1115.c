
#include "ads1115.h"
#include "clocks.h"
#include "ti_sdk_dl_config.h"

#include <string.h>

#define ADS1115_I2C_TRANSFER_TYPE_TX_SINGLE_BYTE            (1U << 0)
#define ADS1115_I2C_TRANSFER_TYPE_TX_MULTIPLE_BYTES         (1U << 1)
#define ADS1115_I2C_TRANSFER_TYPE_TX_ONLY                   (1U << 2)
#define ADS1115_I2C_TRANSFER_TYPE_RX_SINGLE_BYTE            (1U << 3)
#define ADS1115_I2C_TRANSFER_TYPE_RX_MULTIPLE_BYTES         (1U << 4)
#define ADS1115_I2C_TRANSFER_TYPE_RX_ONLY                   (1U << 5)

#define ADS1115_I2C_TIMEOUT_US                      (100000U)
#define ADS1115_I2C_ADDRESS                         (0x48)

struct ads1115_i2c_transaction
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

struct ads1115_context {
    uint8_t read_buffer[4];
    uint8_t write_buffer[4];
    uint32_t status;
};

struct ads1115_context ads1115_context;

struct ads1115_i2c_transaction ads1115_i2c_transaction;

clock_t ads1115_i2c_transaction_start_time = 0;
clock_t ads1115_i2c_transaction_current_time = 0;
clock_t ads1115_i2c_transaction_time = 0;

#define ADS1115_I2C_TRANSACTION_START()         \
    ads1115_i2c_transaction_start_time = Clocks_get_tick_count();

#define ADS1115_I2C_MONITOR_TRANSACTION()              \
    if( ( ADS1115_I2C_INST->i2cc->CPU_INT.RIS & UNICOMMI2CC_CPU_INT_RIS_NACK_MASK )  ||    \
        ( ( ADS1115_I2C_INST->i2cc->SR & UNICOMMI2CC_SR_ERR_MASK ) && \
        (( ADS1115_I2C_INST->i2cc->SR & UNICOMMI2CC_SR_ADRACK_MASK ) || \
        ( ADS1115_I2C_INST->i2cc->SR & UNICOMMI2CC_SR_DATACK_MASK ))  )  )   \
    {                                                                               \
        *status = ADS1115_I2C_STATUS_NACK;                                                  \
        transaction->status = *status;                                              \
        ADS1115_I2C_INST->i2cc->CPU_INT.ICLR = ~0;                                     \
        return *status;                                                             \
    }                                                                               \
                                                                                    \
    ads1115_i2c_transaction_current_time = Clocks_get_tick_count();                    \
    ads1115_i2c_transaction_time = ads1115_i2c_transaction_current_time - ads1115_i2c_transaction_start_time;   \
    if( ads1115_i2c_transaction_time > (clock_t)ADS1115_I2C_TIMEOUT_US )        \
    {                                                                                                   \
        *status = ADS1115_I2C_STATUS_TIMEOUT;                                                           \
        transaction->status = *status;                                                                  \
        return *status;                                                                                 \
    }

uint32_t ADS1115_I2C_transfer(uint8_t* write_buffer, uint8_t* read_buffer, size_t write_count, size_t read_count, uint32_t* status)
{
    struct ads1115_i2c_transaction* transaction = &ads1115_i2c_transaction;
    uint32_t transfer_type = 0;
    bool is_first_transfer = true;
    int current_tx_count = 0;

    *status = ADS1115_I2C_STATUS_OK;

    transaction->status = ADS1115_I2C_STATUS_OK; 

    ads1115_i2c_transaction_start_time = (clock_t)0;

    transaction->tx_count = 0;
    transaction->rx_count = 0;
    transaction->tx_size = (int)write_count;
    transaction->rx_size = (int)read_count;
    transaction->address = ADS1115_I2C_ADDRESS;
    transaction->timeout_us = ADS1115_I2C_TIMEOUT_US;
    transaction->tx_bytes = write_buffer;
    transaction->rx_bytes = read_buffer;


    if( transaction->tx_size == 1 )
    {
        transfer_type |= ADS1115_I2C_TRANSFER_TYPE_TX_SINGLE_BYTE;
    }
    else if( transaction->tx_size > 1 )
    {
        transfer_type |= ADS1115_I2C_TRANSFER_TYPE_TX_MULTIPLE_BYTES;
    }
    else if ( transaction->tx_size == 0 )
    {
        transfer_type |= ADS1115_I2C_TRANSFER_TYPE_RX_ONLY;
    }

    if( transaction->rx_size == 1 )
    {
        transfer_type |= ADS1115_I2C_TRANSFER_TYPE_RX_SINGLE_BYTE;
    }
    else if( transaction->rx_size > 1 )
    {
        transfer_type |= ADS1115_I2C_TRANSFER_TYPE_RX_MULTIPLE_BYTES;
    }
    else if( transaction->rx_size == 0 )
    {
        transfer_type |= ADS1115_I2C_TRANSFER_TYPE_TX_ONLY;
    }

    if( transfer_type & ADS1115_I2C_TRANSFER_TYPE_TX_SINGLE_BYTE )
    {
        DL_I2CC_fillTXFIFO( ADS1115_I2C_INST, transaction->tx_bytes, 1U );

        DL_I2CC_startTransferAdvanced(  ADS1115_I2C_INST,              \
                                        transaction->address,       \
                                        DL_I2CC_DIRECTION_TX,       \
                                        1,                          \
                                        DL_I2CC_START_ENABLE,           \
                                        ( transfer_type & ADS1115_I2C_TRANSFER_TYPE_TX_ONLY ) ?    \
                                        DL_I2CC_STOP_ENABLE : DL_I2CC_STOP_DISABLE,        \
                                        DL_I2CC_ACK_DISABLE);

        ADS1115_I2C_TRANSACTION_START();

        while( !( ADS1115_I2C_INST->i2cc->CPU_INT.RIS & UNICOMMI2CC_CPU_INT_RIS_TXDONE_MASK ) )
        {
            ADS1115_I2C_MONITOR_TRANSACTION();
        }

        ADS1115_I2C_TRANSACTION_START();

        while( ( ADS1115_I2C_INST->i2cc->SR & UNICOMMI2CC_SR_BUSY_MASK ) )
        {
            ADS1115_I2C_MONITOR_TRANSACTION();
        }

        transaction->tx_count++;
    }
    else if( transfer_type & ADS1115_I2C_TRANSFER_TYPE_TX_MULTIPLE_BYTES )
    {
        do{
            current_tx_count = 0;

            if( !DL_I2CC_isTXFIFOFull( ADS1115_I2C_INST) && ( transaction->tx_size - transaction->tx_count ) > 0 )
            {
                current_tx_count = DL_I2CC_fillTXFIFO( ADS1115_I2C_INST, transaction->tx_bytes, transaction->tx_size - transaction->tx_count );
                transaction->tx_bytes += current_tx_count;
                transaction->tx_count += current_tx_count;
            }

            if( !( DL_I2CC_getStatus(ADS1115_I2C_INST) & DL_I2CC_STATUS_BUSY ) && ( current_tx_count > 0 ) )
            {
                DL_I2CC_startTransferAdvanced(  ADS1115_I2C_INST,              \
                                            transaction->address,       \
                                            DL_I2CC_DIRECTION_TX,       \
                                            transaction->tx_size,                          \
                                            is_first_transfer ? DL_I2CC_START_ENABLE : DL_I2CC_START_DISABLE,       \
                                            ( transfer_type & ADS1115_I2C_TRANSFER_TYPE_TX_ONLY ) ?    \
                                            DL_I2CC_STOP_ENABLE : DL_I2CC_STOP_DISABLE,        \
                                            DL_I2CC_ACK_DISABLE);

                is_first_transfer = false;

                ADS1115_I2C_TRANSACTION_START();

                /* Wait until the TX FIFO is empty (transmission of all bytes are done) */
                while(!DL_I2CC_isTXFIFOEmpty( ADS1115_I2C_INST ))
                {
                    ADS1115_I2C_MONITOR_TRANSACTION();
                }
            }
        }while( transaction->tx_count < transaction->tx_size );

        ADS1115_I2C_TRANSACTION_START();

        /* Wait until transmission of all bytes is done. */
        while( !( ADS1115_I2C_INST->i2cc->CPU_INT.RIS & UNICOMMI2CC_CPU_INT_RIS_TXDONE_MASK ) )
        {
            ADS1115_I2C_MONITOR_TRANSACTION();
        }

        /* Wait until all activity in the bus is done. */
        ADS1115_I2C_TRANSACTION_START();

        while( ( ADS1115_I2C_INST->i2cc->SR & UNICOMMI2CC_SR_BUSY_MASK ) )
        {
            ADS1115_I2C_MONITOR_TRANSACTION();
        }
    }


    if( transfer_type & ADS1115_I2C_TRANSFER_TYPE_TX_ONLY )
    {
        return *status;
    }

    is_first_transfer = true;

    if( transfer_type & ADS1115_I2C_TRANSFER_TYPE_RX_SINGLE_BYTE )
    {
        DL_I2CC_startTransferAdvanced(  ADS1115_I2C_INST,              \
                                        transaction->address,       \
                                        DL_I2CC_DIRECTION_RX,       \
                                        1,                          \
                                        DL_I2CC_START_ENABLE,       \
                                        DL_I2CC_STOP_ENABLE,        \
                                        DL_I2CC_ACK_DISABLE);

        ADS1115_I2C_TRANSACTION_START();

        while(DL_I2CC_isRXFIFOEmpty(ADS1115_I2C_INST))
        {
            ADS1115_I2C_MONITOR_TRANSACTION();
        }

        transaction->rx_bytes[0] = DL_I2CC_receiveData(ADS1115_I2C_INST);

        ADS1115_I2C_TRANSACTION_START();

        while( !(ADS1115_I2C_INST->i2cc->SR & UNICOMMI2CC_SR_IDLE_MASK) )
        {
            ADS1115_I2C_MONITOR_TRANSACTION();
        }
    }
    else if( transfer_type & ADS1115_I2C_TRANSFER_TYPE_RX_MULTIPLE_BYTES)
    {
        DL_I2CC_startTransferAdvanced(  ADS1115_I2C_INST,             \
                                        transaction->address,       \
                                        DL_I2CC_DIRECTION_RX,       \
                                        transaction->rx_size,       \
                                        DL_I2CC_START_ENABLE,       \
                                        DL_I2CC_STOP_ENABLE,        \
                                        DL_I2CC_ACK_DISABLE);

        ADS1115_I2C_TRANSACTION_START();

        while(DL_I2CC_isRXFIFOEmpty(ADS1115_I2C_INST))
        {
            ADS1115_I2C_MONITOR_TRANSACTION();
        }

        ADS1115_I2C_TRANSACTION_START();

        do{
            while(!DL_I2CC_isRXFIFOEmpty(ADS1115_I2C_INST))
            {
                transaction->rx_bytes[transaction->rx_count] = DL_I2CC_receiveData(ADS1115_I2C_INST);
                transaction->rx_count++;
                ADS1115_I2C_MONITOR_TRANSACTION();
            }
        }while( transaction->rx_count < transaction->rx_size );

        ADS1115_I2C_TRANSACTION_START();

        while(!DL_I2CC_isRXFIFOEmpty(ADS1115_I2C_INST))
        {
            ADS1115_I2C_MONITOR_TRANSACTION();
        }

        ADS1115_I2C_TRANSACTION_START();

        while( !(ADS1115_I2C_INST->i2cc->SR & UNICOMMI2CC_SR_IDLE_MASK) )
        {
            ADS1115_I2C_MONITOR_TRANSACTION();
        }
    }

    return *status;
}

uint32_t ADS1115_read_registers(uint8_t reg, uint8_t* data, int len)
{
    uint32_t status = ADS1115_I2C_STATUS_OK;
    
    (void)status;

    ads1115_context.write_buffer[0] = reg;

    (void)ADS1115_I2C_transfer(ads1115_context.write_buffer, ads1115_context.read_buffer, 1, 0, &status);

    if(status != ADS1115_I2C_STATUS_OK)
    {
        if(status == ADS1115_I2C_STATUS_TIMEOUT)
        {
            __asm volatile("bkpt 0");
        }

        ADS1115_init();
    }


    (void)ADS1115_I2C_transfer(ads1115_context.write_buffer, ads1115_context.read_buffer, 0, len, &status);

    memcpy(data, ads1115_context.read_buffer, len);

    ads1115_context.status = status;

    if(status != ADS1115_I2C_STATUS_OK)
    {
        if(status == ADS1115_I2C_STATUS_TIMEOUT)
        {
            //__asm volatile("bkpt 0");
        }

        ADS1115_init();
    }

    return status;
}

uint32_t ADS1115_write_registers(uint8_t reg, uint8_t* data, int len)
{
    uint32_t status = ADS1115_I2C_STATUS_OK;
    
    (void)status;

    ads1115_context.write_buffer[0] = reg;

    (void)ADS1115_I2C_transfer(ads1115_context.write_buffer, ads1115_context.read_buffer, 1, 0, &status);

    if(status != ADS1115_I2C_STATUS_OK)
    {
        if(status == ADS1115_I2C_STATUS_TIMEOUT)
        {
            //__asm volatile("bkpt 0");
        }

        ADS1115_init();
    }

    memcpy(&ads1115_context.write_buffer[0], data, len);

    (void)ADS1115_I2C_transfer(ads1115_context.write_buffer, ads1115_context.read_buffer, len, 0, &status);

    ads1115_context.status = status;

    if(status != ADS1115_I2C_STATUS_OK)
    {
        if(status == ADS1115_I2C_STATUS_TIMEOUT)
        {
            //__asm volatile("bkpt 0");
        }

        ADS1115_init();
    }

    return status;
}

void ADS1115_init()
{
    //DL_I2CC_reset(ADS1115_I2C_INST);
    DL_I2CC_resetTransfer(ADS1115_I2C_INST);
    SYSCFG_DL_ADS1115_I2C_init();
}