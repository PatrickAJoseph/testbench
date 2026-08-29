
#include "user_timer.h"

#include "ti_sdk_dl_config.h"

static timer_callback_t timer_callback;
static void* timer_callback_args;

void USER_TIMER_INT_Handler()
{
    if(timer_callback)
    {
        timer_callback(timer_callback_args);
    }
}

void Timer_init()
{
    DL_TimerG_disableInterrupt(USER_TIMER_INST, (DL_TIMER_INTERRUPT_ZERO_EVENT));
}

void Timer_start()
{
    DL_TimerG_enableClock(USER_TIMER_INST);
    DL_TimerG_enableInterrupt(USER_TIMER_INST, (DL_TIMER_INTERRUPT_ZERO_EVENT));
}

void Timer_stop()
{
    DL_TimerG_disableClock(USER_TIMER_INST);
    DL_TimerG_disableInterrupt(USER_TIMER_INST, (DL_TIMER_INTERRUPT_ZERO_EVENT));
}

void Timer_set_callback(timer_callback_t callback, void* args)
{
    timer_callback = callback;
    timer_callback_args = args;
}

void Timer_set_interval_ns(uint32_t interval)
{
    uint32_t frequency;

    (void)frequency;

    frequency = (uint32_t)((double)1000000000.0 / (double)interval);

    Timer_stop();

    USER_TIMER_INST->COUNTERREGS.LOAD = (((MCLK_FREQ_HZ/2)/frequency) - 1);

    Timer_start();
}