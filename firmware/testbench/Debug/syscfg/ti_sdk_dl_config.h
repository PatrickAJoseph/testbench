
/* This file is AUTO GENERATED, DO NOT EDIT manually */


/*
 * Copyright (c) 2024, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef ti_sdk_dl_config_h
#define ti_sdk_dl_config_h

#include <driverlib.h>
#include <cmsis_compiler.h>

#define SYSCONFIG_WEAK __WEAK


#ifdef __cplusplus
extern "C" {
#endif

void SYSCFG_DL_init(void);
void SYSCFG_DL_initPower(void);
void SYSCFG_DL_Pinmux_init(void);
void SYSCFG_DL_FLASH_init(void);


/* Defines for Clock Frequencies */
#define CPUCLK_FREQ_HZ                                                  200000000
#define MCLK_FREQ_HZ                                                    200000000
#define SEMIMCLK_FREQ_HZ                                                100000000
#define QTRMCLK_FREQ_HZ                                                  50000000
#define CANCLK_FREQ_HZ                                                  100000000
#define SYSOSC_FREQ_HZ                                                   32000000
#define LFOSC_FREQ_HZ                                                       32000
#define HFCLK_FREQ_HZ                                                    25000000
#define SYSPLLCLK0_FREQ_HZ                                              200000000
#define SYSPLLCLK1_FREQ_HZ                                              200000000


/*
Macros to call DL_Common_delayCycles to achieve a delay in microseconds / milliseconds.
These macros convert the desired delay in us/ms to the count value expected
by the function.
Note that the function DL_Common_delayCycles only guarantees at least this
number of cycles will be delayed, not that exactly this number of cycles will be
delayed. Please refer to the function documentation for more details.
*/
#define DEVICE_DELAY_US(us)   DL_Common_delayCycles((uint32_t)(us) * (CPUCLK_FREQ_HZ / 1000000))
#define DEVICE_DELAY_MS(ms)   DL_Common_delayCycles((uint32_t)(ms) * (CPUCLK_FREQ_HZ / 1000))




/* Port definition for Pin Group CAPTURE_DIN_GPIO_GROUP */
#define CAPTURE_DIN_GPIO_GROUP_PORT                                      (GPIO0)
#define CAPTURE_DIN_GPIO_GROUP_PORT_BASE                            (GPIO0_BASE)

/* Defines for CAPTURE_CH0_DIN: GPIO0.2 with pinCMx PA2 on package pin 14 */
#define CAPTURE_DIN_GPIO_GROUP_CAPTURE_CH0_DIN_PIN              (DL_GPIO_PIN(2))
#define CAPTURE_DIN_GPIO_GROUP_CAPTURE_CH0_DIN_IOMUX           (IOMUX_PINCM_PA2)
/* Defines for CAPTURE_CH1_DIN: GPIO0.25 with pinCMx PA25 on package pin 62 */
#define CAPTURE_DIN_GPIO_GROUP_CAPTURE_CH1_DIN_PIN             (DL_GPIO_PIN(25))
#define CAPTURE_DIN_GPIO_GROUP_CAPTURE_CH1_DIN_IOMUX          (IOMUX_PINCM_PA25)


/* Defines for CAPTURE_INPUTXBAR1 */
#define CAPTURE_INPUTXBAR1                                        DL_XBAR_INPUT1
/* Defines for CAPTURE_INPUTXBAR2 */
#define CAPTURE_INPUTXBAR2                                        DL_XBAR_INPUT2



#define HSADC_DMA_DMA_INST                      DMA0
#define HSADC_DMA_CHANNEL_ID                    0
#define HSADC_DMA_CHANNEL_INTERRUPT_MASK        DL_DMA_FULL_TRANSFER_COMPLETE_INTERRUPT_MASK(0)
#define HSADC_DMA_CHANNEL_EARLY_INTERRUPT_MASK  DL_DMA_HALF_TRANSFER_COMPLETE_INTERRUPT_MASK(0)


#define HSADC_INST                                                           ADC0
#define HSADC_BASE                                                      ADC0_BASE
#define HSADC_RESULT_INST                                              ADC0RESULT
#define HSADC_RESULT_BASE                                         ADC0RESULT_BASE
#define HSADC_SOC0                                             DL_ADC_SOC_NUMBER0
#define HSADC_CHANNEL_SOC0                                      DL_ADC_CH_ADCIN15
#define HSADC_SOC1                                             DL_ADC_SOC_NUMBER1
#define HSADC_CHANNEL_SOC1                                      DL_ADC_CH_ADCIN12
#define HSADC_SEQ0                                             DL_ADC_SEQ_NUMBER1



