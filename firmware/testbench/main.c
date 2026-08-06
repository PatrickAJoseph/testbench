
#include "device.h"
#include "ti_sdk_dl_config.h"

#include "clocks.h"
#include "user_uart.h"
#include "capture.h"
#include "pwm.h"
#include "hsadc.h"
#include "user_spi.h"

#include "protocol_app_interface.h"

int main()
{
    int index;

    Device_Init();
    SYSCFG_DL_init();

    Clocks_init();
    UART_init();
    Capture_init();
    PWM_init();
    HSADC_init();
    SPI_init();
    SPI_set_bitrate(2500000);

    SPI_set_transfer_count(64);

    for( index = 0 ; index < 64 ; index++ )
    {
        SPI_write_buffer_put(index, index);
    }

    protocol_app_interface_init();

    while(1)
    {
        SPI_start_transfer();
    }

    return 0;
}
