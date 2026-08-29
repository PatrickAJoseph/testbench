/*
 * protocol_app_if.c
 *
 *  Created on: 03-Jul-2026
 *      Author: hp
 */

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include "protocol_app_interface.h"
#include "protocol.h"

#include "device.h"
#include "ti_sdk_dl_config.h"

#include "pwm.h"
#include "user_uart.h"
#include "user_i2c.h"
#include "user_spi.h"
#include "user_gpio.h"
#include "capture.h"
#include "hsadc.h"

struct protocol_context protocol_context;

void RS485_UART_INT_Handler()
{
    uint8_t rx_byte;

    rx_byte = DL_UART_receiveDataBlocking(RS485_UART_INST);

    protocol_handle_rx_byte(&protocol_context, rx_byte);

    DL_UART_clearInterruptStatus(RS485_UART_INST, 0xFFFFFFFFU);
}

static void protocol_uart_send(uint8_t* data, size_t length)
{
    int index = 0;

    while(length)
    {
        DL_UART_transmitDataBlocking(RS485_UART_INST, data[index]);
        index++;
        length--;
    }
}

static void protocol_uart_send_byte(uint8_t byte)
{
    DL_UART_transmitDataBlocking(RS485_UART_INST, byte);
}

static void protocol_uart_init()
{

}

static const protocol_app_callback_list_element_t protocol_app_if_callback_list[];

void protocol_app_interface_init()
{
    protocol_uart_init();
    protocol_init(&protocol_context);

    protocol_context.send = protocol_uart_send;
    protocol_context.send_byte = protocol_uart_send_byte;
    protocol_context.app_callback_list = protocol_app_if_callback_list;
    protocol_context.address = 0x10;
}

void protocol_app_interface_process()
{
    while( !protocol_process(&protocol_context) );
}

/* Application specific code starts here. */

static void ping_device(struct protocol_context* context);
static void get_device_info(struct protocol_context* context);
static void pwm_set_config(struct protocol_context* context);
static void uart_configure(struct protocol_context* context);
void uart_write(struct protocol_context* context);
static void uart_read(struct protocol_context* context);
static void uart_read_buffer_get(struct protocol_context* context);
static void uart_put_into_write_buffer(struct protocol_context* context);
static void uart_single_transfer(struct protocol_context* context);
static void uart_consecutive_transfers_configure(struct protocol_context* context);
static void uart_configure_consecutive_transfer_instance(struct protocol_context* context);
static void uart_consecutive_transfer_instance_write_bytes(struct protocol_context* context);
static void uart_consecutive_transfer_start(struct protocol_context* context);
static void uart_consecutive_transfer_instance_get_read_count_and_status(struct protocol_context* context);
static void uart_consecutive_transfer_instance_read_bytes(struct protocol_context* context);
static void capture_control(struct protocol_context* context);
static void capture_get_data(struct protocol_context* context);
static void hsadc_configure(struct protocol_context* context);
static void hsadc_control(struct protocol_context* context);
static void hsadc_get_samples(struct protocol_context* context);
static void i2c_configure(struct protocol_context* context);
static void i2c_configure_single_transfer(struct protocol_context* context);
static void i2c_write_buffer_put(struct protocol_context* context);
static void i2c_read_buffer_get(struct protocol_context* context);
static void i2c_single_transfer(struct protocol_context* context);
static void i2c_consecutive_transfer_configure(struct protocol_context* context);
static void i2c_consecutive_transfer_write_buffer_put(struct protocol_context* context);
static void i2c_consecutive_transfer_read_buffer_get(struct protocol_context* context);
static void i2c_consecutive_transfer_start(struct protocol_context* context);
static void spi_configure(struct protocol_context* context);
static void spi_configure_single_transfer(struct protocol_context* context);
static void spi_write_buffer_put(struct protocol_context* context);
static void spi_read_buffer_get(struct protocol_context* context);
static void spi_single_transfer(struct protocol_context* context);
static void spi_consecutive_transfer_configure(struct protocol_context* context);
static void spi_consecutive_transfer_write_buffer_put(struct protocol_context* context);
static void spi_consecutive_transfer_read_buffer_get(struct protocol_context* context);
static void spi_consecutive_transfer_start(struct protocol_context* context);
static void gpio_set_pin_state(struct protocol_context* context);
static void gpio_get_pin_state(struct protocol_context* context);
static void gpio_write_port(struct protocol_context* context);
static void gpio_read_port(struct protocol_context* context);
static void gpio_pattern_generator_configure(struct protocol_context* context);
static void gpio_pattern_generator_write_output_sample(struct protocol_context* context);
static void gpio_pattern_generator_read_input_sample(struct protocol_context* context);
static void gpio_pattern_generator_start(struct protocol_context* context);
static void gpio_pattern_generator_stop(struct protocol_context* context);