/* Defines for CAPTURE_ECAP0 */
#define CAPTURE_ECAP0_INST                                                 ECAP0
#define CAPTURE_ECAP0_BASE                                            ECAP0_BASE
/* Defines for CAPTURE_ECAP1 */
#define CAPTURE_ECAP1_INST                                                 ECAP1
#define CAPTURE_ECAP1_BASE                                            ECAP1_BASE



/* Defines for PWM_CH0 */
#define PWM_CH0_INST                                                      MCPWM0
#define PWM_CH0_BASE                                                 MCPWM0_BASE
#define PWM_CH0_PERIOD                                                     10000
#define PWM_CH0_TBPHS                                                          0
#define PWM_CH0_PWM1_CMPA                                                   1000
#define PWM_CH0_PWM1_CMPB                                                      0
#define PWM_CH0_PWM2_CMPA                                                      0
#define PWM_CH0_PWM2_CMPB                                                      0
#define PWM_CH0_PWM3_CMPA                                                      0
#define PWM_CH0_PWM3_CMPB                                                      0
#define PWM_CH0_CMPC                                                           0
#define PWM_CH0_CMPD                                                           0
/* Defines for PWM_CH1 */
#define PWM_CH1_INST                                                      MCPWM4
#define PWM_CH1_BASE                                                 MCPWM4_BASE
#define PWM_CH1_PERIOD                                                         0
#define PWM_CH1_TBPHS                                                          0
#define PWM_CH1_PWM1_CMPA                                                      0
#define PWM_CH1_PWM1_CMPB                                                      0
#define PWM_CH1_PWM2_CMPA                                                      0
#define PWM_CH1_PWM2_CMPB                                                      0
#define PWM_CH1_PWM3_CMPA                                                      0
#define PWM_CH1_PWM3_CMPB                                                      0
#define PWM_CH1_CMPC                                                           0
#define PWM_CH1_CMPD                                                           0
/* Defines for PWM_CH2 */
#define PWM_CH2_INST                                                      MCPWM3
#define PWM_CH2_BASE                                                 MCPWM3_BASE
#define PWM_CH2_PERIOD                                                         0
#define PWM_CH2_TBPHS                                                          0
#define PWM_CH2_PWM1_CMPA                                                      0
#define PWM_CH2_PWM1_CMPB                                                      0
#define PWM_CH2_PWM2_CMPA                                                      0
#define PWM_CH2_PWM2_CMPB                                                      0
#define PWM_CH2_PWM3_CMPA                                                      0
#define PWM_CH2_PWM3_CMPB                                                      0
#define PWM_CH2_CMPC                                                           0
#define PWM_CH2_CMPD                                                           0
/* Defines for PWM_CH3 */
#define PWM_CH3_INST                                                      MCPWM2
#define PWM_CH3_BASE                                                 MCPWM2_BASE
#define PWM_CH3_PERIOD                                                         0
#define PWM_CH3_TBPHS                                                          0
#define PWM_CH3_PWM1_CMPA                                                      0
#define PWM_CH3_PWM1_CMPB                                                      0
#define PWM_CH3_PWM2_CMPA                                                      0
#define PWM_CH3_PWM2_CMPB                                                      0
#define PWM_CH3_PWM3_CMPA                                                      0
#define PWM_CH3_PWM3_CMPB                                                      0
#define PWM_CH3_CMPC                                                           0
#define PWM_CH3_CMPD                                                           0
/* Defines for HSADC_TRIGGER_PWM */
#define HSADC_TRIGGER_PWM_INST                                            MCPWM1
#define HSADC_TRIGGER_PWM_BASE                                       MCPWM1_BASE
#define HSADC_TRIGGER_PWM_PERIOD                                            9999
#define HSADC_TRIGGER_PWM_TBPHS                                                0
#define HSADC_TRIGGER_PWM_PWM1_CMPA                                            0
#define HSADC_TRIGGER_PWM_PWM1_CMPB                                            0
#define HSADC_TRIGGER_PWM_PWM2_CMPA                                            0
#define HSADC_TRIGGER_PWM_PWM2_CMPB                                            0
#define HSADC_TRIGGER_PWM_PWM3_CMPA                                            0
#define HSADC_TRIGGER_PWM_PWM3_CMPB                                            0
#define HSADC_TRIGGER_PWM_CMPC                                                 0
#define HSADC_TRIGGER_PWM_CMPD                                                 0



