
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

/*
 *  ======== SYSCFG_DL_init ========
 *  Perform any initialization needed before using any board APIs
 */

#include "ti_sdk_dl_config.h"

SYSCONFIG_WEAK void SYSCFG_DL_init(void)
{
    SYSCFG_DL_FLASH_init();
    SYSCFG_DL_SYSCTL_CLK_init();
    SYSCFG_DL_initPower();
    SYSCFG_DL_Pinmux_init();
    SYSCFG_DL_SYSCTL_init();
	SYSCFG_DL_INPUTXBAR_init();
    SYSCFG_DL_DMA0_init();
    SYSCFG_DL_HSADC_init();
    SYSCFG_DL_CAPTURE_ECAP0_init();
    SYSCFG_DL_CAPTURE_ECAP1_init();
    SYSCFG_DL_PWM_CH0_init();
    SYSCFG_DL_PWM_CH1_init();
    SYSCFG_DL_PWM_CH2_init();
    SYSCFG_DL_PWM_CH3_init();
    SYSCFG_DL_HSADC_TRIGGER_PWM_init();
    SYSCFG_DL_RS485_UART_init();
    SYSCFG_DL_USER_UART_init();
    SYSCFG_DL_SYSTICK_init();
    SYSCFG_DL_INTERRUPT_init();
}

SYSCONFIG_WEAK void SYSCFG_DL_initPower(void)
{
    DL_GPIO_reset(GPIO0);
    DL_GPIO_reset(GPIO1);
    DL_GPIO_reset(GPIO2);
    DL_GPIO_reset(GPIO3);

	DL_SYSCTL_resetPeripheral(DL_SYSCTL_RESET_XBAR);

    DL_ADC_reset(HSADC_INST);
    DL_SYSCTL_resetPeripheral(DL_SYSCTL_RESET_ECAP0);
    DL_SYSCTL_resetPeripheral(DL_SYSCTL_RESET_ECAP1);
    DL_MCPWM_disableTBCLK();
    DL_SYSCTL_resetPeripheral(DL_SYSCTL_RESET_PWM0);
    DL_SYSCTL_resetPeripheral(DL_SYSCTL_RESET_PWM4);
    DL_SYSCTL_resetPeripheral(DL_SYSCTL_RESET_PWM3);
    DL_SYSCTL_resetPeripheral(DL_SYSCTL_RESET_PWM2);
    DL_SYSCTL_resetPeripheral(DL_SYSCTL_RESET_PWM1);
    DL_UART_reset(RS485_UART_INST);
    DL_UART_reset(USER_UART_INST);


    DL_GPIO_enablePower(GPIO0);
    DL_GPIO_enablePower(GPIO1);
    DL_GPIO_enablePower(GPIO2);
    DL_GPIO_enablePower(GPIO3);

	DL_SYSCTL_enablePower(DL_SYSCTL_PWREN_XBAR);

    DL_ADC_enablePower(HSADC_INST);
    DL_SYSCTL_enablePower(DL_SYSCTL_PWREN_ECAP0);
    DL_SYSCTL_enablePower(DL_SYSCTL_PWREN_ECAP1);
    DL_SYSCTL_enablePower(DL_SYSCTL_PWREN_PWM0);
    DL_SYSCTL_enablePower(DL_SYSCTL_PWREN_PWM4);
    DL_SYSCTL_enablePower(DL_SYSCTL_PWREN_PWM3);
    DL_SYSCTL_enablePower(DL_SYSCTL_PWREN_PWM2);
    DL_SYSCTL_enablePower(DL_SYSCTL_PWREN_PWM1);
    DL_UART_enablePower(RS485_UART_INST);
    DL_UART_enablePower(USER_UART_INST);

}

SYSCONFIG_WEAK void SYSCFG_DL_Pinmux_init(void)
{
    DL_GPIO_initPeripheralAnalogFunction(IOMUX_PINCM_PC16_X1);
    DL_GPIO_initPeripheralAnalogFunction(IOMUX_PINCM_PC17_X2);

    DL_GPIO_initDigitalInput(CAPTURE_DIN_GPIO_GROUP_CAPTURE_CH0_DIN_IOMUX);

    DL_GPIO_initDigitalInput(CAPTURE_DIN_GPIO_GROUP_CAPTURE_CH1_DIN_IOMUX);



    // IOMUX Setting for MCPWM pins are done after the module initialization.
    // (part of SYSCFG_DL_<MCPWM_Instance>_init)

    DL_GPIO_initPeripheralOutputFunction(
        GPIO_RS485_UART_IOMUX_TX, GPIO_RS485_UART_IOMUX_TX_FUNC);
    DL_GPIO_initPeripheralInputFunction(
        GPIO_RS485_UART_IOMUX_RX, GPIO_RS485_UART_IOMUX_RX_FUNC);
    DL_GPIO_initPeripheralOutputFunction(
        GPIO_USER_UART_IOMUX_TX, GPIO_USER_UART_IOMUX_TX_FUNC);
    DL_GPIO_initPeripheralInputFunction(
        GPIO_USER_UART_IOMUX_RX, GPIO_USER_UART_IOMUX_RX_FUNC);

    DL_GPIO_initPeripheralAnalogFunction(IOMUX_PINCM_PA16);
    DL_GPIO_initPeripheralAnalogFunction(IOMUX_PINCM_PA17);
}