/* Define the command list over here */

static const protocol_app_callback_list_element_t protocol_app_if_callback_list[] =
{
 { .command = 0x01, .callback = ping_device },
 { .command = 0x02, .callback = get_device_info },
 { .command = 0x03, .callback = pwm_set_config },
 { .command = 0x04, .callback = uart_configure },
 { .command = 0x05, .callback = uart_write },
 { .command = 0x06, .callback = uart_read },
 { .command = 0x07, .callback = uart_read_buffer_get },
 { .command = 0x08, .callback = uart_put_into_write_buffer },
 { .command = 0x09, .callback = uart_single_transfer },
 { .command = 0x0A, .callback = uart_consecutive_transfers_configure },
 { .command = 0x0B, .callback = uart_configure_consecutive_transfer_instance },
 { .command = 0x0C, .callback = uart_consecutive_transfer_instance_write_bytes },
 { .command = 0x0D, .callback = uart_consecutive_transfer_start },
 { .command = 0x0E, .callback = uart_consecutive_transfer_instance_get_read_count_and_status },
 { .command = 0x0F, .callback = uart_consecutive_transfer_instance_read_bytes },
 { .command = 0x10, .callback = capture_control },
 { .command = 0x11, .callback = capture_get_data },
 { .command = 0x12, .callback = hsadc_configure },
 { .command = 0x13, .callback = hsadc_control },
 { .command = 0x14, .callback = hsadc_get_samples },
 { .command = 0x15, .callback = i2c_configure },
 { .command = 0x16, .callback = i2c_configure_single_transfer },
 { .command = 0x17, .callback = i2c_write_buffer_put, },
 { .command = 0x18, .callback = i2c_read_buffer_get, },
 { .command = 0x19, .callback = i2c_single_transfer, },
 { .command = 0x1A, .callback = i2c_consecutive_transfer_configure, },
 { .command = 0x1B, .callback = i2c_consecutive_transfer_write_buffer_put, },
 { .command = 0x1C, .callback = i2c_consecutive_transfer_read_buffer_get, },
 { .command = 0x1D, .callback = i2c_consecutive_transfer_start, },
 { .command = 0x1E, .callback = spi_configure, },
 { .command = 0x1F, .callback = spi_configure_single_transfer, },
 { .command = 0x20, .callback = spi_write_buffer_put, },
 { .command = 0x21, .callback = spi_read_buffer_get, },
 { .command = 0x22, .callback = spi_single_transfer, },
 { .command = 0x23, .callback = spi_consecutive_transfer_configure, },
 { .command = 0x24, .callback = spi_consecutive_transfer_write_buffer_put, },
 { .command = 0x25, .callback = spi_consecutive_transfer_read_buffer_get, },
 { .command = 0x26, .callback = spi_consecutive_transfer_start, },
 { .command = 0x27, .callback = gpio_set_pin_state, },
 { .command = 0x28, .callback = gpio_get_pin_state, },
 { .command = 0x29, .callback = gpio_write_port, },
 { .command = 0x2A, .callback = gpio_read_port, },
 { .command = 0x2B, .callback = gpio_pattern_generator_configure, },
 { .command = 0x2C, .callback = gpio_pattern_generator_write_output_sample, },
 { .command = 0x2D, .callback = gpio_pattern_generator_read_input_sample, },
 { .command = 0x2E, .callback = gpio_pattern_generator_start, },
 { .command = 0x2F, .callback = gpio_pattern_generator_stop, },
 { .command = 0x00, .callback = NULL },
};

/**
 * @brief       Ping device
 * @detail      Used to test connection with the device. Returns a ping count value.
 */

struct ping_device_info_request
{
    uint8_t reset_ping_counter;
    uint32_t reset_ping_counter_value;
};

static int ping_counter_value = 0;

static void ping_device(struct protocol_context* context)
{
    struct ping_device_info_request* ping_info = (struct ping_device_info_request*)context->payload;

    if(ping_info->reset_ping_counter)
    {
        ping_counter_value = ping_info->reset_ping_counter;
    }
    else
    {
        ping_counter_value++;
    }

    protocol_set_response_payload_length(context, 4);
    PROTOCOL_WRITE_UINT32(context, ping_counter_value, 0);
}

/**
 * @brief   Function to get device information.
 *
 */

#define DEVICE_NAME                         "test_bench"
#define DEVICE_FIRMWARE_VERSION             "1.0.0.0"
#define DEVICE_INFO_INVALID_REQUEST         "invalid_request"

