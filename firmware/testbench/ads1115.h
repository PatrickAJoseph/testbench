
#ifndef __ADS1115_H__
#define __ADS1115_H__

#include <stdint.h>

#define ADS1115_I2C_STATUS_OK               (0U)
#define ADS1115_I2C_STATUS_TIMEOUT          (1U)
#define ADS1115_I2C_STATUS_NACK             (2u)

extern void ADS1115_init();
extern uint32_t ADS1115_read_registers(uint8_t reg, uint8_t* data, int len);
extern uint32_t ADS1115_write_registers(uint8_t reg, uint8_t* data, int len);

#endif /* __ADS1115_H__ */