static const DL_SYSCTL_SYSPLLConfig gSYSPLLConfig = {
    .sysPLLRef   = DL_SYSCTL_SYSPLL_REF_HFCLK,
    .inputFreq   = DL_SYSCTL_SYSPLL_INPUT_FREQ_8_16_MHZ,
    .pDiv        = DL_SYSCTL_SYSPLL_PDIV_2,
    .qDiv        = 31,
    .enableCLK1  = DL_SYSCTL_SYSPLL_CLK1_ENABLE,
    .enableCLK0  = DL_SYSCTL_SYSPLL_CLK0_ENABLE,
    .rDivClk1    = DL_SYSCTL_SYSPLL_RDIVCLK1_DIV2,
    .rDivClk0    = DL_SYSCTL_SYSPLL_RDIVCLK0_DIV2,
};
SYSCONFIG_WEAK void  SYSCFG_DL_SYSCTL_CLK_init(void)
{
    DL_SYSCTL_setHFCLKSourceXTAL(255, false);
    DL_SYSCTL_configSYSPLL((DL_SYSCTL_SYSPLLConfig *)&gSYSPLLConfig);

    // Before switching to PLL output, step down to a lower frequency and gradually increase
    DL_SYSCTL_enablePLLDivider(DL_SYSCTL_PLL_DIVIDER_DIV4);
    DL_SYSCTL_switchMCLKfromSYSOSCtoHSCLK(DL_SYSCTL_HSCLK_SOURCE_SYSPLL);
    DL_SYSCTL_enablePLLDivider(DL_SYSCTL_PLL_DIVIDER_DIV2);
    DL_Common_delayCycles(20);
    while ((DL_SYSCTL_getClockStatus() & SYSCTL_CLKSTATUS_HSCLKMUX_MASK) != DL_SYSCTL_CLK_STATUS_MCLK_SOURCE_HSCLK);
    DL_SYSCTL_disablePLLDivider();
    DL_Common_delayCycles(20);
    while ((DL_SYSCTL_getClockStatus() & SYSCTL_CLKSTATUS_HSCLKMUX_MASK) != DL_SYSCTL_CLK_STATUS_MCLK_SOURCE_HSCLK);


    DL_SYSCTL_setMCLKDivider(DL_SYSCTL_MCLK_DIV_2_DIV_4);
    DL_SYSCTL_setCANCLKSource(DL_SYSCTL_CANCLK_SOURCE_SYSPLL_DIV2);
}

SYSCONFIG_WEAK void SYSCFG_DL_SYSCTL_init(void)
{
    DL_SYSCTL_setVREF(DL_SYSCTL_VREF_INTERNAL_3_3_V);

}

SYSCONFIG_WEAK void	SYSCFG_DL_INPUTXBAR_init()
{
	DL_XBAR_setInputXBAR(DL_XBAR_INPUT1, 2);
	DL_XBAR_setInputXBAR(DL_XBAR_INPUT2, 25);

}

static const DL_DMA_Config gHSADC_DMAConfig = {
    .transferMode   = DL_DMA_FULL_CH_REPEAT_SINGLE_TRANSFER_MODE,
    .extendedMode   = DL_DMA_NORMAL_MODE,
    .destIncrement  = DL_DMA_ADDR_INCREMENT,
    .srcIncrement   = DL_DMA_ADDR_UNCHANGED,
    .destWidth      = DL_DMA_WIDTH_WORD,
    .srcWidth       = DL_DMA_WIDTH_WORD,
    .trigger        = DL_DMA_TRIGGER_SOURCE_ADC0_DMA_TRIG1,
    .triggerType    = DL_DMA_TRIGGER_TYPE_EXTERNAL,
};

SYSCONFIG_WEAK void SYSCFG_DL_HSADC_DMA_init(void)
{
    DL_DMA_clearInterruptStatus(DMA0, DL_DMA_INTERRUPT_CHANNEL0);
    DL_DMA_enableInterrupt(DMA0, DL_DMA_INTERRUPT_CHANNEL0);
    DL_DMA_setTransferSize(DMA0, 0, 8192);
    DL_DMA_initChannel(DMA0, 0 , (DL_DMA_Config *) &gHSADC_DMAConfig);
    DL_DMA_enableAutoEnable(DMA0, DL_DMA_AUTOEN_DISABLE, 0);
}
SYSCONFIG_WEAK void SYSCFG_DL_DMA0_init(void){
    DL_DMA_setBurstSize(DMA0, DL_DMA_BURST_SIZE_8);
    DL_DMA_enableRoundRobinPriority(DMA0);
    SYSCFG_DL_HSADC_DMA_init();
}


static const DL_ADC_SocConfig gHSADC_SOC0_SocConfig = {
    .channel = DL_ADC_CH_ADCIN15,
    .enableComparator = false
};

static const DL_ADC_SocConfig gHSADC_SOC1_SocConfig = {
    .channel = DL_ADC_CH_ADCIN12,
    .enableComparator = false
};

static const DL_ADC_SeqNConfig gHSADC_SEQ1_SeqNConfig = {
    .enableSequencer = true,
    .sampleWindow = 15,
    .trigger = DL_ADC_TRIGGER_PWM1_SOCA,
    .socStartNumber = DL_ADC_SOC_NUMBER0,
    .enableSampleCapReset = false,
    .capResetVal = DL_ADC_SAMPLE_CAP_RESET_SELECT_VREFLO
};

static const DL_ADC_IntNConfig gHSADC_DMAINT1_IntNConfig = {
    .enableContinuousMode = true,
    .enableInterrupt = true,
    .trigger = DL_ADC_SOC_NUMBER1
};

SYSCONFIG_WEAK void SYSCFG_DL_HSADC_init(void)
{
    DL_ADC_setPrescaler(HSADC_INST, DL_ADC_CLOCK_DIVIDE_2_0);
    DL_ADC_socInit(HSADC_INST, DL_ADC_SOC_NUMBER0, (DL_ADC_SocConfig *)&gHSADC_SOC0_SocConfig);
    DL_ADC_socInit(HSADC_INST, DL_ADC_SOC_NUMBER1, (DL_ADC_SocConfig *)&gHSADC_SOC1_SocConfig);
    DL_ADC_setInterruptPulseMode(HSADC_INST, DL_ADC_PULSE_END_OF_CONV);
    DL_ADC_setInterruptCycleDelay(HSADC_INST, 0);
    DL_ADC_dmaInterruptInit(HSADC_INST, DL_ADC_DMAINT_NUMBER1, (DL_ADC_IntNConfig *)&gHSADC_DMAINT1_IntNConfig);
    DL_ADC_setSeqEndSOC(HSADC_INST, DL_ADC_SOC_NUMBER1);
    DL_ADC_setSeqPreemptMode(HSADC_INST, DL_ADC_SEQ_PREEMPT_DISABLE);
    DL_ADC_sequencerInit(HSADC_INST, DL_ADC_SEQ_NUMBER1, (DL_ADC_SeqNConfig *)&gHSADC_SEQ1_SeqNConfig);
}


