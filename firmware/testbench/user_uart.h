
#ifndef USER_UART_H_
#define USER_UART_H_

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#include "clocks.h"

#define UART_PARITY_NONE        (0U)
#define UART_PARITY_ODD         (1U)
#define UART_PARITY_EVEN        (2U)

#define UART_STOPBITS_ONE       (0U)
#define UART_STOPBITS_TWO       (1U)

#define UART_STATUS_OK                      (0U)
#define UART_STATUS_TIMEOUT                 (1U)
#define UART_STATUS_RECEIVE_BUFFER_FULL     (2U)
#define UART_STATUS_FRAMING_ERROR           (3U)
#define UART_STATUS_PARITY_ERROR            (4U)
#define UART_STATUS_OVERRUN_ERROR           (5U)

#define UART_CONSECUTIVE_TRANSFER_TERMINATION_MODE_READ_COUNT       (0U)
#define UART_CONSECUTIVE_TRANSFER_TERMINATION_MODE_CHAR             (1U)

extern void UART_init();
extern void UART_set_baudrate(int baudrate);
extern void UART_set_parity(int parity);
extern void UART_set_stop_bits(int stop_bits);
extern void UART_start();
extern void UART_stop();
extern void UART_receive_start();
extern int UART_receive_stop();

extern void UART_reset_write_buffer();
extern void UART_put_write_buffer(uint8_t byte);
extern void UART_write();

extern void UART_set_read_timeout_us(clock_t timeout);
extern void UART_read_till_count(size_t n_bytes);
extern void UART_read_until_char(char term_char);
extern uint32_t UART_get_status();
extern uint8_t UART_get_byte_from_read_buffer();
extern void UART_reset_read_buffer();
extern int UART_get_read_count();
extern uint8_t* UART_get_read_buffer();
extern uint8_t* UART_get_write_buffer();
extern void UART_increment_write_count();

extern uint8_t UART_consecutive_transfer_get_status(uint8_t transfer_index);

extern void UART_transfer_till_read_count(size_t n_bytes);
extern void UART_transfer_until_char(char term_char);

extern void UART_consecutive_transfer_set_transfer_count(int count);
extern void UART_consecutive_transfer_set_termination_char(char term_char);
extern void UART_consecutive_transfer_set_termination_mode(int consecutive_transfer_termination_mode);
extern void UART_consecutive_transfer_set_timeout(int transfer_index, clock_t timeout);
extern void UART_consecutive_transfer_set_pause_time(int transfer_index, clock_t timeout);
extern void UART_consecutive_transfer_set_write_count(int transfer_index, size_t count);
extern void UART_consecutive_transfer_set_target_read_count(int transfer_index, size_t count);
extern uint32_t UART_consecutive_transfer_get_read_count(int transfer_index);
extern void UART_consecutive_transfer_write_byte(int transfer_index, uint8_t byte, int byte_index);
extern uint8_t UART_consecutive_transfer_read_byte(int transfer_index, int byte_index);
extern void UART_consecutive_transfer_start();
extern void UART_consecutive_transfer_stop();

#endif /* USER_UART_H_ */