static void get_device_info(struct protocol_context* context)
{
    uint8_t get_device_info_request_info_string_length;
    char* device_info;
    char* device_info_response;
    int index;
    int response_string_length;

    get_device_info_request_info_string_length = (uint8_t)context->payload[0];
    device_info = (char*)&context->payload[1];
    device_info[get_device_info_request_info_string_length] = '\0';

    response_string_length = strlen(DEVICE_INFO_INVALID_REQUEST);
    device_info_response = (char*)DEVICE_INFO_INVALID_REQUEST;

    if( strstr(device_info, "name") != NULL )
    {
        response_string_length = strlen(DEVICE_NAME);
        device_info_response = (char*)DEVICE_NAME;
    }

    if( strstr(device_info, "fw_version") != NULL )
    {
        response_string_length = strlen(DEVICE_FIRMWARE_VERSION);
        device_info_response = (char*)DEVICE_FIRMWARE_VERSION;
    }

    protocol_set_response_payload_length(context, response_string_length + 1);
    context->payload[0] = response_string_length;

    for(index = 0; index < response_string_length ; index++)
    {
        context->payload[index + 1] = device_info_response[index];
    }
}

/**
 * @brief Configure PWM settings.
 *
 */

#define PWM_CONFIG_OK                   (0)
#define PWM_CONFIG_INVALID_CHANNEL      (-1)
#define PWM_CONFIG_INVALID_FREQUENCY    (-2)
#define PWM_CONFIG_INVALID_DUTY         (-3)

static void pwm_set_config(struct protocol_context* context)
{
    uint8_t enable;
    uint32_t frequency;
    float duty;
    int channel;

    protocol_set_response_payload_length(context, 1);

    channel = (int)context->payload[0];

    if(channel > 4)
    {
        context->payload[0] = (uint8_t)PWM_CONFIG_INVALID_CHANNEL;
        return;
    }

    enable = context->payload[1];

    PROTOCOL_READ_UINT32( context, (&frequency), 2 );
    PROTOCOL_READ_FLOAT( context, ((uint32_t*)&duty), 6 );

    if((duty > 1.0f) || (duty < 0.0f))
    {
        context->payload[0] = (uint8_t)PWM_CONFIG_INVALID_DUTY;
        return;
    }

    if( (frequency == 0) || (frequency > PWM_MAX_FREQUENCY) )
    {
        context->payload[0] = (uint8_t)PWM_CONFIG_INVALID_FREQUENCY;
        return;
    }

    PWM_set_frequency(channel, frequency);
    PWM_set_duty(channel, duty);

    if(enable)
    {
        PWM_enable(channel);
    }
    else
    {
        PWM_disable(channel);
    }

    context->payload[0] = (uint8_t)PWM_CONFIG_OK;
}

/**
 * @brief Configures settings of the UART: baud rate, parity & stop bits.
 *
 */

#define UART_CONFIGURE_STATUS_OK        0

static void uart_configure(struct protocol_context* context)
{
    uint32_t baud_rate;
    uint8_t stop_bits;
    uint8_t parity;

    PROTOCOL_READ_UINT32(context, &baud_rate, 0);
    parity = context->payload[4];
    stop_bits = context->payload[5];

    UART_set_baudrate(baud_rate);
    UART_set_parity(parity);
    UART_set_stop_bits(stop_bits);

    protocol_set_response_payload_length(context, 1);
    context->payload[0] = UART_get_status();
}

void uart_write(struct protocol_context* context)
{
    int number_of_bytes;
    bool reset;
    bool start_write;
    uint8_t* data = &context->payload[4];

    reset = ( (context->payload[0]) != 0 );
    start_write = ( (context->payload[1]) != 0 );
    number_of_bytes = ((int)context->payload[2] << 8) | ((int)context->payload[3]);

    if(reset)
    {
        UART_reset_write_buffer();
    }

    while(number_of_bytes)
    {
        number_of_bytes--;
        UART_put_write_buffer(*data);
        data++;
    }

    if(start_write)
    {
        UART_write();
    }

    protocol_set_response_payload_length(context, 1);
    context->payload[0] = UART_get_status();
}

/**
 * @brief   Read target number of bytes from UART IF with specified
 *          timeout (specified in micro-seconds).
 *
 */

static void uart_read(struct protocol_context* context)
{
    bool start;
    bool stop;
    bool blocking_mode;
    bool reset;
    bool terminal_character_mode;
    uint8_t terminal_byte;
    int length;
    int timeout;
    int count;

    reset = (context->payload[0] != 0);
    start = (context->payload[1] != 0);
    stop = (context->payload[2] != 0);
    blocking_mode = (context->payload[3] != 0);
    terminal_character_mode = (context->payload[4] != 0);
    terminal_byte = context->payload[5];

    PROTOCOL_READ_UINT32( context, &length, 6 );
    PROTOCOL_READ_UINT32( context, &timeout, 10 );

    if(blocking_mode)
    {
        UART_set_read_timeout_us(timeout);
    }

    if(reset)
    {
        UART_reset_read_buffer();
    }

    if(start)
    {
        if(blocking_mode)
        {
            if(!terminal_character_mode)
            {
                UART_read_till_count(length);
            }
            else
            {
                UART_read_until_char(terminal_byte);
            }
        }
        else
        {
            UART_receive_start();
        }
    }

    if(stop)
    {
        count = UART_receive_stop();
    }

    if(blocking_mode)
    {
        count = UART_get_read_count();
    }

    protocol_set_response_payload_length(context, 5);

    context->payload[0] = (uint8_t)UART_get_status();
    PROTOCOL_WRITE_UINT32( context, count, 1 );
}

