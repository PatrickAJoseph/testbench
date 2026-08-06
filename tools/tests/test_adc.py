from testbench.adc import adc
from testbench.pwm import pwm

from pytest import mark

import time
import yaml

def get_sampling_rate_enum_value(frequency: int, channel: adc.channels):

    sampling_rate = adc.sampling_rate.SAMPLING_RATE_1000KSPS

    if channel == adc.channels.CHANNEL_BOTH:
        sampling_rate = adc.sampling_rate.SAMPLING_RATE_500KSPS

    if( (frequency >= 500000) and (frequency < 1000000) ):
        sampling_rate = adc.sampling_rate.SAMPLING_RATE_1000KSPS

        if channel == adc.channels.CHANNEL_BOTH:
            sampling_rate = adc.sampling_rate.SAMPLING_RATE_500KSPS

    if( (frequency >= 250000) and (frequency < 500000) ):
        sampling_rate = adc.sampling_rate.SAMPLING_RATE_500KSPS

    if( (frequency >= 125000) and (frequency < 250000) ):
        sampling_rate = adc.sampling_rate.SAMPLING_RATE_250KSPS

    if( (frequency >= 62500) and (frequency < 125000) ):
        sampling_rate = adc.sampling_rate.SAMPLING_RATE_125KSPS

    if( (frequency >= 31250) and (frequency < 62500) ):
        sampling_rate = adc.sampling_rate.SAMPLING_RATE_62K5SPS

    if( frequency < 31250 and frequency > 15625 ):
        sampling_rate = adc.sampling_rate.SAMPLING_RATE_31K25SPS

    if(frequency < 15625):
        sampling_rate = adc.sampling_rate.SAMPLING_RATE_15K625SPS

    return sampling_rate

def get_sampling_rate_enum_frequency_value(sampling_rate):
    sampling_frequency = 1000000

    if(sampling_rate == adc.sampling_rate.SAMPLING_RATE_15K625SPS):
        sampling_frequency = 15625

    if(sampling_rate == adc.sampling_rate.SAMPLING_RATE_31K25SPS):
        sampling_frequency = 31250

    if(sampling_rate == adc.sampling_rate.SAMPLING_RATE_62K5SPS):
        sampling_frequency = 62500
 
    if(sampling_rate == adc.sampling_rate.SAMPLING_RATE_125KSPS):
        sampling_frequency = 125000

    if(sampling_rate == adc.sampling_rate.SAMPLING_RATE_250KSPS):
        sampling_frequency = 250000

    if(sampling_rate == adc.sampling_rate.SAMPLING_RATE_500KSPS):
        sampling_frequency = 500000

    if(sampling_rate == adc.sampling_rate.SAMPLING_RATE_1000KSPS):
        sampling_frequency = 1000000

    return sampling_frequency

#
#   Returns the following tuple
#   1. Maximum amplitude
#   2. Minimum amplitude
#   3. Mean amplitude
#   4. ON time
#   5. OFF time
#   6. duty cycle
#   

class adc_capture_data:
    def __init__(self):
        self.max_amplitude: int = 0
        self.min_amplitude: int = 0
        self.mean_amplitude: int = 0
        self.on_time_ns: int = 0
        self.off_time_ns: int = 0
        self.duty_cycle: float = 0.0
        self.frequency: int = 0
        self.period_ns: int = 0

