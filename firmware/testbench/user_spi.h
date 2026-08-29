
#ifndef USER_SPI_H_
#define USER_SPI_H_

#include <stdint.h>
#include <stdbool.h>

#include "clocks.h"

#define SPI_MODE0           (0U)
#define SPI_MODE1           (1U)
#define SPI_MODE2           (2U)
#define SPI_MODE3           (3U)

#define SPI_BITORDER_MSB_FIRST      (0U)
#define SPI_BITORDER_LSB_FIRST      (1U)

#define SPI_CS_POLARITY_ACTIVE_LOW  (0U)
#define SPI_CS_POLARITY_ACTIVE_HIGH (1U)

extern void SPI_init();
extern void SPI_set_bitrate(int bitrate);
extern void SPI_set_mode(int mode);
extern void SPI_set_bit_order(int bit_order);
extern void SPI_set_cs_polarity(int polarity);
extern void SPI_start_transfer();
extern void SPI_write_buffer_put(uint8_t byte, int index);
extern uint8_t SPI_read_buffer_get(int index);
extern void SPI_set_transfer_count(int count);

extern void SPI_consecutive_transfer_set_count(int count);
extern void SPI_consecutive_transfer_set_transfer_count(int transfer_index, int count);
extern void SPI_consecutive_transfer_write_byte(int transfer_index, uint8_t byte, int index);
extern uint8_t SPI_consecutive_transfer_read_byte(int transfer_index, int index);
extern void SPI_consecutive_transfer_set_pause_interval(int transfer_index, clock_t interval);
extern void SPI_consecutive_transfer_start();

#endif /* USER_SPI_H_ */