/**
 *
 * @brief Get data from the UART read buffer.
 *
 */

static void uart_read_buffer_get(struct protocol_context* context)
{
    uint16_t start_index;
    uint8_t length;
    uint8_t* read_buffer;
    int end_byte_index = 0;
    int number_of_bytes_received = 0;
    int read_length;
    int index;

    PROTOCOL_READ_UINT16( context, &start_index, 0 );

    length = context->payload[2];

    end_byte_index = ((int)start_index + (int)length - 1);

    number_of_bytes_received = UART_get_read_count();

    read_buffer = UART_get_read_buffer();

    if(end_byte_index >= number_of_bytes_received)
    {
        end_byte_index = number_of_bytes_received - 1;
    }

    read_length = end_byte_index - start_index + 1;

    protocol_set_response_payload_length(context, read_length + 1);

    context->payload[0] = (uint8_t)read_length;

    for(index = 0; index < read_length ; index++)
    {
        context->payload[index+1] = read_buffer[((int)start_index) + index];
    }
}

/**
 * @brief       Write bytes into the UART write buffer
 *
 *
 */

static void uart_put_into_write_buffer(struct protocol_context* context)
{
    uint8_t* write_buffer = UART_get_write_buffer();
    bool reset_write_buffer;
    uint16_t start_index;
    uint8_t length;
    int index = 0;

    reset_write_buffer = (context->payload[0] != 0);

    if(reset_write_buffer)
    {
        UART_reset_write_buffer();
    }

    PROTOCOL_READ_UINT16(context, &start_index, 1);

    length = context->payload[3];

    for( index = 0 ; index < (int)length ; index++ )
    {
        write_buffer[(int)start_index + (int)index] = context->payload[index + 4];
        UART_increment_write_count();
    }

    protocol_set_response_payload_length(context, 1);

    context->payload[0] = 0;
}

/**
 *
 * @brief:          Single transfer via UART. The test bench transmits the provided number
 *                  of bytes and read the target number of bytes within the specified timeout.
 *
 */

static void uart_single_transfer(struct protocol_context* context)
{
    uint8_t count_limit_mode;
    uint8_t termination_character_mode;
    uint16_t count_limit;
    uint8_t termination_character;
    uint32_t timeout_us;
    uint8_t status;
    uint16_t rx_count;


    count_limit_mode = context->payload[0];
    termination_character_mode = context->payload[1];

    PROTOCOL_READ_UINT16(context, &count_limit, 2);

    termination_character = context->payload[4];

    PROTOCOL_READ_UINT32(context, &timeout_us, 5);

    UART_set_read_timeout_us(timeout_us);

    if(count_limit_mode && !termination_character_mode)
    {
        UART_reset_read_buffer();
        UART_start();
        UART_transfer_till_read_count(count_limit);
        UART_stop();
    }

    if(termination_character_mode && !count_limit_mode)
    {
        UART_reset_read_buffer();
        UART_start();
        UART_transfer_until_char(termination_character);
        UART_stop();
    }

    status = UART_get_status();

    rx_count = (uint16_t)UART_get_read_count();

    protocol_set_response_payload_length(context, 3);

    PROTOCOL_WRITE_UINT16(context, rx_count, 0);

    context->payload[2] = status;
}

/**
 *  @brief      Configures the overall settings of the consecutive transfers such as
 *              character mode and the transfer termination character.
 *
 */

static void uart_consecutive_transfers_configure(struct protocol_context* context)
{
    uint8_t number_of_transfers;
    bool character_termination_mode;
    uint8_t termination_character;

    number_of_transfers = context->payload[0];
    character_termination_mode = context->payload[1];
    termination_character = context->payload[2];

    UART_consecutive_transfer_set_transfer_count(number_of_transfers);
    UART_consecutive_transfer_set_termination_mode(character_termination_mode);
    UART_consecutive_transfer_set_termination_char(termination_character);

    protocol_set_response_payload_length(context, 1);

    context->payload[0] = 0;
}

/**
 * @brief:      Sets the target read count, receive timeout, next transfer time and
 *              the number of bytes to write
 */

