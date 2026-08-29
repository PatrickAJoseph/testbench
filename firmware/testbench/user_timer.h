
#ifndef __USER_TIMER_H__
#define __USER_TIMER_H__ 

#include <stdint.h>

typedef void (*timer_callback_t)(void* args);

extern void Timer_init();
extern void Timer_start();
extern void Timer_stop();
extern void Timer_set_callback(timer_callback_t callback, void* args);
extern void Timer_set_interval_ns(uint32_t interval);

#endif /* __USER_TIMER_H__ */