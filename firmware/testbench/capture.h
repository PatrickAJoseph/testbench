
#ifndef CAPTURE_H_
#define CAPTURE_H_

#include <stdint.h>
#include <stdbool.h>

#define CAPTURE_CHANNEL0        0
#define CAPTURE_CHANNEL1        1

typedef uint32_t capture_t;

extern void Capture_init();
extern void Capture_start(int channel);
extern void Capture_stop(int channel);
extern capture_t Capture_get_frequency(int channel);
extern capture_t Capture_get_on_time_ns(int channel);
extern capture_t Capture_get_off_time_ns(int channel);
extern float Capture_get_duty_cycle(int channel);
extern capture_t Capture_get_period_ns(int channel);

#endif /* CAPTURE_H_ */
