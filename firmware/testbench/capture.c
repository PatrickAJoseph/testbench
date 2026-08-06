#include "capture.h"
#include "clocks.h"

#include "device.h"
#include "ti_sdk_dl_config.h"

#define NANOSECONDS_PER_MCLK        (1000000000ULL/MCLK_FREQ_HZ)

struct capture_channel
{
    ECAP_Regs* base;
    uint32_t on_time_ns;
    uint32_t off_time_ns;
    uint32_t period_ns;
    uint32_t frequency;
    float duty;
    uint32_t on_count;
    uint32_t off_count;
};

struct capture_channel capture_channels[2] = { { .base = ECAP0, }, { .base = ECAP1, } };

void CAPTURE_ECAP0_INT_Handler()
{
    uint32_t cap1;
    uint32_t cap2;
    uint32_t cap3;
    uint32_t on_count;
    uint32_t off_count;

    (void)cap1;
    (void)cap2;
    (void)cap3;
    (void)on_count;
    (void)off_count;

    cap1 = DL_ECAP_getEventTimeStamp(ECAP0, DL_ECAP_EVENT_1);
    cap2 = DL_ECAP_getEventTimeStamp(ECAP0, DL_ECAP_EVENT_2);
    cap3 = DL_ECAP_getEventTimeStamp(ECAP0, DL_ECAP_EVENT_3);

    if(cap2 >= cap1)
    {
        on_count = cap2 - cap1;
    }
    else 
    {
        on_count = (uint32_t)(((uint64_t)(0xFFFFFFFF - cap1) + (uint64_t)cap2) + 1); 
    }

    if(cap3 >= cap2)
    {
        off_count = cap3 - cap2;
    }
    else 
    {
        off_count = (uint32_t)(((uint64_t)(0xFFFFFFFF - cap2) + (uint64_t)cap3) + 1); 
    }

    capture_channels[0].on_count = on_count;
    capture_channels[0].off_count = off_count;

    DL_ECAP_clearInterrupt(ECAP0,
                           ( DL_ECAP_ISR_SOURCE_CEVT1 | 
                             DL_ECAP_ISR_SOURCE_CEVT2 | 
                             DL_ECAP_ISR_SOURCE_CEVT3 |
                             DL_ECAP_ISR_SOURCE_CEVT4 |
                             DL_ECAP_ISR_SOURCE_CTROVF) );
    
    DL_ECAP_clearGlobalInterrupt(ECAP0);
}

void CAPTURE_ECAP1_INT_Handler()
{
    uint32_t cap1;
    uint32_t cap2;
    uint32_t cap3;
    uint32_t on_count;
    uint32_t off_count;

    (void)cap1;
    (void)cap2;
    (void)cap3;
    (void)on_count;
    (void)off_count;

    cap1 = DL_ECAP_getEventTimeStamp(ECAP1, DL_ECAP_EVENT_1);
    cap2 = DL_ECAP_getEventTimeStamp(ECAP1, DL_ECAP_EVENT_2);
    cap3 = DL_ECAP_getEventTimeStamp(ECAP1, DL_ECAP_EVENT_3);

    if(cap2 >= cap1)
    {
        on_count = cap2 - cap1;
    }
    else 
    {
        on_count = (uint32_t)(((uint64_t)(0xFFFFFFFF - cap1) + (uint64_t)cap2) + 1); 
    }

    if(cap3 >= cap2)
    {
        off_count = cap3 - cap2;
    }
    else 
    {
        off_count = (uint32_t)(((uint64_t)(0xFFFFFFFF - cap2) + (uint64_t)cap3) + 1); 
    }

    capture_channels[1].on_count = on_count;
    capture_channels[1].off_count = off_count;

    DL_ECAP_clearInterrupt(ECAP1,
                           ( DL_ECAP_ISR_SOURCE_CEVT1 | 
                             DL_ECAP_ISR_SOURCE_CEVT2 | 
                             DL_ECAP_ISR_SOURCE_CEVT3 |
                             DL_ECAP_ISR_SOURCE_CEVT4 |
                             DL_ECAP_ISR_SOURCE_CTROVF) );
    
    DL_ECAP_clearGlobalInterrupt(ECAP1);
}

void Capture_start(int channel)
{
    struct capture_channel* ch = &capture_channels[channel];

    DL_ECAP_startCounter(ch->base);
    DL_ECAP_enableTimeStampCapture(ch->base);
}

void Capture_stop(int channel)
{
    struct capture_channel* ch = &capture_channels[channel];

    DL_ECAP_stopCounter(ch->base);
    DL_ECAP_disableTimeStampCapture(ch->base);

    ch->on_time_ns = (uint32_t)((uint64_t)ch->on_count*(uint64_t)NANOSECONDS_PER_MCLK);
    ch->off_time_ns = (uint32_t)((uint64_t)ch->off_count*(uint64_t)NANOSECONDS_PER_MCLK);
    ch->period_ns = ch->on_time_ns + ch->off_time_ns;

    ch->frequency = (uint32_t)((double)1000000000.0/(double)ch->period_ns);
    ch->duty = (float)((double)ch->on_time_ns/(double)ch->period_ns);
}

capture_t Capture_get_frequency(int channel)
{
    return capture_channels[channel].frequency;
}

capture_t Capture_get_on_time_ns(int channel)
{
    return capture_channels[channel].on_time_ns;
}

capture_t Capture_get_off_time_ns(int channel)
{
    return capture_channels[channel].off_time_ns;
}

float Capture_get_duty_cycle(int channel)
{
    return capture_channels[channel].duty;
}

capture_t Capture_get_period_ns(int channel)
{
    return capture_channels[channel].period_ns;
}

void Capture_init()
{

}