def analyze_captured_data(sampling_rate, samples) -> adc_capture_data:

    print(f"Sampling rate: {sampling_rate}")

    capture_data = adc_capture_data();

    max_value = max(samples)
    min_value = min(samples)

    mean_value = (max_value + min_value)//2

    capture_data.max_amplitude = max_value
    capture_data.min_amplitude = min_value
    capture_data.mean_amplitude = mean_value

    frequency = get_sampling_rate_enum_frequency_value(sampling_rate)

    logic = [ sample >= mean_value for sample in samples ]

    low_count:int = 0
    high_count:int = 0

    capture_list = []

    print(f"Sample logic waveform: {logic}")

    for index in range( 1, len(logic) ):
        if logic[index] != logic[index-1] and logic[index] == True:
            capture_list.append((high_count, low_count))
            low_count = 0
            high_count = 1
        elif logic[index] == logic[index-1] and logic[index] == True:
            high_count = high_count + 1
        elif logic[index] == logic[index-1] and logic[index] == False:
            low_count = low_count + 1
        elif logic[index] != logic[index-1] and logic[index] == False:
            low_count = 1 

    if(frequency >= 100):
        capture_list = capture_list[2:]
        capture_list = capture_list[:len(capture_list) - 2]
    else:
        capture_list = capture_list[1:]

    print(f"Capture list length: {len(capture_list)}")

    mean_low_count = 0
    mean_high_count = 0

    index = 0

    for index in range(0, len(capture_list)):

        mean_high_count = mean_high_count + capture_list[index][0]
        mean_low_count = mean_low_count + capture_list[index][1]

    mean_high_count /= len(capture_list)
    mean_low_count /= len(capture_list)

    periods = [h + l for h, l in capture_list]
    period_count = sum(periods) / len(periods)

    capture_data.duty_cycle = float(mean_high_count) / float(period_count)
    capture_data.frequency = int( float(frequency) / float(period_count) )
    capture_data.period_ns = 1000_000_000.0 / float(capture_data.frequency)
    capture_data.on_time_ns = int( float(capture_data.period_ns) * float(capture_data.duty_cycle) )
    capture_data.off_time_ns = capture_data.period_ns - capture_data.on_time_ns

    print(f"Mean high count: {mean_high_count}")
    print(f"Mean low count: {mean_low_count}")
    print(f"Capture data maximum amplitude :{capture_data.max_amplitude}")
    print(f"Capture data minimum amplitude :{capture_data.min_amplitude}")
    print(f"Capture data mean amplitude :{capture_data.mean_amplitude}")
    print(f"Capture data frequency (Hz) :{capture_data.frequency}")
    print(f"Capture data duty cycle (%) :{capture_data.duty_cycle * 100.0}")
    print(f"Capture data on time (ns) :{capture_data.on_time_ns}")
    print(f"Capture data off time (ns) :{capture_data.off_time_ns}")
    print(f"Capture data period (ns) :{capture_data.period_ns}")

    return capture_data

@mark.description("Provide PWM signals of frequencies from 50 Hz to 100kHz." \
                  "Check if samples are proper. This is a time axis test and" \
                  "not an amplitude axis test.")
@mark.parametrize('frequency', [50, 75, 100, 110, 130, 150, 170, 190,
                                200, 300, 400, 500, 600, 700, 800, 
                                810, 830, 850, 870, 890, 900, 1000,
                                1100, 1300, 1500, 1700, 1900,
                                2000, 3000, 4000, 5000, 6000, 7000, 8000, 9000,
                                10000, 11000, 13000, 15000, 17000, 19000,
                                20000, 30000, 40000, 50000, 60000, 70000, 80000, 90000, 100000] )