static void uart_configure_consecutive_transfer_instance(struct protocol_context* context)
{
    uint8_t consecutive_transfer_index;
    uint32_t timeout_us;
    uint32_t pause_time_us;
    uint16_t write_count;
    uint16_t target_read_count;

    consecutive_transfer_index = context->payload[0];

    PROTOCOL_READ_UINT32( context, &timeout_us, 1 );
    PROTOCOL_READ_UINT32( context, &pause_time_us, 5 );
    PROTOCOL_READ_UINT16( context, &write_count, 9 );
    PROTOCOL_READ_UINT16( context, &target_read_count, 11 );

    UART_consecutive_transfer_set_timeout(consecutive_transfer_index, timeout_us);
    UART_consecutive_transfer_set_pause_time(consecutive_transfer_index, pause_time_us);
    UART_consecutive_transfer_set_write_count(consecutive_transfer_index, write_count);
    UART_consecutive_transfer_set_target_read_count(consecutive_transfer_index, target_read_count);

    protocol_set_response_payload_length(context, 1);

    context->payload[0] = 0;
}

/**
 *
 * @brief       Writes bytes to a consecutive transfer instance starting from a given index
 *              and for a give length.
 */

static void uart_consecutive_transfer_instance_write_bytes(struct protocol_context* context)
{
    int transfer_instance;
    int start_index;
    int end_index;
    int length;
    int index;
    uint8_t* data = &context->payload[3];

    transfer_instance = (int)context->payload[0];
    start_index = (int)context->payload[1];
    length = (int)context->payload[2];

    end_index = start_index + length - 1;

    for(index = start_index ; index <= end_index ; index++)
    {
        UART_consecutive_transfer_write_byte(transfer_instance, *data, index);
        data++;
    }

    protocol_set_response_payload_length(context, 1);

    context->payload[0] = 0;
}

static void uart_consecutive_transfer_start(struct protocol_context* context)
{
    if(context->payload[0])
    {
        UART_consecutive_transfer_start();
    }

    UART_consecutive_transfer_stop();

    protocol_set_response_payload_length(context, 1);

    context->payload[0] = UART_get_status();
}

static void uart_consecutive_transfer_instance_get_read_count_and_status(struct protocol_context* context)
{
    uint16_t length;
    uint8_t status;

    length = UART_consecutive_transfer_get_read_count(context->payload[0]);
    status = UART_consecutive_transfer_get_status(context->payload[0]);

    protocol_set_response_payload_length(context, 3);

    PROTOCOL_WRITE_UINT16( context, length ,0 );

    context->payload[2] = status;
}

static void uart_consecutive_transfer_instance_read_bytes(struct protocol_context* context)
{
    int transfer_index;
    int start_index;
    int length;
    uint8_t* data = &context->payload[1];

    int index;

    transfer_index = (int)context->payload[0];
    start_index = (int)context->payload[1];
    length = (int)context->payload[2];

    for(index = 0 ; index < length ; index++)
    {
        *data = UART_consecutive_transfer_read_byte(transfer_index, start_index + index);
        data++;
    }

    protocol_set_response_payload_length(context, length + 1);

    context->payload[0] = (uint8_t)length;
}

/**
 *
 * @brief Starts/stops capture on specific channel.
 *
 */

static void capture_control(struct protocol_context* context)
{
    uint8_t channel;
    bool start;
    bool stop;

    channel = context->payload[0];
    start = (bool)context->payload[1];
    stop = (bool)context->payload[2];

    if(start)
    {
        Capture_start(channel);
    }

    if(stop)
    {
        Capture_stop(channel);
    }

    protocol_set_response_payload_length(context, 1);

    context->payload[0] = 0;
}

/**
 * @brief       Gets the following capture data for the specified channel.
 *              1. frequency
 *              2. on time (nanoseconds)
 *              3. off time (nanoseconds)
 *              4. period (nanoseconds)
 *              5. duty cycle ( 0 to 1 )
 */

static void capture_get_data(struct protocol_context* context)
{
    uint8_t channel;

    uint32_t frequency;
    uint32_t on_time_ns;
    uint32_t off_time_ns;
    uint32_t period_ns;
    static float duty;

    channel = context->payload[0];

    frequency = Capture_get_frequency(channel);
    on_time_ns = Capture_get_on_time_ns(channel);
    off_time_ns = Capture_get_off_time_ns(channel);
    period_ns = Capture_get_period_ns(channel);
    duty = Capture_get_duty_cycle(channel);

    protocol_set_response_payload_length(context, 20);

    PROTOCOL_WRITE_UINT32( context, frequency, 0 );
    PROTOCOL_WRITE_UINT32( context, on_time_ns, 4 );
    PROTOCOL_WRITE_UINT32( context, off_time_ns, 8 );
    PROTOCOL_WRITE_UINT32( context, period_ns, 12 );
    PROTOCOL_WRITE_FLOAT( context, duty, 16 );
}


