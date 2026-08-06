
#ifndef PWM_H_
#define PWM_H_

#include <stdint.h>
#include <stdbool.h>

#define PWM_CHANNEL0               0
#define PWM_CHANNEL1               1
#define PWM_CHANNEL2               2
#define PWM_CHANNEL3               3

#define PWM_MAX_FREQUENCY               (1000000U)

extern void PWM_init();
extern void PWM_enable(int channel);
extern void PWM_disable(int channel);
extern void PWM_set_frequency(int channel, int frequency);
extern void PWM_set_duty(int channel, float duty);

#endif /* PWM_H_ */