static const DL_ECAP_Config gCAPTURE_ECAP0Config = {
    .modeSelect = DL_ECAP_OPERATING_MODE_CAPTURE,
    .captureModeConfig =
    {
        .input                              = DL_ECAP_INPUT_INPUTXBAR1,
        .prescalerValue                     = 0,
        .continouousOrOneShot               = DL_ECAP_CONTINUOUS_CAPTURE_MODE,
        .wrapOrStopAtEvent                  = DL_ECAP_EVENT_3,
        .enableCounterResetOnCaptureEvent1  = false,
        .enableCounterResetOnCaptureEvent2  = false,
        .enableCounterResetOnCaptureEvent3  = false,
        .enableCounterResetOnCaptureEvent4  = false,
        .captureEvent1Polarity              = DL_ECAP_EVENT_RISING_EDGE,
        .captureEvent2Polarity              = DL_ECAP_EVENT_FALLING_EDGE,
        .captureEvent3Polarity              = DL_ECAP_EVENT_RISING_EDGE,
        .captureEvent4Polarity              = DL_ECAP_EVENT_FALLING_EDGE,
        .resetCounter                       = false,
        .reArm                              = true
    },
    .syncConfig =
    {
        .ecapSyncInSel     = DL_ECAP_SYNC_IN_PULSE_SRC_DISABLE,
        .phaseShiftCount   = 0,
        .syncOutSelect     = DL_ECAP_SYNC_OUT_SYNCI,
        .enableLoadCounter = false
        //todo loadCounter in js file unused
    },
    .interruptsConfig =
    {
        .interruptSourceEnableMask = DL_ECAP_ISR_SOURCE_CEVT3,
        .dmaEventSelect            = DL_ECAP_EVENT_1
    },
    .emulationMode  = DL_ECAP_EMULATION_STOP
};

SYSCONFIG_WEAK void SYSCFG_DL_CAPTURE_ECAP0_init(void){
    DL_ECAP_init(CAPTURE_ECAP0_INST, (DL_ECAP_Config *)&gCAPTURE_ECAP0Config);
}


static const DL_ECAP_Config gCAPTURE_ECAP1Config = {
    .modeSelect = DL_ECAP_OPERATING_MODE_CAPTURE,
    .captureModeConfig =
    {
        .input                              = DL_ECAP_INPUT_INPUTXBAR2,
        .prescalerValue                     = 0,
        .continouousOrOneShot               = DL_ECAP_CONTINUOUS_CAPTURE_MODE,
        .wrapOrStopAtEvent                  = DL_ECAP_EVENT_3,
        .enableCounterResetOnCaptureEvent1  = false,
        .enableCounterResetOnCaptureEvent2  = false,
        .enableCounterResetOnCaptureEvent3  = false,
        .enableCounterResetOnCaptureEvent4  = false,
        .captureEvent1Polarity              = DL_ECAP_EVENT_RISING_EDGE,
        .captureEvent2Polarity              = DL_ECAP_EVENT_FALLING_EDGE,
        .captureEvent3Polarity              = DL_ECAP_EVENT_RISING_EDGE,
        .captureEvent4Polarity              = DL_ECAP_EVENT_FALLING_EDGE,
        .resetCounter                       = false,
        .reArm                              = true
    },
    .syncConfig =
    {
        .ecapSyncInSel     = DL_ECAP_SYNC_IN_PULSE_SRC_DISABLE,
        .phaseShiftCount   = 0,
        .syncOutSelect     = DL_ECAP_SYNC_OUT_SYNCI,
        .enableLoadCounter = false
        //todo loadCounter in js file unused
    },
    .interruptsConfig =
    {
        .interruptSourceEnableMask = DL_ECAP_ISR_SOURCE_CEVT3,
        .dmaEventSelect            = DL_ECAP_EVENT_1
    },
    .emulationMode  = DL_ECAP_EMULATION_STOP
};

SYSCONFIG_WEAK void SYSCFG_DL_CAPTURE_ECAP1_init(void){
    DL_ECAP_init(CAPTURE_ECAP1_INST, (DL_ECAP_Config *)&gCAPTURE_ECAP1Config);
}


static const DL_MCPWM_TimeBaseConfig gPWM_CH0_TimeBaseConfig =
{
    .clockDivider              = DL_MCPWM_CLOCK_DIVIDER_1,
    .counterMode               = DL_MCPWM_COUNTER_MODE_UP,
    .counterDirectionAfterSync = DL_MCPWM_COUNT_MODE_DOWN_AFTER_SYNC,
    .periodValue               = 10000,
    .initialCounterValue       = 0,
    .enablePhaseShift          = false,
    .phaseShiftValue           = 0,
    .syncInSource              = DL_MCPWM_SYNC_IN_PULSE_SRC_DISABLE,
    .syncPerSource             = DL_MCPWM_SYNC_PULSE_DISABLED,
    .syncOutPulseMode          = DL_MCPWM_SYNC_OUT_PULSE_ON_SOFTWARE,
    .emulationMode             = DL_MCPWM_EMULATION_FREE_RUN,
};

static const DL_MCPWM_CounterCompareConfig gPWM_CH0_CounterCompareConfig =
{
    .pwm1CompareAValue = 1000,
    .pwm1CompareBValue = 0,
    .pwm2CompareAValue = 0,
    .pwm2CompareBValue = 0,
    .pwm3CompareAValue = 0,
    .pwm3CompareBValue = 0,
    .compareCValue     = 0,
    .compareDValue     = 0,
};

static const DL_MCPWM_Actions gPWM_CH0_ActionQualifierActions_PWM1A =
{
    .actionAtZero          = DL_MCPWM_AQ_OUTPUT_HIGH,
    .actionAtPeriod        = DL_MCPWM_AQ_OUTPUT_NO_CHANGE,
    .actionAtCountUpCmpA   = DL_MCPWM_AQ_OUTPUT_LOW,
    .actionAtCountDownCmpA = DL_MCPWM_AQ_OUTPUT_NO_CHANGE,
    .actionAtCountUpCmpB   = DL_MCPWM_AQ_OUTPUT_NO_CHANGE,
    .actionAtCountDownCmpB = DL_MCPWM_AQ_OUTPUT_NO_CHANGE,
    .pwmSwForceAction      = DL_MCPWM_AQ_SW_FORCE_DISABLED,
};

