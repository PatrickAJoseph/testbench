#include <string.h>
#include "device.h"
#include "ti_sdk_dl_config.h"
#include "hsadc.h"

#define ADC_SAMPLE_BUFFER_SIZE      (8192U)             //  32kB buffer (in total).

struct adc_context {
    MCPWM_Regs* soc_pwm_base;
    uint32_t samples[ADC_SAMPLE_BUFFER_SIZE];
    volatile bool done;
};

 struct adc_context hsadc_context = {
    .samples = {0},
    .done = false,
    .soc_pwm_base = HSADC_TRIGGER_PWM_INST,
};


struct adc_sampling_rate_map_entry{
    adc_sampling_rate sampling_rate;
    uint32_t value;
};

 
#define ADC_SAMPLING_RATE_ENTRY(__sampling_rate, __value)   \
    {                                                   \
        .sampling_rate = __sampling_rate,               \
        .value = __value,                               \
    }

static const struct adc_sampling_rate_map_entry adc_sampling_rate_map[] =
{
    ADC_SAMPLING_RATE_ENTRY(ADC_SAMPLING_RATE_1000KSPS, 1000000),
    ADC_SAMPLING_RATE_ENTRY(ADC_SAMPLING_RATE_625KSPS, 625000),
    ADC_SAMPLING_RATE_ENTRY(ADC_SAMPLING_RATE_500KSPS, 500000),
    ADC_SAMPLING_RATE_ENTRY(ADC_SAMPLING_RATE_400KSPS, 400000),
    ADC_SAMPLING_RATE_ENTRY(ADC_SAMPLING_RATE_250KSPS, 250000),
    ADC_SAMPLING_RATE_ENTRY(ADC_SAMPLING_RATE_200KSPS, 200000),
    ADC_SAMPLING_RATE_ENTRY(ADC_SAMPLING_RATE_125KSPS, 125000),
    ADC_SAMPLING_RATE_ENTRY(ADC_SAMPLING_RATE_100KSPS, 100000),
    ADC_SAMPLING_RATE_ENTRY(ADC_SAMPLING_RATE_75KSPS, 75000),
    ADC_SAMPLING_RATE_ENTRY(ADC_SAMPLING_RATE_62K5SPS, 62500),
    ADC_SAMPLING_RATE_ENTRY(ADC_SAMPLING_RATE_50KSPS, 50000),
    ADC_SAMPLING_RATE_ENTRY(ADC_SAMPLING_RATE_25KSPS, 25000),
    ADC_SAMPLING_RATE_ENTRY(ADC_SAMPLING_RATE_12K5SPS, 12500),
    ADC_SAMPLING_RATE_ENTRY(ADC_SAMPLING_RATE_10KSPS, 10000),
    ADC_SAMPLING_RATE_ENTRY(ADC_SAMPLING_RATE_7K5SPS, 7500),
    ADC_SAMPLING_RATE_ENTRY(ADC_SAMPLING_RATE_6K25SPS, 6250),
    ADC_SAMPLING_RATE_ENTRY(ADC_SAMPLING_RATE_5KSPS, 5000),
};

 

static void adc_set_sampling_rate(struct adc_context* context, adc_sampling_rate sampling_rate)
{
    uint32_t frequency;
    uint32_t clkdiv;
    uint32_t index;
    uint32_t tbprd;

    (void)frequency;
    (void)clkdiv;
    (void)index;
    (void)tbprd;

    frequency = adc_sampling_rate_map[(int)sampling_rate].value;

    for( index = 0 ; index <= 15 ; index++ )
    {
        clkdiv = index;
        tbprd = ((uint32_t)(((double)MCLK_FREQ_HZ)/((double)( 1U << clkdiv ) * (double)frequency)) - 1);

        if(tbprd < 65536)
        {
            break;
        }
    }

    DL_MCPWM_setClockPrescaler(context->soc_pwm_base, (DL_MCPWM_CLOCK_DIVIDER)clkdiv);
    DL_MCPWM_setTimeBasePeriodShadow(context->soc_pwm_base, tbprd);
}

 

static void adc_acquire(struct adc_context* context)
{
    context->done = false;
    DL_MCPWM_enableADCTrigger(context->soc_pwm_base, DL_MCPWM_SOC_A);
    memset(context->samples, 0, sizeof(context->samples));
    DL_DMA_enableChannel(HSADC_DMA_DMA_INST, HSADC_DMA_CHANNEL_ID);

    while(!context->done);

    DL_MCPWM_disableADCTrigger(context->soc_pwm_base, DL_MCPWM_SOC_A);
}

 

void HSADC_init()
{
    DL_ADC_powerUp(HSADC_INST);
    DEVICE_DELAY_US(5000);

    DL_DMA_setSrcAddr( HSADC_DMA_DMA_INST, HSADC_DMA_CHANNEL_ID, (uint32_t)&HSADC_RESULT_INST->ADCSEQ1FIFORESULT );
    DL_DMA_setDestAddr( HSADC_DMA_DMA_INST, HSADC_DMA_CHANNEL_ID, (uint32_t)&hsadc_context.samples[0] );
}


void HSADC_set_sampling_rate(int rate)
{
    adc_set_sampling_rate(&hsadc_context, (adc_sampling_rate)rate);
}

uint16_t HSADC_get_sample(int channel, int index)
{
    return (uint16_t)(((uint16_t*)hsadc_context.samples)[ 2*index + channel ]);
}

void HSADC_acquire()
{
    adc_acquire(&hsadc_context);
}

volatile int hsadc_interrupt_count = 0;

void DMA0_INT_Handler()
{
    DL_DMA_clearInterruptStatus(HSADC_DMA_DMA_INST, HSADC_DMA_CHANNEL_INTERRUPT_MASK);
    hsadc_context.done = true;
    hsadc_interrupt_count++;
}