/* Defines for RS485_UART */
#define RS485_UART_INST                                             UC4_INST_PTR
#define RS485_UART_INST_FREQUENCY                                      100000000
#define GPIO_RS485_UART_RX_PORT                                            GPIO0
#define GPIO_RS485_UART_TX_PORT                                            GPIO0
#define GPIO_RS485_UART_RX_PIN                                     DL_GPIO_PIN_1
#define GPIO_RS485_UART_TX_PIN                                     DL_GPIO_PIN_0
#define GPIO_RS485_UART_IOMUX_RX                               (IOMUX_PINCM_PA1)
#define GPIO_RS485_UART_IOMUX_TX                               (IOMUX_PINCM_PA0)
#define GPIO_RS485_UART_IOMUX_RX_FUNC                  IOMUX_PA1_UC4_RX_SCL_SCLK
#define GPIO_RS485_UART_IOMUX_TX_FUNC                  IOMUX_PA0_UC4_TX_SDA_PICO
#define RS485_UART_BAUD_RATE                                            (115200)
#define RS485_UART_IBRD_100_MHZ_115200_BAUD                                 (54)
#define RS485_UART_FBRD_100_MHZ_115200_BAUD                                 (16)
/* Defines for USER_UART */
#define USER_UART_INST                                              UC2_INST_PTR
#define USER_UART_INST_FREQUENCY                                       100000000
#define GPIO_USER_UART_RX_PORT                                             GPIO0
#define GPIO_USER_UART_TX_PORT                                             GPIO0
#define GPIO_USER_UART_RX_PIN                                     DL_GPIO_PIN_23
#define GPIO_USER_UART_TX_PIN                                     DL_GPIO_PIN_22
#define GPIO_USER_UART_IOMUX_RX                               (IOMUX_PINCM_PA23)
#define GPIO_USER_UART_IOMUX_TX                               (IOMUX_PINCM_PA22)
#define GPIO_USER_UART_IOMUX_RX_FUNC                       IOMUX_PA23_UC2_RX_SCL
#define GPIO_USER_UART_IOMUX_TX_FUNC                       IOMUX_PA22_UC2_TX_SDA
#define USER_UART_BAUD_RATE                                             (115200)
#define USER_UART_IBRD_100_MHZ_115200_BAUD                                  (54)
#define USER_UART_FBRD_100_MHZ_115200_BAUD                                  (16)





/* Interrupt Defines */
#define DMA0_INT                                                    DMA0_INT_IRQn
#define DMA0_INT_Handler                                          DMA0_IRQHandler
#define CAPTURE_ECAP0_INT                                          ECAP0_INT_IRQn
#define CAPTURE_ECAP0_INT_Handler                                ECAP0_IRQHandler
#define Systick_INT                                                  SysTick_IRQn
#define Systick_INT_Handler                                       SysTick_Handler
#define RS485_UART_INT                                               UC4_INT_IRQn
#define RS485_UART_INT_Handler                                     UC4_IRQHandler
#define USER_UART_INT                                                UC2_INT_IRQn
#define USER_UART_INT_Handler                                      UC2_IRQHandler


void SYSCFG_DL_SYSCTL_CLK_init(void);
void SYSCFG_DL_SYSCTL_init(void);

void SYSCFG_DL_INPUTXBAR_init(void);
void SYSCFG_DL_DMA0_init(void);
void SYSCFG_DL_HSADC_init(void);
void SYSCFG_DL_CAPTURE_ECAP0_init(void);
void SYSCFG_DL_CAPTURE_ECAP1_init(void);
void SYSCFG_DL_PWM_CH0_init(void);
void SYSCFG_DL_PWM_CH1_init(void);
void SYSCFG_DL_PWM_CH2_init(void);
void SYSCFG_DL_PWM_CH3_init(void);
void SYSCFG_DL_HSADC_TRIGGER_PWM_init(void);
void SYSCFG_DL_RS485_UART_init(void);
void SYSCFG_DL_USER_UART_init(void);
void SYSCFG_DL_SYSTICK_init(void);
void SYSCFG_DL_INTERRUPT_init(void);


#ifdef __cplusplus
}
#endif

#endif /* ti_sdk_dl_config_h */