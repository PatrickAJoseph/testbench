
#include "device.h"
#include "ti_sdk_dl_config.h"


#define TBPRD_MIN_LOW_FREQUENCY       (1000U)
#define TBPRD_MIN_HIGH_FREQUENCY      (200U)
#define IS_LOW_FREQUENCY(f)           (f < 100000U)     
#define TBPRD_MIN(f)                ( IS_LOW_FREQUENCY(f) ? TBPRD_MIN_LOW_FREQUENCY : TBPRD_MIN_HIGH_FREQUENCY )
#define TBPRD_MAX(f)                ( 65535 )
#define TBPRD_WITHIN_RANGE(x,f)     ( ( (x) >= TBPRD_MIN(f) ) && ( (x) <= TBPRD_MAX(f) )  )

struct pwm_channel
{
    MCPWM_Regs* base;
    DL_MCPWM_COUNTER_COMPARE_MODULE compare_module;
    uint32_t frequency;
    float duty;
};

void pwm_configure(struct pwm_channel* channel, uint32_t frequency, float duty)
{
    uint32_t tbprd;
    uint32_t clkdiv = 0;
    uint32_t clkdiv_value;
    uint32_t compare;
    int index = 0;

    for( index = 0 ; index <= 15 ; index++ )
    {
        clkdiv_value = ( 1U << index );
        tbprd = ((double)MCLK_FREQ_HZ/((double)clkdiv_value*(double)frequency)) - 1;

        if( TBPRD_WITHIN_RANGE(tbprd, frequency) )
        {
            clkdiv = index;
            break;
        }
    }

    compare = (uint32_t)( (double)duty * (double)(1.0+tbprd) );

    DL_MCPWM_setClockPrescaler(channel->base, (DL_MCPWM_CLOCK_DIVIDER)clkdiv);
    DL_MCPWM_setTimeBasePeriodShadow(channel->base, tbprd);
    DL_MCPWM_setCounterCompareShadowValue(channel->base, channel->compare_module, compare);

    channel->frequency = frequency;
    channel->duty = duty;
}

struct pwm_channel pwm_channels[4] = {
    {
        .base = MCPWM0,
        .compare_module = DL_MCPWM_COUNTER_COMPARE_1A, 
    },
    {
        .base = MCPWM4,
        .compare_module = DL_MCPWM_COUNTER_COMPARE_1B,
    },
    {
        .base = MCPWM3,
        .compare_module = DL_MCPWM_COUNTER_COMPARE_3A,
    },
    {
        .base               =   MCPWM2,
        .compare_module     =   DL_MCPWM_COUNTER_COMPARE_2A,
    },
};

void PWM_init()
{
    pwm_configure( &pwm_channels[0], 1000, 0.0f );
    pwm_configure( &pwm_channels[1], 1000, 0.0f );
    pwm_configure( &pwm_channels[2], 1000, 0.0f );
    pwm_configure( &pwm_channels[3], 1000, 0.0f ); 
}

extern void PWM_enable(int channel)
{
    pwm_configure( &pwm_channels[channel], pwm_channels[channel].frequency, pwm_channels[channel].duty );
}

void PWM_disable(int channel)
{
    pwm_configure( &pwm_channels[channel], pwm_channels[channel].frequency , 0.0f );
}

void PWM_set_frequency(int channel, int frequency)
{
    pwm_configure( &pwm_channels[channel], frequency, pwm_channels[channel].duty );
}

void PWM_set_duty(int channel, float duty)
{
    pwm_configure( &pwm_channels[channel], pwm_channels[channel].frequency, duty );
}