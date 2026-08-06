
#include "device.h"
#include "ti_sdk_dl_config.h"

#include "clocks.h"
#include "user_uart.h"
#include "capture.h"
#include "pwm.h"
#include "protocol_app_interface.h"

int main()
{
    Device_Init();
    SYSCFG_DL_init();

    Clocks_init();
    UART_init();
    Capture_init();
    PWM_init();

    protocol_app_interface_init();

    DL_MCPWM_enableTBCLK();

    while(1)
    {
        protocol_app_interface_process();        
    }

    return 0;
}
