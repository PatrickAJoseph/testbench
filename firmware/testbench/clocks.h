/*
 * clocks.h
 *
 *  Created on: 30-May-2026
 *      Author: hp
 */

/**
 * Header file containing functions to initialize the main
 * clock and the SysTimer for overall system time keeping.
 */

#ifndef CLOCKS_H_
#define CLOCKS_H_

#include <stdint.h>

#include "device.h"
#include "ti_sdk_dl_config.h"

#define CLOCKS_SYSTEM_FREQUENCY         (MCLK_FREQ_HZ)
#define CLOCKS_TICKS_PER_US             (CLOCKS_SYSTEM_FREQUENCY/1000000UL)
#define CLOCKS_TICKS_PER_MS             (CLOCKS_SYSTEM_FREQUENCY/1000UL)
#define CLOCKS_TICKS_TO_US(x)                   \
    ( ( ((uint64_t)(x)) ) / ((uint64_t)CLOCKS_SYSTEM_FREQUENCY / (uint64_t)1000000) )
#define CLOCKS_TICKS_TO_NS(x)                   \
    ( ( ((uint64_t)(x)) * 1000000000 ) / (uint64_t)CLOCKS_SYSTEM_FREQUENCY )
#define CLOCKS_US_TO_TICKS(x)                   \
    ( ( ((uint64_t)(x)) * CLOCKS_SYSTEM_FREQUENCY ) / (uint64_t)1000000 )
#define CLOCKS_TICKS_TO_MS(x)                   \
    ( ( ((uint64_t)(x)) * (1000) ) / (uint64_t)CLOCKS_SYSTEM_FREQUENCY )
#define CLOCKS_MS_TO_TICKS(x)                   \
    ( ( ((uint64_t)(x)) * (CLOCKS_SYSTEM_FREQUENCY) ) / (uint64_t)1000 )

typedef uint64_t clock_t;

extern void Clocks_init();
extern clock_t Clocks_get_tick_count();
extern clock_t Clocks_get_time_us();
extern void Clocks_wait_us(clock_t us);
extern void Clocks_wait_us_precise(clock_t us);

#endif /* CLOCKS_H_ */