@mark.parametrize('duty', [0.1, 0.11, 0.13, 0.15, 0.17, 0.19, 0.2, 0.3, 0.4, 0.5, 0.6 ,0.7 , 0.8, 0.81, 0.83, 0.85, 0.87, 0.89, 0.9])
@mark.parametrize('channel', [int(adc.channels.CHANNEL_0.value), int(adc.channels.CHANNEL_1.value)])
def test_adc_frequency_single_channel(test_harness, channel, frequency, duty):

    tolerance = 4.0

    if(frequency > 1000):
        tolerance = 8.0

    pwm_handle = pwm(test_harness.protocol_handle)

    pwm_handle.set_frequency(channel_id = pwm.channel(channel), frequency = frequency)
    pwm_handle.set_duty(channel_id = pwm.channel(channel), duty = duty)
    pwm_handle.enable(channel_id = pwm.channel(channel))

    adc_handle = adc(test_harness.protocol_handle)

    minimum_sampling_frequency = 150 * frequency

    sampling_rate = get_sampling_rate_enum_value(minimum_sampling_frequency, adc.channels(channel))
    #sampling_rate = adc.sampling_rate.SAMPLING_RATE_500KSPS

    time.sleep(0.1)

    adc_handle.configure( channel = adc.channels(channel), sampling_rate = sampling_rate )
    samples = adc_handle.acquire()

    print(f"Input frequency: {frequency}")
    print(f"Input duty: {duty}")
    print(f"Chosen sampling rate: {sampling_rate}")
    print(f"Channel 0 samples: {samples.channel_0}")
    print(f"Channel 1 samples: {samples.channel_1}")

    capture_data = analyze_captured_data(sampling_rate, samples.channel_0 if channel == 0 else samples.channel_1 )

    actual_sampling_frequency = get_sampling_rate_enum_frequency_value(sampling_rate)

    sampling_time = 1/actual_sampling_frequency

    frequency_min = int(1/((1/frequency) + tolerance*(sampling_time)))
    frequency_max = int(1/((1/frequency) - tolerance*(sampling_time)))

    target_on_time = float(duty)/float(frequency)
    target_off_time = float(1.0-duty)/float(frequency)
    target_period = target_on_time + target_off_time

    min_on_time_ns = int( 1000_000_000.0 * ( target_on_time - tolerance*sampling_time ) )
    max_on_time_ns = int( 1000_000_000.0 * ( target_on_time + tolerance*sampling_time ) )
    min_off_time_ns = int( 1000_000_000.0 * ( target_off_time - tolerance*sampling_time ) )
    max_off_time_ns = int( 1000_000_000.0 * ( target_off_time + tolerance*sampling_time ) )
    min_period_ns = int( 1000_000_000.0 * ( target_period - tolerance*sampling_time ) )
    max_period_ns = int( 1000_000_000.0 * ( target_period + tolerance*sampling_time ) )
    
    min_duty = float(min_on_time_ns)/float(max_period_ns)
    max_duty = float(max_on_time_ns)/float(min_period_ns)

    assert frequency_min <= capture_data.frequency <= frequency_max
    assert min_on_time_ns <= capture_data.on_time_ns <= max_on_time_ns
    assert min_off_time_ns <= capture_data.off_time_ns <= max_off_time_ns
    assert min_period_ns <= capture_data.period_ns <= max_period_ns
    assert min_duty <= capture_data.duty_cycle <= max_duty

    pwm_handle.disable(channel_id = pwm.channel(channel))

    time.sleep(0.1)


@mark.description("Provide PWM signals of frequencies from 100 Hz to 50kHz." \
                  "Check if samples are proper. This is a time axis test and" \
                  "not an amplitude axis test.")
@mark.parametrize('frequency', [100, 110, 130, 150, 170, 190,
                                200, 300, 400, 500, 600, 700, 800, 
                                810, 830, 850, 870, 890, 900, 1000,
                                1100, 1300, 1500, 1700, 1900,
                                2000, 3000, 4000, 5000, 6000, 7000, 8000, 9000,
                                10000, 11000, 13000, 15000, 17000, 19000,
                                20000, 30000, 40000, 50000] )