static const DL_MCPWM_LoadModeConfig gPWM_CH0_LoadModeConfig =
{
    .pwm1CmpALoadMode   = DL_MCPWM_COMP_LOAD_ON_CNTR_ZERO,
    .pwm1CmpBLoadMode   = DL_MCPWM_COMP_LOAD_ON_CNTR_ZERO,
    .pwm2CmpALoadMode   = DL_MCPWM_COMP_LOAD_ON_CNTR_ZERO,
    .pwm2CmpBLoadMode   = DL_MCPWM_COMP_LOAD_ON_CNTR_ZERO,
    .pwm3CmpALoadMode   = DL_MCPWM_COMP_LOAD_ON_CNTR_ZERO,
    .pwm3CmpBLoadMode   = DL_MCPWM_COMP_LOAD_ON_CNTR_ZERO,
    .cmpCLoadMode       = DL_MCPWM_COMP_LOAD_ON_CNTR_ZERO,
    .cmpDLoadMode       = DL_MCPWM_COMP_LOAD_ON_CNTR_ZERO,
    .pwm1AaqLoadMode    = DL_MCPWM_AQ_LOAD_ON_CNTR_ZERO,
    .pwm1BaqLoadMode    = DL_MCPWM_AQ_LOAD_ON_CNTR_ZERO,
    .pwm2AaqLoadMode    = DL_MCPWM_AQ_LOAD_ON_CNTR_ZERO,
    .pwm2BaqLoadMode    = DL_MCPWM_AQ_LOAD_ON_CNTR_ZERO,
    .pwm3AaqLoadMode    = DL_MCPWM_AQ_LOAD_ON_CNTR_ZERO,
    .pwm3BaqLoadMode    = DL_MCPWM_AQ_LOAD_ON_CNTR_ZERO,
    .globalLoadConfig   =
    {
        .enableGlobalLoad   = false,
        .triggerEvent       = DL_MCPWM_GL_LOAD_PULSE_CNTR_ZERO,
        .enableOneShotMode  = false,
        .enableOneShotLatch = false,
        .forceLoadEvent     = false,
    }
};

SYSCONFIG_WEAK void SYSCFG_DL_PWM_CH0_init(void)
{
    DL_MCPWM_configureTimeBase(PWM_CH0_INST, (DL_MCPWM_TimeBaseConfig *)&gPWM_CH0_TimeBaseConfig);
    DL_MCPWM_configureCounterCompare(PWM_CH0_INST, (DL_MCPWM_CounterCompareConfig *)&gPWM_CH0_CounterCompareConfig);
    DL_MCPWM_configureActionQualifierActions(PWM_CH0_INST, DL_MCPWM_AQ_OUTPUT_1A, (DL_MCPWM_Actions *)&gPWM_CH0_ActionQualifierActions_PWM1A);
    DL_MCPWM_configureLoadMode(PWM_CH0_INST, (DL_MCPWM_LoadModeConfig *)&gPWM_CH0_LoadModeConfig);

    // IOMUX Setting
    DL_GPIO_initPeripheralOutputFunction(IOMUX_PINCM_PB3, IOMUX_PB3_MCPWM0_1A);
}
static const DL_MCPWM_TimeBaseConfig gPWM_CH1_TimeBaseConfig =
{
    .clockDivider              = DL_MCPWM_CLOCK_DIVIDER_1,
    .counterMode               = DL_MCPWM_COUNTER_MODE_UP,
    .counterDirectionAfterSync = DL_MCPWM_COUNT_MODE_DOWN_AFTER_SYNC,
    .periodValue               = 0,
    .initialCounterValue       = 0,
    .enablePhaseShift          = false,
    .phaseShiftValue           = 0,
    .syncInSource              = DL_MCPWM_SYNC_IN_PULSE_SRC_DISABLE,
    .syncPerSource             = DL_MCPWM_SYNC_PULSE_DISABLED,
    .syncOutPulseMode          = DL_MCPWM_SYNC_OUT_PULSE_ON_SOFTWARE,
    .emulationMode             = DL_MCPWM_EMULATION_FREE_RUN,
};

static const DL_MCPWM_CounterCompareConfig gPWM_CH1_CounterCompareConfig =
{
    .pwm1CompareAValue = 0,
    .pwm1CompareBValue = 0,
    .pwm2CompareAValue = 0,
    .pwm2CompareBValue = 0,
    .pwm3CompareAValue = 0,
    .pwm3CompareBValue = 0,
    .compareCValue     = 0,
    .compareDValue     = 0,
};

static const DL_MCPWM_Actions gPWM_CH1_ActionQualifierActions_PWM1B =
{
    .actionAtZero          = DL_MCPWM_AQ_OUTPUT_HIGH,
    .actionAtPeriod        = DL_MCPWM_AQ_OUTPUT_NO_CHANGE,
    .actionAtCountUpCmpA   = DL_MCPWM_AQ_OUTPUT_NO_CHANGE,
    .actionAtCountDownCmpA = DL_MCPWM_AQ_OUTPUT_NO_CHANGE,
    .actionAtCountUpCmpB   = DL_MCPWM_AQ_OUTPUT_LOW,
    .actionAtCountDownCmpB = DL_MCPWM_AQ_OUTPUT_NO_CHANGE,
    .pwmSwForceAction      = DL_MCPWM_AQ_SW_FORCE_DISABLED,
};

static const DL_MCPWM_LoadModeConfig gPWM_CH1_LoadModeConfig =
{
    .pwm1CmpALoadMode   = DL_MCPWM_COMP_LOAD_ON_CNTR_ZERO,
    .pwm1CmpBLoadMode   = DL_MCPWM_COMP_LOAD_ON_CNTR_ZERO,
    .pwm2CmpALoadMode   = DL_MCPWM_COMP_LOAD_ON_CNTR_ZERO,
    .pwm2CmpBLoadMode   = DL_MCPWM_COMP_LOAD_ON_CNTR_ZERO,
    .pwm3CmpALoadMode   = DL_MCPWM_COMP_LOAD_ON_CNTR_ZERO,
    .pwm3CmpBLoadMode   = DL_MCPWM_COMP_LOAD_ON_CNTR_ZERO,
    .cmpCLoadMode       = DL_MCPWM_COMP_LOAD_ON_CNTR_ZERO,
    .cmpDLoadMode       = DL_MCPWM_COMP_LOAD_ON_CNTR_ZERO,
    .pwm1AaqLoadMode    = DL_MCPWM_AQ_LOAD_ON_CNTR_ZERO,
    .pwm1BaqLoadMode    = DL_MCPWM_AQ_LOAD_ON_CNTR_ZERO,
    .pwm2AaqLoadMode    = DL_MCPWM_AQ_LOAD_ON_CNTR_ZERO,
    .pwm2BaqLoadMode    = DL_MCPWM_AQ_LOAD_ON_CNTR_ZERO,
    .pwm3AaqLoadMode    = DL_MCPWM_AQ_LOAD_ON_CNTR_ZERO,
    .pwm3BaqLoadMode    = DL_MCPWM_AQ_LOAD_ON_CNTR_ZERO,
    .globalLoadConfig   =
    {
        .enableGlobalLoad   = false,
        .triggerEvent       = DL_MCPWM_GL_LOAD_PULSE_CNTR_ZERO,
        .enableOneShotMode  = false,
        .enableOneShotLatch = false,
        .forceLoadEvent     = false,
    }
};