static void hsadc_configure(struct protocol_context* context)
{
    uint8_t sampling_rate;
    uint8_t channels;
    uint8_t status;

    (void)sampling_rate;
    (void)channels;
    (void)status;

    status = 1;

    sampling_rate = context->payload[0];
    channels = context->payload[1];

    HSADC_set_sampling_rate(sampling_rate);

    protocol_set_response_payload_length(context, 1);

    context->payload[0] = 0;
}


static void hsadc_control(struct protocol_context* context)
{
    uint8_t start_conversion;

    start_conversion = context->payload[0];

    if(start_conversion)
    {
        HSADC_acquire();
    }

    protocol_set_response_payload_length(context, 1);

    context->payload[0] = 0;
}

static void hsadc_get_samples(struct protocol_context* context)
{
    uint8_t channel;
    uint16_t start_index;
    uint16_t length;
    uint16_t index;
    uint16_t current_sample;

    channel = context->payload[0];
    PROTOCOL_READ_UINT16( context, &start_index, 1 );
    length = (uint16_t)context->payload[3];

    protocol_set_response_payload_length(context, (1 + (length*2)));

    PROTOCOL_WRITE_UINT8( context, length, 0 );

    for( index = 0 ; index < length; index++ )
    {
        current_sample = HSADC_get_sample(channel, start_index + index);
        PROTOCOL_WRITE_UINT16( context, current_sample, (2*index + 1) );
    }
}

/**
 *  @brief      Configures the USER I2C interface. Sets the mode and the I2C address.
 *
 */

static void i2c_configure(struct protocol_context* context)
{
    uint8_t mode;
    uint8_t address;
    uint8_t status;
    uint32_t timeout;

    status = 0;

    mode = context->payload[0];
    address = context->payload[1];
    PROTOCOL_READ_UINT32( context, &timeout, 2 );

    if(address > 0x7F)
    {
        status = 1;
    }
    else 
    {
        I2C_set_mode(mode);
        I2C_set_address(address);
        I2C_set_transfer_timeout_us(timeout);
    }

    protocol_set_response_payload_length(context, 1);

    context->payload[0] = status;
}

static void i2c_configure_single_transfer(struct protocol_context* context)
{
    uint16_t write_count;
    uint16_t read_count;

    PROTOCOL_READ_UINT16(context, &write_count, 0);
    PROTOCOL_READ_UINT16(context, &read_count, 2);

    I2C_set_read_count(read_count);
    I2C_set_write_count(write_count);

    protocol_set_response_payload_length(context, 1);

    context->payload[0] = 0;
}

static void i2c_write_buffer_put(struct protocol_context* context)
{
    int start_index;
    int length;
    uint8_t* payload;
    int index;

    PROTOCOL_READ_UINT16( context, ((uint16_t*)&start_index), 0);

    length = (int)context->payload[2];
    payload = &context->payload[3];

    for(index = 0; index < length; index++)
    {
        I2C_write_buffer_put_byte(payload[index], start_index + index);
    }

    protocol_set_response_payload_length(context, 1);
    context->payload[0] = 0;
}

static void i2c_read_buffer_get(struct protocol_context* context)
{
    int start_index;
    int length;
    int index;

    PROTOCOL_READ_UINT16( context, ((uint16_t*)&start_index), 0);

    length = (int)context->payload[2];

    for(index = 0; index < length; index++)
    {
        context->payload[index + 1] = I2C_read_buffer_get_byte(start_index + index);
    }

    protocol_set_response_payload_length(context, length + 1);
    context->payload[0] = length;   
}

static void i2c_single_transfer(struct protocol_context* context)
{
    uint8_t start;
    uint8_t status;

    start = context->payload[0];

    if(start)
    {
        I2C_transfer_start();
    }

    status = I2C_transfer_status();

    protocol_set_response_payload_length(context, 1);

    context->payload[0] = status;
}

static void i2c_consecutive_transfer_configure(struct protocol_context* context)
{
    uint8_t number_of_transfers;
    uint8_t transfer_index;
    uint16_t transfer_read_count;
    uint16_t transfer_write_count;
    uint32_t pause_time_us;

    number_of_transfers = context->payload[0];
    transfer_index = context->payload[1];
    PROTOCOL_READ_UINT16( context, &transfer_read_count, 2 );
    PROTOCOL_READ_UINT16( context, &transfer_write_count, 4 );
    PROTOCOL_READ_UINT32( context, &pause_time_us, 6 );

    I2C_consecutive_transfer_set_count(number_of_transfers);
    I2C_consecutive_transfer_set_read_count(transfer_index, transfer_read_count);
    I2C_consecutive_transfer_set_write_count(transfer_index, transfer_write_count);
    I2C_consecutive_transfer_set_pause_time_us(transfer_index, pause_time_us);

    protocol_set_response_payload_length(context, 1);

    context->payload[0] = 0;
}

