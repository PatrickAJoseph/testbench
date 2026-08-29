#ifndef USER_I2C_H_

#define USER_I2C_H_

 

#include <stdint.h>

#include <stdbool.h>

 

#define I2C_MODE_STANDARD           (0U)
#define I2C_MODE_FAST               (1U)

 

#define I2C_STATUS_OK               (0U)
#define I2C_STATUS_TIMEOUT          (1U)
#define I2C_STATUS_NACK             (2u)

 

#include "clocks.h"

extern void I2C_init();
extern void I2C_set_mode(int mode);
extern void I2C_set_address(int address);
extern void I2C_set_transfer_timeout_us(uint32_t timeout_us);
extern void I2C_set_read_count(int read_count);
extern void I2C_set_write_count(int write_count);
extern void I2C_write_buffer_put_byte(uint8_t byte, int index);
extern uint8_t I2C_read_buffer_get_byte(int index);
extern void I2C_transfer_start();
extern void I2C_consecutive_transfer_set_count(int count);
extern void I2C_consecutive_transfer_set_read_count(int transfer_index, int count);
extern void I2C_consecutive_transfer_set_write_count(int transfer_index, int count);
extern void I2C_consecutive_transfer_set_pause_time_us(int transfer_index, clock_t time);
extern void I2C_consecutive_transfer_write_buffer_put(int transfer_index, uint8_t byte, int byte_index);
extern uint8_t I2C_consecutive_transfer_read_buffer_get(int transfer_index, int index);
extern void I2C_consecutive_transfer_start(); 
extern uint32_t I2C_transfer_status();
extern uint32_t I2C_consecutive_transfer_status(int index);

#endif /* USER_I2C_H_ */

