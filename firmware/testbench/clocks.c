/*
 * clocks.c
 *
 *  Created on: 30-May-2026
 *      Author: hp
 */

#include "clocks.h"

#include <stdbool.h>
#include <stdint.h>

#define CLOCKS_SYSTICK_MAX_VALUE        (CLOCKS_SYSTEM_FREQUENCY / 10)

volatile clock_t Clocks_systick_isr_count = 0;

void Systick_INT_Handler()
{
    Clocks_systick_isr_count++;
}

void Clocks_init()
{

}

int64_t Clocks_get_tick_count(void)
{
    clock_t ticks;
    uint32_t systick_count;

    systick_count = DL_SYSTICK_getValue();

    ticks = 0;
    ticks += ((clock_t)CLOCKS_SYSTICK_MAX_VALUE * (clock_t)Clocks_systick_isr_count );
    ticks += ((clock_t)CLOCKS_SYSTICK_MAX_VALUE - (clock_t)systick_count);

    return ticks;
}

clock_t Clocks_get_time_us(void)
{
    clock_t ticks;
    clock_t us;
    clock_t systick_count;

    systick_count = DL_SYSTICK_getValue();

    ticks = 0;
    ticks += ((clock_t)CLOCKS_SYSTICK_MAX_VALUE * (clock_t)Clocks_systick_isr_count);
    ticks += ((clock_t)CLOCKS_SYSTICK_MAX_VALUE - (clock_t)systick_count);

    us = CLOCKS_TICKS_TO_US(ticks);

    return us;
}

extern void Clocks_wait_us(clock_t us)
{
    clock_t start_time;
    clock_t stop_time;
    clock_t current_time;

    start_time = Clocks_get_time_us();
    current_time = start_time;
    stop_time = us + start_time;

    while(current_time < stop_time)
    {
        current_time = Clocks_get_time_us();
    }
}

void Clocks_wait_us_precise(clock_t us)
{
    uint32_t start;
    uint32_t now;
    uint32_t elapsed;
    uint32_t ticks;

    ticks = (uint32_t)((CLOCKS_SYSTEM_FREQUENCY / 1000000U) * us);

    start = DL_SYSTICK_getValue();

    do
    {
        now = DL_SYSTICK_getValue();

        elapsed = (start - now) & ((1U << 24) - 1);
    } while(elapsed < ticks);
}