static void i2c_consecutive_transfer_write_buffer_put(struct protocol_context* context)
{
    int transfer_index;
    int start_index;
    int length;
    uint8_t* payload;
    int index;

    transfer_index = (int)context->payload[0];

    PROTOCOL_READ_UINT16( context, ((uint16_t*)&start_index), 1);

    length = (int)context->payload[3];
    payload = &context->payload[4];

    for(index = 0; index < length; index++)
    {
        I2C_consecutive_transfer_write_buffer_put(transfer_index, payload[index], start_index + index);
    }

    protocol_set_response_payload_length(context, 1);
    context->payload[0] = 0;
}

static void i2c_consecutive_transfer_read_buffer_get(struct protocol_context* context)
{
    int transfer_index;
    int start_index;
    int length;
    int index;

    transfer_index = (int)context->payload[0];

    PROTOCOL_READ_UINT16( context, ((uint16_t*)&start_index), 1);

    length = (int)context->payload[3];

    for(index = 0; index < length; index++)
    {
        context->payload[index + 1] = I2C_consecutive_transfer_read_buffer_get(transfer_index, start_index + index);
    }

    protocol_set_response_payload_length(context, length + 1);
    context->payload[0] = length;
}

static void i2c_consecutive_transfer_start(struct protocol_context* context)
{
    int number_of_transfers;
    int index;

    number_of_transfers = (int)context->payload[0];

    I2C_consecutive_transfer_start();

    for( index = 0 ; index < number_of_transfers ; index++ )
    {
        context->payload[index + 1] = I2C_consecutive_transfer_status(index);
    }

    protocol_set_response_payload_length(context, number_of_transfers + 1);
}

static void spi_configure(struct protocol_context* context)
{
    uint32_t bitrate;
    uint32_t mode;
    uint32_t order;
    uint32_t cs_polarity;

    PROTOCOL_READ_UINT32( context, &bitrate, 0 );
    mode = (uint32_t)context->payload[4];
    order = (uint32_t)context->payload[5];
    cs_polarity = (uint32_t)context->payload[6];

    SPI_set_bit_order(order);
    SPI_set_bitrate(bitrate);
    SPI_set_mode(mode);
    SPI_set_cs_polarity(cs_polarity);

    protocol_set_response_payload_length(context, 1);

    context->payload[0] = 0;
}

static void spi_configure_single_transfer(struct protocol_context* context)
{
    uint16_t transfer_count;

    PROTOCOL_READ_UINT16(context, &transfer_count, 0);

    SPI_set_transfer_count(transfer_count);

    protocol_set_response_payload_length(context, 1);

    context->payload[0] = 0;
}

static void spi_write_buffer_put(struct protocol_context* context)
{
    int start_index;
    int length;
    uint8_t* payload;
    int index;

    PROTOCOL_READ_UINT16( context, ((uint16_t*)&start_index), 0);

    length = (int)context->payload[2];
    payload = &context->payload[3];

    for(index = 0; index < length; index++)
    {
        SPI_write_buffer_put(payload[index], start_index + index);
    }

    protocol_set_response_payload_length(context, 1);
    context->payload[0] = 0;
}

static void spi_read_buffer_get(struct protocol_context* context)
{
    int start_index;
    int length;
    int index;

    PROTOCOL_READ_UINT16( context, ((uint16_t*)&start_index), 0);

    length = (int)context->payload[2];

    for(index = 0; index < length; index++)
    {
        context->payload[index + 1] = SPI_read_buffer_get(start_index + index);
    }

    protocol_set_response_payload_length(context, length + 1);
    context->payload[0] = length;   
}

static void spi_single_transfer(struct protocol_context* context)
{
    uint8_t start;

    start = context->payload[0];

    if(start)
    {
        SPI_start_transfer();
    }

    protocol_set_response_payload_length(context, 1);

    context->payload[0] = 0;
}

static void spi_consecutive_transfer_configure(struct protocol_context* context)
{
    uint8_t number_of_transfers;
    uint8_t transfer_index;
    uint16_t transfer_count;
    uint32_t pause_time_us;

    number_of_transfers = context->payload[0];
    transfer_index = context->payload[1];
    PROTOCOL_READ_UINT16( context, &transfer_count, 2 );
    PROTOCOL_READ_UINT32( context, &pause_time_us, 4 );

    SPI_consecutive_transfer_set_count(number_of_transfers);
    SPI_consecutive_transfer_set_transfer_count(transfer_index, transfer_count);
    SPI_consecutive_transfer_set_pause_interval(transfer_index, pause_time_us);

    protocol_set_response_payload_length(context, 1);

    context->payload[0] = 0;
}

