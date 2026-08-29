#ifndef __USER_GPIO_H__
#define __USER_GPIO_H__

#include <stdint.h>
#include <stdbool.h>

extern void GPIO_init();
extern void GPIO_set_output_state(int index, bool state);
extern bool GPIO_get_input_state(int index);
extern void GPIO_write_port(uint8_t data);
extern uint8_t GPIO_read_port();
extern void GPIO_set_pattern_generation_rate(int rate);
extern void GPIO_set_pattern_generation_length(uint32_t length);
extern void GPIO_set_output_pattern_at_index(int index, uint8_t data);
extern uint8_t GPIO_get_input_pattern_at_index(int index);
extern void GPIO_pattern_generation_start();
extern void GPIO_pattern_generation_stop();

#endif /* __USER_GPIO_H__ */