SYSCONFIG_WEAK void SYSCFG_DL_PWM_CH1_init(void)
{
    DL_MCPWM_configureTimeBase(PWM_CH1_INST, (DL_MCPWM_TimeBaseConfig *)&gPWM_CH1_TimeBaseConfig);
    DL_MCPWM_configureCounterCompare(PWM_CH1_INST, (DL_MCPWM_CounterCompareConfig *)&gPWM_CH1_CounterCompareConfig);
    DL_MCPWM_configureActionQualifierActions(PWM_CH1_INST, DL_MCPWM_AQ_OUTPUT_1B, (DL_MCPWM_Actions *)&gPWM_CH1_ActionQualifierActions_PWM1B);
    DL_MCPWM_configureLoadMode(PWM_CH1_INST, (DL_MCPWM_LoadModeConfig *)&gPWM_CH1_LoadModeConfig);

    // IOMUX Setting
    DL_GPIO_initPeripheralOutputFunction(IOMUX_PINCM_PA7, IOMUX_PA7_MCPWM4_1B);
}
static const DL_MCPWM_TimeBaseConfig gPWM_CH2_TimeBaseConfig =
{
    .clockDivider              = DL_MCPWM_CLOCK_DIVIDER_1,
    .counterMode               = DL_MCPWM_COUNTER_MODE_UP,
    .counterDirectionAfterSync = DL_MCPWM_COUNT_MODE_DOWN_AFTER_SYNC,
    .periodValue               = 0,
    .initialCounterValue       = 0,
    .enablePhaseShift          = false,
    .phaseShiftValue           = 0,
    .syncInSource              = DL_MCPWM_SYNC_IN_PULSE_SRC_DISABLE,
    .syncPerSource             = DL_MCPWM_SYNC_PULSE_DISABLED,
    .syncOutPulseMode          = DL_MCPWM_SYNC_OUT_PULSE_ON_SOFTWARE,
    .emulationMode             = DL_MCPWM_EMULATION_FREE_RUN,
};

static const DL_MCPWM_CounterCompareConfig gPWM_CH2_CounterCompareConfig =
{
    .pwm1CompareAValue = 0,
    .pwm1CompareBValue = 0,
    .pwm2CompareAValue = 0,
    .pwm2CompareBValue = 0,
    .pwm3CompareAValue = 0,
    .pwm3CompareBValue = 0,
    .compareCValue     = 0,
    .compareDValue     = 0,
};

static const DL_MCPWM_Actions gPWM_CH2_ActionQualifierActions_PWM3A =
{
    .actionAtZero          = DL_MCPWM_AQ_OUTPUT_HIGH,
    .actionAtPeriod        = DL_MCPWM_AQ_OUTPUT_NO_CHANGE,
    .actionAtCountUpCmpA   = DL_MCPWM_AQ_OUTPUT_LOW,
    .actionAtCountDownCmpA = DL_MCPWM_AQ_OUTPUT_NO_CHANGE,
    .actionAtCountUpCmpB   = DL_MCPWM_AQ_OUTPUT_NO_CHANGE,
    .actionAtCountDownCmpB = DL_MCPWM_AQ_OUTPUT_NO_CHANGE,
    .pwmSwForceAction      = DL_MCPWM_AQ_SW_FORCE_DISABLED,
};

static const DL_MCPWM_LoadModeConfig gPWM_CH2_LoadModeConfig =
{
    .pwm1CmpALoadMode   = DL_MCPWM_COMP_LOAD_ON_CNTR_ZERO,
    .pwm1CmpBLoadMode   = DL_MCPWM_COMP_LOAD_ON_CNTR_ZERO,
    .pwm2CmpALoadMode   = DL_MCPWM_COMP_LOAD_ON_CNTR_ZERO,
    .pwm2CmpBLoadMode   = DL_MCPWM_COMP_LOAD_ON_CNTR_ZERO,
    .pwm3CmpALoadMode   = DL_MCPWM_COMP_LOAD_ON_CNTR_ZERO,
    .pwm3CmpBLoadMode   = DL_MCPWM_COMP_LOAD_ON_CNTR_ZERO,
    .cmpCLoadMode       = DL_MCPWM_COMP_LOAD_ON_CNTR_ZERO,
    .cmpDLoadMode       = DL_MCPWM_COMP_LOAD_ON_CNTR_ZERO,
    .pwm1AaqLoadMode    = DL_MCPWM_AQ_LOAD_ON_CNTR_ZERO,
    .pwm1BaqLoadMode    = DL_MCPWM_AQ_LOAD_ON_CNTR_ZERO,
    .pwm2AaqLoadMode    = DL_MCPWM_AQ_LOAD_ON_CNTR_ZERO,
    .pwm2BaqLoadMode    = DL_MCPWM_AQ_LOAD_ON_CNTR_ZERO,
    .pwm3AaqLoadMode    = DL_MCPWM_AQ_LOAD_ON_CNTR_ZERO,
    .pwm3BaqLoadMode    = DL_MCPWM_AQ_LOAD_ON_CNTR_ZERO,
    .globalLoadConfig   =
    {
        .enableGlobalLoad   = false,
        .triggerEvent       = DL_MCPWM_GL_LOAD_PULSE_CNTR_ZERO,
        .enableOneShotMode  = false,
        .enableOneShotLatch = false,
        .forceLoadEvent     = false,
    }
};