static void spi_consecutive_transfer_write_buffer_put(struct protocol_context* context)
{
    int transfer_index;
    int start_index;
    int length;
    uint8_t* payload;
    int index;

    transfer_index = (int)context->payload[0];

    PROTOCOL_READ_UINT16( context, ((uint16_t*)&start_index), 1);

    length = (int)context->payload[3];
    payload = &context->payload[4];

    for(index = 0; index < length; index++)
    {
        SPI_consecutive_transfer_write_byte(transfer_index, payload[index], start_index + index);
    }

    protocol_set_response_payload_length(context, 1);
    context->payload[0] = 0;
}

static void spi_consecutive_transfer_read_buffer_get(struct protocol_context* context)
{
    int transfer_index;
    int start_index;
    int length;
    int index;

    transfer_index = (int)context->payload[0];

    PROTOCOL_READ_UINT16( context, ((uint16_t*)&start_index), 1);

    length = (int)context->payload[3];

    for(index = 0; index < length; index++)
    {
        context->payload[index + 1] = SPI_consecutive_transfer_read_byte(transfer_index, start_index + index);
    }

    protocol_set_response_payload_length(context, length + 1);
    context->payload[0] = length;
}

static void spi_consecutive_transfer_start(struct protocol_context* context)
{
    int dummy;

    (void)dummy;

    dummy = (int)context->payload[0];

    SPI_consecutive_transfer_start();

    protocol_set_response_payload_length(context, 1);

    context->payload[0] = 0;
}

static void gpio_set_pin_state(struct protocol_context* context)
{
    int pin;
    bool state;

    pin = (int)context->payload[0];
    state = (bool)context->payload[1];

    GPIO_set_output_state(pin, state);

    protocol_set_response_payload_length(context, 1);

    context->payload[0] = 0;
}

static void gpio_get_pin_state(struct protocol_context* context)
{
    int pin;
    bool state;

    pin = (int)context->payload[0];

    state = GPIO_get_input_state(pin);

    protocol_set_response_payload_length(context, 1);

    context->payload[0] = (uint8_t)state;
}

static void gpio_write_port(struct protocol_context* context)
{
    uint8_t port;

    port = (uint8_t)context->payload[0];

    GPIO_write_port(port);

    protocol_set_response_payload_length(context, 1);

    context->payload[0] = 0;
}

static void gpio_read_port(struct protocol_context* context)
{
    uint8_t port;

    port = GPIO_read_port();

    protocol_set_response_payload_length(context, 1);

    context->payload[0] = port;
}

static void gpio_pattern_generator_configure(struct protocol_context* context)
{
    uint16_t pattern_length;
    uint32_t pattern_generation_rate;

    PROTOCOL_READ_UINT16( context, &pattern_length, 0 );
    PROTOCOL_READ_UINT32( context, &pattern_generation_rate, 2 );

    GPIO_set_pattern_generation_length(pattern_length);
    GPIO_set_pattern_generation_rate(pattern_generation_rate);

    protocol_set_response_payload_length(context, 1);

    context->payload[0] = 0;
}

static void gpio_pattern_generator_write_output_sample(struct protocol_context* context)
{
    int start_index;
    int length;
    int index;
    uint8_t* pattern;
    
    PROTOCOL_READ_UINT16( context, ((uint16_t*)&start_index), 0 );

    length = (uint8_t)context->payload[2];

    pattern = &context->payload[3];

    for( index = 0 ; index < length ; index++ )
    {
        GPIO_set_output_pattern_at_index( start_index + index, pattern[index] );
    }

    protocol_set_response_payload_length(context, 1);

    context->payload[0] = 0;
}

static void gpio_pattern_generator_read_input_sample(struct protocol_context* context)
{
    int start_index;
    int length;
    int index;
    
    PROTOCOL_READ_UINT16( context, ((uint16_t*)&start_index), 0 );

    length = (int)context->payload[2];

    protocol_set_response_payload_length(context, (length + 1));

    context->payload[0] = (uint8_t)length;

    for( index = 0 ; index < length ; index++ )
    {
        context->payload[index + 1] = GPIO_get_input_pattern_at_index(start_index + index);
    }
}

static void gpio_pattern_generator_start(struct protocol_context* context)
{
    bool start;

    start = (bool)context->payload[0];

    if(start)
    {
        GPIO_pattern_generation_start();
    }

    protocol_set_response_payload_length(context, 1);

    context->payload[0] = 0;
}

static void gpio_pattern_generator_stop(struct protocol_context* context)
{
    bool stop;

    stop = (bool)context->payload[0];

    if(stop)
    {
        GPIO_pattern_generation_stop();
    }

    protocol_set_response_payload_length(context, 1);

    context->payload[0] = 0;
}