@mark.parametrize('duty', [0.1, 0.11, 0.13, 0.15, 0.17, 0.19, 0.2, 0.3, 0.4, 0.5, 0.6 ,0.7 , 0.8, 0.81, 0.83, 0.85, 0.87, 0.89, 0.9])
def test_adc_frequency_dual_channel(test_harness, frequency, duty):

    tolerance = 4.0

    if(frequency > 1000):
        tolerance = 8.0

    pwm_handle = pwm(test_harness.protocol_handle)

    pwm_handle.set_frequency(channel_id = pwm.channel(0), frequency = frequency)
    pwm_handle.set_duty(channel_id = pwm.channel(0), duty = duty)
    pwm_handle.enable(channel_id = pwm.channel(0))

    pwm_handle.set_frequency(channel_id = pwm.channel(1), frequency = frequency)
    pwm_handle.set_duty(channel_id = pwm.channel(1), duty = (1.0-duty))
    pwm_handle.enable(channel_id = pwm.channel(1))

    adc_handle = adc(test_harness.protocol_handle)

    minimum_sampling_frequency = 100 * frequency

    sampling_rate = get_sampling_rate_enum_value(minimum_sampling_frequency, adc.channels.CHANNEL_BOTH)
    #sampling_rate = adc.sampling_rate.SAMPLING_RATE_500KSPS

    time.sleep(0.1)

    adc_handle.configure( channel = adc.channels.CHANNEL_BOTH, sampling_rate = sampling_rate )
    samples = adc_handle.acquire()

    print(f"Input frequency: {frequency}")
    print(f"Input duty: {duty}")
    print(f"Chosen sampling rate: {sampling_rate}")
    print(f"Channel 0 samples: {samples.channel_0}")
    print(f"Channel 1 samples: {samples.channel_1}")

    pwm_handle.disable(channel_id = pwm.channel(0))
    pwm_handle.disable(channel_id = pwm.channel(1))

    capture_data = analyze_captured_data(sampling_rate, samples.channel_0)

    actual_sampling_frequency = get_sampling_rate_enum_frequency_value(sampling_rate)

    sampling_time = 1/actual_sampling_frequency

    frequency_min = int(1/((1/frequency) + tolerance*(sampling_time)))
    frequency_max = int(1/((1/frequency) - tolerance*(sampling_time)))

    target_on_time = float(duty)/float(frequency)
    target_off_time = float(1.0-duty)/float(frequency)
    target_period = target_on_time + target_off_time

    min_on_time_ns = int( 1000_000_000.0 * ( target_on_time - tolerance*sampling_time ) )
    max_on_time_ns = int( 1000_000_000.0 * ( target_on_time + tolerance*sampling_time ) )
    min_off_time_ns = int( 1000_000_000.0 * ( target_off_time - tolerance*sampling_time ) )
    max_off_time_ns = int( 1000_000_000.0 * ( target_off_time + tolerance*sampling_time ) )
    min_period_ns = int( 1000_000_000.0 * ( target_period - tolerance*sampling_time ) )
    max_period_ns = int( 1000_000_000.0 * ( target_period + tolerance*sampling_time ) )
    
    min_duty = float(min_on_time_ns)/float(max_period_ns)
    max_duty = float(max_on_time_ns)/float(min_period_ns)

    assert frequency_min <= capture_data.frequency <= frequency_max
    assert min_on_time_ns <= capture_data.on_time_ns <= max_on_time_ns
    assert min_off_time_ns <= capture_data.off_time_ns <= max_off_time_ns
    assert min_period_ns <= capture_data.period_ns <= max_period_ns
    assert min_duty <= capture_data.duty_cycle <= max_duty

    capture_data = analyze_captured_data(sampling_rate, samples.channel_1)

    actual_sampling_frequency = get_sampling_rate_enum_frequency_value(sampling_rate)

    sampling_time = 1/actual_sampling_frequency

    frequency_min = int(1/((1/frequency) + tolerance*(sampling_time)))
    frequency_max = int(1/((1/frequency) - tolerance*(sampling_time)))

    target_on_time = float(1.0-duty)/float(frequency)
    target_off_time = float(duty)/float(frequency)
    target_period = target_on_time + target_off_time

    min_on_time_ns = int( 1000_000_000.0 * ( target_on_time - tolerance*sampling_time ) )
    max_on_time_ns = int( 1000_000_000.0 * ( target_on_time + tolerance*sampling_time ) )
    min_off_time_ns = int( 1000_000_000.0 * ( target_off_time - tolerance*sampling_time ) )
    max_off_time_ns = int( 1000_000_000.0 * ( target_off_time + tolerance*sampling_time ) )
    min_period_ns = int( 1000_000_000.0 * ( target_period - tolerance*sampling_time ) )
    max_period_ns = int( 1000_000_000.0 * ( target_period + tolerance*sampling_time ) )
    
    min_duty = float(min_on_time_ns)/float(max_period_ns)
    max_duty = float(max_on_time_ns)/float(min_period_ns)

    assert frequency_min <= capture_data.frequency <= frequency_max
    assert min_on_time_ns <= capture_data.on_time_ns <= max_on_time_ns
    assert min_off_time_ns <= capture_data.off_time_ns <= max_off_time_ns
    assert min_period_ns <= capture_data.period_ns <= max_period_ns
    assert min_duty <= capture_data.duty_cycle <= max_duty

    time.sleep(0.1)
    