
#include "ti_sdk_dl_config.h"

#include <stdint.h>
#include <stdbool.h>

#include "user_timer.h"
#include "user_gpio.h"

#define CPUDelay DL_Common_delayCycles

#define GPIO_PATTERN_BUFFER_SIZE                (4096U)

struct user_gpio
{
    uint8_t input;
    uint8_t output;
    int pattern_genertion_rate;
    int pattern_generation_length;
    volatile int pattern_generation_index;
    volatile bool pattern_generation_done;
};

struct user_gpio user_gpio;
uint8_t GPIO_input_pattern_buffer[GPIO_PATTERN_BUFFER_SIZE];
uint8_t GPIO_output_pattern_buffer[GPIO_PATTERN_BUFFER_SIZE];

static void gpio_pattern_generastion_callback(void* args)
{
    (void)args;

    if( user_gpio.pattern_generation_index < user_gpio.pattern_generation_length )
    {
        GPIO_write_port(GPIO_output_pattern_buffer[user_gpio.pattern_generation_index]);
        GPIO_input_pattern_buffer[user_gpio.pattern_generation_index] = GPIO_read_port();
 
        user_gpio.pattern_generation_index++;

        if( user_gpio.pattern_generation_index == user_gpio.pattern_generation_length )
        {
            user_gpio.pattern_generation_done = true;
        }
    }
}

static inline void output_shift_register_write(uint8_t data)
{
    DL_GPIO_setPins(TEST_GROUP_PORT, TEST_GROUP_TEST_PIN_0_PIN);
    while(DL_SPI_isBusy(IOEXP_SPI_INST));
    DL_SPI_transmitDataBlocking8(IOEXP_SPI_INST, data);
    (void)DL_SPI_receiveDataBlocking8(IOEXP_SPI_INST);
    while(DL_SPI_isBusy(IOEXP_SPI_INST));

    DL_GPIO_clearPins(IOEXP_GROUP_PORT, IOEXP_GROUP_IOEXP_OUTPUT_LOAD_PIN);
    CPUDelay(20);
    DL_GPIO_setPins(IOEXP_GROUP_PORT, IOEXP_GROUP_IOEXP_OUTPUT_LOAD_PIN);
    CPUDelay(20);
    DL_GPIO_clearPins(TEST_GROUP_PORT, TEST_GROUP_TEST_PIN_0_PIN);
}

static inline uint8_t input_shift_register_read()
{
    uint8_t data;

    DL_GPIO_clearPins(IOEXP_GROUP_PORT, IOEXP_GROUP_IOEXP_INPUT_LOAD_PIN);
    CPUDelay(20);
    DL_GPIO_setPins(IOEXP_GROUP_PORT, IOEXP_GROUP_IOEXP_INPUT_LOAD_PIN);
    CPUDelay(20);

    while(DL_SPI_isBusy(IOEXP_SPI_INST));

    DL_SPI_transmitDataBlocking8(IOEXP_SPI_INST, 0);
    data = DL_SPI_receiveDataBlocking8(IOEXP_SPI_INST);

    while(DL_SPI_isBusy(IOEXP_SPI_INST));

 
    return data;
}

void GPIO_init()
{

}

void GPIO_set_output_state(int index, bool state)
{
    user_gpio.output &= ~(1U << index);
    user_gpio.output |= (state << index);

    output_shift_register_write(user_gpio.output);
}

bool GPIO_get_input_state(int index)
{
    uint8_t data;
    
    data = input_shift_register_read();
    user_gpio.input = data;

    return ( (user_gpio.input >> index) & 1 ); 
}

void GPIO_write_port(uint8_t data)
{
    output_shift_register_write(data);
}

uint8_t GPIO_read_port()
{
    uint8_t data;

    data = input_shift_register_read();

    return data;
}

void GPIO_set_pattern_generation_rate(int rate)
{
    user_gpio.pattern_genertion_rate = rate;
}

void GPIO_set_pattern_generation_length(uint32_t length)
{
    user_gpio.pattern_generation_length = length;
}

void GPIO_set_output_pattern_at_index(int index, uint8_t data)
{
    GPIO_output_pattern_buffer[index] = data;
}

uint8_t GPIO_get_input_pattern_at_index(int index)
{
    return GPIO_input_pattern_buffer[index];
}

void GPIO_pattern_generation_start()
{
    uint32_t interval_ns;

    interval_ns = (uint32_t)((double)1000000000.0 / (double)user_gpio.pattern_genertion_rate);

    user_gpio.pattern_generation_index = 0;
    user_gpio.pattern_generation_done = false;

    Timer_set_interval_ns(interval_ns);
    Timer_set_callback(gpio_pattern_generastion_callback, NULL);
    Timer_start();
}

void GPIO_pattern_generation_stop()
{
    while( !user_gpio.pattern_generation_done );
    Timer_stop();
}