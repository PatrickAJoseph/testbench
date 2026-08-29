#include "device.h"

#include "ti_sdk_dl_config.h"

#include "clocks.h"
#include "user_uart.h"
#include "user_i2c.h"
#include "capture.h"
#include "pwm.h"
#include "hsadc.h"
#include "user_spi.h"
#include "user_gpio.h"
#include "user_timer.h"
#include "configurations.h"
#include "protocol_app_interface.h"
#include "ads1115.h"

uint8_t ads1115_regs[4];

int main()
{
    Device_Init();

    SYSCFG_DL_init();

    Clocks_init();
    UART_init();
    Capture_init();
    PWM_init();
    HSADC_init();
    SPI_init();
    I2C_init();
    GPIO_init();
    Timer_init();
    ADS1115_init();

    configuration_init();

    protocol_app_interface_init();

    while(1)
    {
        ads1115_regs[0] = 0x0A;
        ADS1115_write_registers(2, ads1115_regs, 1);
        DL_Common_delayCycles(900000);
        ADS1115_read_registers(2, ads1115_regs, 1);
        DL_Common_delayCycles(900000);
    }

    return 0;
}