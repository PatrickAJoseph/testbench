/*
 * clocks.c
 *
 *  Created on: 30-May-2026
 *      Author: hp
 */

#include "clocks.h"

#include <stdbool.h>
#include <stdint.h>

#define CLOCKS_SYSTICK_MAX_VALUE        (16777216U)

volatile clock_t Clocks_systick_isr_count = 0;

void Systick_INT_Handler()
{
    Clocks_systick_isr_count++;
}

void Clocks_init()
{

}

/*
clock_t Clocks_get_tick_count(void)
{
    clock_t ticks;
    uint32_t systick_count;

    systick_count = DL_SYSTICK_getValue();

    ticks = 0;
    ticks += ((clock_t)CLOCKS_SYSTICK_MAX_VALUE * (clock_t)Clocks_systick_isr_count );
    ticks += ((clock_t)CLOCKS_SYSTICK_MAX_VALUE - (clock_t)systick_count);

    return ticks;
}
*/

clock_t Clocks_get_tick_count(void)
{
    uint32_t isr_count_1;
    uint32_t isr_count_2;
    uint32_t systick_count;

    do
    {
        isr_count_1 = Clocks_systick_isr_count;
        systick_count = DL_SYSTICK_getValue();
        isr_count_2 = Clocks_systick_isr_count;
    }
    while (isr_count_1 != isr_count_2);

    return
        (isr_count_1 * CLOCKS_SYSTICK_MAX_VALUE) +
        (CLOCKS_SYSTICK_MAX_VALUE - systick_count);
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

    start = (1U << 24) - DL_SYSTICK_getValue() + (Clocks_systick_isr_count * (1U << 24));

    do
    {
        now = (1U << 24) - DL_SYSTICK_getValue() + (Clocks_systick_isr_count * (1U << 24));

        elapsed = (now - start);
    } while(elapsed < ticks);
}