SYSCONFIG_WEAK void SYSCFG_DL_PWM_CH2_init(void)
{
    DL_MCPWM_configureTimeBase(PWM_CH2_INST, (DL_MCPWM_TimeBaseConfig *)&gPWM_CH2_TimeBaseConfig);
    DL_MCPWM_configureCounterCompare(PWM_CH2_INST, (DL_MCPWM_CounterCompareConfig *)&gPWM_CH2_CounterCompareConfig);
    DL_MCPWM_configureActionQualifierActions(PWM_CH2_INST, DL_MCPWM_AQ_OUTPUT_3A, (DL_MCPWM_Actions *)&gPWM_CH2_ActionQualifierActions_PWM3A);
    DL_MCPWM_configureLoadMode(PWM_CH2_INST, (DL_MCPWM_LoadModeConfig *)&gPWM_CH2_LoadModeConfig);

    // IOMUX Setting
    DL_GPIO_initPeripheralOutputFunction(IOMUX_PINCM_PB1, IOMUX_PB1_MCPWM3_3A);
}
static const DL_MCPWM_TimeBaseConfig gPWM_CH3_TimeBaseConfig =
{
    .clockDivider              = DL_MCPWM_CLOCK_DIVIDER_1,
    .counterMode               = DL_MCPWM_COUNTER_MODE_UP,
    .counterDirectionAfterSync = DL_MCPWM_COUNT_MODE_DOWN_AFTER_SYNC,
    .periodValue               = 0,
    .initialCounterValue       = 0,
    .enablePhaseShift          = false,
    .phaseShiftValue           = 0,
    .syncInSource              = DL_MCPWM_SYNC_IN_PULSE_SRC_DISABLE,
    .syncPerSource             = DL_MCPWM_SYNC_PULSE_DISABLED,
    .syncOutPulseMode          = DL_MCPWM_SYNC_OUT_PULSE_ON_SOFTWARE,
    .emulationMode             = DL_MCPWM_EMULATION_FREE_RUN,
};

static const DL_MCPWM_CounterCompareConfig gPWM_CH3_CounterCompareConfig =
{
    .pwm1CompareAValue = 0,
    .pwm1CompareBValue = 0,
    .pwm2CompareAValue = 0,
    .pwm2CompareBValue = 0,
    .pwm3CompareAValue = 0,
    .pwm3CompareBValue = 0,
    .compareCValue     = 0,
    .compareDValue     = 0,
};

static const DL_MCPWM_Actions gPWM_CH3_ActionQualifierActions_PWM2A =
{
    .actionAtZero          = DL_MCPWM_AQ_OUTPUT_HIGH,
    .actionAtPeriod        = DL_MCPWM_AQ_OUTPUT_NO_CHANGE,
    .actionAtCountUpCmpA   = DL_MCPWM_AQ_OUTPUT_LOW,
    .actionAtCountDownCmpA = DL_MCPWM_AQ_OUTPUT_NO_CHANGE,
    .actionAtCountUpCmpB   = DL_MCPWM_AQ_OUTPUT_NO_CHANGE,
    .actionAtCountDownCmpB = DL_MCPWM_AQ_OUTPUT_NO_CHANGE,
    .pwmSwForceAction      = DL_MCPWM_AQ_SW_FORCE_DISABLED,
};

static const DL_MCPWM_LoadModeConfig gPWM_CH3_LoadModeConfig =
{
    .pwm1CmpALoadMode   = DL_MCPWM_COMP_LOAD_ON_CNTR_ZERO,
    .pwm1CmpBLoadMode   = DL_MCPWM_COMP_LOAD_ON_CNTR_ZERO,
    .pwm2CmpALoadMode   = DL_MCPWM_COMP_LOAD_ON_CNTR_ZERO,
    .pwm2CmpBLoadMode   = DL_MCPWM_COMP_LOAD_ON_CNTR_ZERO,
    .pwm3CmpALoadMode   = DL_MCPWM_COMP_LOAD_ON_CNTR_ZERO,
    .pwm3CmpBLoadMode   = DL_MCPWM_COMP_LOAD_ON_CNTR_ZERO,
    .cmpCLoadMode       = DL_MCPWM_COMP_LOAD_ON_CNTR_ZERO,
    .cmpDLoadMode       = DL_MCPWM_COMP_LOAD_ON_CNTR_ZERO,
    .pwm1AaqLoadMode    = DL_MCPWM_AQ_LOAD_ON_CNTR_ZERO,
    .pwm1BaqLoadMode    = DL_MCPWM_AQ_LOAD_ON_CNTR_ZERO,
    .pwm2AaqLoadMode    = DL_MCPWM_AQ_LOAD_ON_CNTR_ZERO,
    .pwm2BaqLoadMode    = DL_MCPWM_AQ_LOAD_ON_CNTR_ZERO,
    .pwm3AaqLoadMode    = DL_MCPWM_AQ_LOAD_ON_CNTR_ZERO,
    .pwm3BaqLoadMode    = DL_MCPWM_AQ_LOAD_ON_CNTR_ZERO,
    .globalLoadConfig   =
    {
        .enableGlobalLoad   = false,
        .triggerEvent       = DL_MCPWM_GL_LOAD_PULSE_CNTR_ZERO,
        .enableOneShotMode  = false,
        .enableOneShotLatch = false,
        .forceLoadEvent     = false,
    }
};

