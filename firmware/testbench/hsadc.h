/*

 * hsadc.h

 *

 *  Created on: 03-Jun-2026

 *      Author: hp

 */

 

#ifndef HSADC_H_

#define HSADC_H_

 

#include <stdint.h>

#include <stdbool.h>

 

#define HSADC_CHANNEL_0                    (0U)

#define HSADC_CHANNEL_1                    (1U)

#define HSADC_CHANNEL_BOTH                 (2U)

 

typedef enum adc_sampling_rate_enum {

    ADC_SAMPLING_RATE_1000KSPS = 0,

    ADC_SAMPLING_RATE_625KSPS,

    ADC_SAMPLING_RATE_500KSPS,

    ADC_SAMPLING_RATE_400KSPS,

    ADC_SAMPLING_RATE_250KSPS,

    ADC_SAMPLING_RATE_200KSPS,

    ADC_SAMPLING_RATE_125KSPS,

    ADC_SAMPLING_RATE_100KSPS,

    ADC_SAMPLING_RATE_75KSPS,

    ADC_SAMPLING_RATE_62K5SPS,

    ADC_SAMPLING_RATE_50KSPS,

    ADC_SAMPLING_RATE_25KSPS,

    ADC_SAMPLING_RATE_12K5SPS,

    ADC_SAMPLING_RATE_10KSPS,

    ADC_SAMPLING_RATE_7K5SPS,

    ADC_SAMPLING_RATE_6K25SPS,

    ADC_SAMPLING_RATE_5KSPS,

}adc_sampling_rate;

 

extern void HSADC_init();
extern void HSADC_set_sampling_rate(int rate);
extern uint16_t HSADC_get_sample(int channel, int index);
extern void HSADC_acquire();


#endif /* HSADC_H */