SYSCONFIG_WEAK void SYSCFG_DL_PWM_CH3_init(void)
{
    DL_MCPWM_configureTimeBase(PWM_CH3_INST, (DL_MCPWM_TimeBaseConfig *)&gPWM_CH3_TimeBaseConfig);
    DL_MCPWM_configureCounterCompare(PWM_CH3_INST, (DL_MCPWM_CounterCompareConfig *)&gPWM_CH3_CounterCompareConfig);
    DL_MCPWM_configureActionQualifierActions(PWM_CH3_INST, DL_MCPWM_AQ_OUTPUT_2A, (DL_MCPWM_Actions *)&gPWM_CH3_ActionQualifierActions_PWM2A);
    DL_MCPWM_configureLoadMode(PWM_CH3_INST, (DL_MCPWM_LoadModeConfig *)&gPWM_CH3_LoadModeConfig);

    // IOMUX Setting
    DL_GPIO_initPeripheralOutputFunction(IOMUX_PINCM_PB2, IOMUX_PB2_MCPWM2_2A);
}
static const DL_MCPWM_TimeBaseConfig gHSADC_TRIGGER_PWM_TimeBaseConfig =
{
    .clockDivider              = DL_MCPWM_CLOCK_DIVIDER_1,
    .counterMode               = DL_MCPWM_COUNTER_MODE_UP,
    .counterDirectionAfterSync = DL_MCPWM_COUNT_MODE_DOWN_AFTER_SYNC,
    .periodValue               = 9999,
    .initialCounterValue       = 0,
    .enablePhaseShift          = false,
    .phaseShiftValue           = 0,
    .syncInSource              = DL_MCPWM_SYNC_IN_PULSE_SRC_DISABLE,
    .syncPerSource             = DL_MCPWM_SYNC_PULSE_DISABLED,
    .syncOutPulseMode          = DL_MCPWM_SYNC_OUT_PULSE_ON_SOFTWARE,
    .emulationMode             = DL_MCPWM_EMULATION_FREE_RUN,
};

static const DL_MCPWM_CounterCompareConfig gHSADC_TRIGGER_PWM_CounterCompareConfig =
{
    .pwm1CompareAValue = 0,
    .pwm1CompareBValue = 0,
    .pwm2CompareAValue = 0,
    .pwm2CompareBValue = 0,
    .pwm3CompareAValue = 0,
    .pwm3CompareBValue = 0,
    .compareCValue     = 0,
    .compareDValue     = 0,
};

static const DL_MCPWM_Actions gHSADC_TRIGGER_PWM_ActionQualifierActions_PWM1A =
{
    .actionAtZero          = DL_MCPWM_AQ_OUTPUT_HIGH,
    .actionAtPeriod        = DL_MCPWM_AQ_OUTPUT_NO_CHANGE,
    .actionAtCountUpCmpA   = DL_MCPWM_AQ_OUTPUT_LOW,
    .actionAtCountDownCmpA = DL_MCPWM_AQ_OUTPUT_NO_CHANGE,
    .actionAtCountUpCmpB   = DL_MCPWM_AQ_OUTPUT_NO_CHANGE,
    .actionAtCountDownCmpB = DL_MCPWM_AQ_OUTPUT_NO_CHANGE,
    .pwmSwForceAction      = DL_MCPWM_AQ_SW_FORCE_DISABLED,
};

static const DL_MCPWM_EventTriggerConfig gHSADC_TRIGGER_PWM_EventTriggerConfig =
{
    .socPulseGenConfig =
    {
        .SOCA =
        {
            .enable      = true,
            .eventSource = DL_MCPWM_SOC_TBCTR_ZERO,
            .eventPeriod = 1,
        },
        .SOCB =
        {
            .enable      = false,
            .eventSource = DL_MCPWM_SOC_DISABLED,
            .eventPeriod = 1,
        },
        .SOCC =
        {
            .enable      = false,
            .eventSource = DL_MCPWM_SOC_DISABLED,
            .eventPeriod = 1,
        },
        .SOCD =
        {
            .enable      = false,
            .eventSource = DL_MCPWM_SOC_DISABLED,
            .eventPeriod = 1,
        }
    },
    .etConfig =
    {
        .ET1 =
        {
            .eventSource = DL_MCPWM_EVT_DISABLED,
            .eventPeriod = 1,
        },
        .ET2 =
        {
            .eventSource = DL_MCPWM_EVT_DISABLED,
            .eventPeriod = 1,
        }
    },
    .interruptGenConfig = 0
};

static const DL_MCPWM_LoadModeConfig gHSADC_TRIGGER_PWM_LoadModeConfig =
{
    .pwm1CmpALoadMode   = DL_MCPWM_COMP_LOAD_ON_CNTR_ZERO,
    .pwm1CmpBLoadMode   = DL_MCPWM_COMP_LOAD_ON_CNTR_ZERO,
    .pwm2CmpALoadMode   = DL_MCPWM_COMP_LOAD_ON_CNTR_ZERO,
    .pwm2CmpBLoadMode   = DL_MCPWM_COMP_LOAD_ON_CNTR_ZERO,
    .pwm3CmpALoadMode   = DL_MCPWM_COMP_LOAD_ON_CNTR_ZERO,
    .pwm3CmpBLoadMode   = DL_MCPWM_COMP_LOAD_ON_CNTR_ZERO,
    .cmpCLoadMode       = DL_MCPWM_COMP_LOAD_ON_CNTR_ZERO,
    .cmpDLoadMode       = DL_MCPWM_COMP_LOAD_ON_CNTR_ZERO,
    .pwm1AaqLoadMode    = DL_MCPWM_AQ_LOAD_ON_CNTR_ZERO,
    .pwm1BaqLoadMode    = DL_MCPWM_AQ_LOAD_ON_CNTR_ZERO,
    .pwm2AaqLoadMode    = DL_MCPWM_AQ_LOAD_ON_CNTR_ZERO,
    .pwm2BaqLoadMode    = DL_MCPWM_AQ_LOAD_ON_CNTR_ZERO,
    .pwm3AaqLoadMode    = DL_MCPWM_AQ_LOAD_ON_CNTR_ZERO,
    .pwm3BaqLoadMode    = DL_MCPWM_AQ_LOAD_ON_CNTR_ZERO,
    .globalLoadConfig   =
    {
        .enableGlobalLoad   = false,
        .triggerEvent       = DL_MCPWM_GL_LOAD_PULSE_CNTR_ZERO,
        .enableOneShotMode  = false,
        .enableOneShotLatch = false,
        .forceLoadEvent     = false,
    }
};

SYSCONFIG_WEAK void SYSCFG_DL_HSADC_TRIGGER_PWM_init(void)
{
    DL_MCPWM_configureTimeBase(HSADC_TRIGGER_PWM_INST, (DL_MCPWM_TimeBaseConfig *)&gHSADC_TRIGGER_PWM_TimeBaseConfig);
    DL_MCPWM_configureCounterCompare(HSADC_TRIGGER_PWM_INST, (DL_MCPWM_CounterCompareConfig *)&gHSADC_TRIGGER_PWM_CounterCompareConfig);
    DL_MCPWM_configureActionQualifierActions(HSADC_TRIGGER_PWM_INST, DL_MCPWM_AQ_OUTPUT_1A, (DL_MCPWM_Actions *)&gHSADC_TRIGGER_PWM_ActionQualifierActions_PWM1A);
    DL_MCPWM_configureEventTrigger(HSADC_TRIGGER_PWM_INST, (DL_MCPWM_EventTriggerConfig *)&gHSADC_TRIGGER_PWM_EventTriggerConfig);
    DL_MCPWM_configureLoadMode(HSADC_TRIGGER_PWM_INST, (DL_MCPWM_LoadModeConfig *)&gHSADC_TRIGGER_PWM_LoadModeConfig);

    // IOMUX Setting
    DL_GPIO_initPeripheralOutputFunction(IOMUX_PINCM_PA15, IOMUX_PA15_MCPWM1_1A);
}

static const DL_UART_ClockConfig gRS485_UART_ClockConfig = {
    .clockSel    = DL_UART_CLOCK_BUSCLK,
    .divideRatio = DL_UART_CLOCK_DIVIDE_RATIO_1
};

static const DL_UART_Config gRS485_UART_Config = {
    .mode        = DL_UART_MODE_NORMAL,
    .direction   = DL_UART_DIRECTION_TX_RX,
    .flowControl = DL_UART_FLOW_CONTROL_NONE,
    .parity      = DL_UART_PARITY_NONE,
    .wordLength  = DL_UART_WORD_LENGTH_8_BITS,
    .stopBits    = DL_UART_STOP_BITS_ONE
};

SYSCONFIG_WEAK void SYSCFG_DL_RS485_UART_init(void)
{
    DL_UART_setClockConfig(RS485_UART_INST, (DL_UART_ClockConfig *) &gRS485_UART_ClockConfig);

    DL_UART_init(RS485_UART_INST, (DL_UART_Config *) &gRS485_UART_Config);
    /*
     * Configure baud rate by setting oversampling and baud rate divisors.
     *  Target baud rate: 115200
     *  Actual baud rate: 115207.37
     */
    DL_UART_setOversampling(RS485_UART_INST, DL_UART_OVERSAMPLING_RATE_16X);
    DL_UART_setBaudRateDivisor(RS485_UART_INST, RS485_UART_IBRD_100_MHZ_115200_BAUD, RS485_UART_FBRD_100_MHZ_115200_BAUD);

    /* Configure Interrupts */
    DL_UART_enableInterrupt(RS485_UART_INST,
                                 DL_UART_INTERRUPT_FRAMING_ERROR |
                                 DL_UART_INTERRUPT_OVERRUN_ERROR |
                                 DL_UART_INTERRUPT_PARITY_ERROR |
                                 DL_UART_INTERRUPT_RX);

    /* Configure FIFOs */
    DL_UART_setRXFIFOThreshold(RS485_UART_INST, DL_UART_RX_FIFO_LEVEL_NOT_EMPTY);
    DL_UART_setTXFIFOThreshold(RS485_UART_INST, DL_UART_TX_FIFO_LEVEL_NOT_FULL);

    DL_UART_enable(RS485_UART_INST);
}
static const DL_UART_ClockConfig gUSER_UART_ClockConfig = {
    .clockSel    = DL_UART_CLOCK_BUSCLK,
    .divideRatio = DL_UART_CLOCK_DIVIDE_RATIO_1
};

static const DL_UART_Config gUSER_UART_Config = {
    .mode        = DL_UART_MODE_NORMAL,
    .direction   = DL_UART_DIRECTION_TX_RX,
    .flowControl = DL_UART_FLOW_CONTROL_NONE,
    .parity      = DL_UART_PARITY_NONE,
    .wordLength  = DL_UART_WORD_LENGTH_8_BITS,
    .stopBits    = DL_UART_STOP_BITS_ONE
};

SYSCONFIG_WEAK void SYSCFG_DL_USER_UART_init(void)
{
    DL_UART_setClockConfig(USER_UART_INST, (DL_UART_ClockConfig *) &gUSER_UART_ClockConfig);

    DL_UART_init(USER_UART_INST, (DL_UART_Config *) &gUSER_UART_Config);
    /*
     * Configure baud rate by setting oversampling and baud rate divisors.
     *  Target baud rate: 115200
     *  Actual baud rate: 115207.37
     */
    DL_UART_setOversampling(USER_UART_INST, DL_UART_OVERSAMPLING_RATE_16X);
    DL_UART_setBaudRateDivisor(USER_UART_INST, USER_UART_IBRD_100_MHZ_115200_BAUD, USER_UART_FBRD_100_MHZ_115200_BAUD);

    /* Configure Interrupts */
    DL_UART_enableInterrupt(USER_UART_INST,
                                 DL_UART_INTERRUPT_FRAMING_ERROR |
                                 DL_UART_INTERRUPT_OVERRUN_ERROR |
                                 DL_UART_INTERRUPT_PARITY_ERROR |
                                 DL_UART_INTERRUPT_RX);

    /* Configure FIFOs */
    DL_UART_setRXFIFOThreshold(USER_UART_INST, DL_UART_RX_FIFO_LEVEL_NOT_EMPTY);
    DL_UART_setTXFIFOThreshold(USER_UART_INST, DL_UART_TX_FIFO_LEVEL_NOT_FULL);

    DL_UART_enable(USER_UART_INST);
}

SYSCONFIG_WEAK void SYSCFG_DL_SYSTICK_init(void)
{
    /* Initialize the period to 83.89 ms */
    DL_SYSTICK_init(16777216);
    DL_SYSTICK_enableInterrupt();
}

SYSCONFIG_WEAK void SYSCFG_DL_INTERRUPT_init(void)
{

    NVIC_EnableIRQ(DMA0_INT);
    NVIC_EnableIRQ(CAPTURE_ECAP0_INT);
    NVIC_EnableIRQ(USER_UART_INT);
}


SYSCONFIG_WEAK void SYSCFG_DL_FLASH_init(void)
{
    // Disable cache before changing wait states
    DL_FRI_disableDLB();
    DL_FRI_disableCache();

    // Set the flash wait states
    DL_FRI_setReadWaitStates(0x3);

    // Enable cache to improve performance of code executed from flash.
    DL_FRI_enableDLB();
    DL_FRI_enableCache();
}