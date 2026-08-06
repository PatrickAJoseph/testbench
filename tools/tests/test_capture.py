from testbench.capture import capture
from testbench.pwm import pwm
from testbench.capture import protocol

from pytest import mark

import time
import yaml

CAPTURE_TOLERANCE_PERCENTAGE = 0.1

@mark.parametrize('target_frequency', [ 1 + 10*i for i in range(1, 1000) ] )
@mark.parametrize('target_duty', [
                           0.02, 0.03, 0.05, 0.07, 0.09, 0.091, 0.093, 0.095, 0.097, 0.1,
                           0.11, 0.111, 0.113, 0.115, 0.117, 0.119, 0.12, 0.13, 0.15, 0.17, 0.19, 0.191, 0.193, 0.195, 0.197, 0.2,
                           0.21, 0.211, 0.213, 0.215, 0.217, 0.219, 0.22, 0.23, 0.25, 0.27, 0.29, 0.291, 0.293, 0.295, 0.297, 0.3,
                           0.31, 0.311, 0.313, 0.315, 0.317, 0.319, 0.32, 0.33, 0.35, 0.37, 0.39, 0.391, 0.393, 0.395, 0.397, 0.4,
                           0.41, 0.411, 0.413, 0.415, 0.417, 0.419, 0.42, 0.43, 0.45, 0.47, 0.49, 0.491, 0.493, 0.495, 0.497, 0.5,
                           0.51, 0.511, 0.513, 0.515, 0.517, 0.519, 0.52, 0.53, 0.55, 0.57, 0.59, 0.591, 0.593, 0.595, 0.597, 0.6,
                           0.61, 0.611, 0.613, 0.615, 0.617, 0.619, 0.62, 0.63, 0.65, 0.67, 0.69, 0.691, 0.693, 0.695, 0.697, 0.7,
                           0.71, 0.711, 0.713, 0.715, 0.717, 0.719, 0.72, 0.73, 0.75, 0.77, 0.79, 0.791, 0.793, 0.795, 0.797, 0.8,
                           0.81, 0.811, 0.813, 0.815, 0.817, 0.819, 0.82, 0.83, 0.85, 0.87, 0.89, 0.891, 0.893, 0.895, 0.897, 0.9,
                           0.91, 0.911, 0.913, 0.915, 0.917, 0.919, 0.92, 0.93, 0.95, 0.97, 0.98
                           ])
@mark.parametrize('channel', [0, 1])
def test_capture_single_channel_range_1(test_harness, target_frequency, target_duty, channel):

    wait_time = 5.0/float(target_frequency)

    if wait_time < 0.01:
        wait_time = 0.01

    capture_handle = capture(test_harness.protocol_handle)

    pwm_handle = pwm(test_harness.protocol_handle)

    pwm_handle.set_frequency(channel_id = pwm.channel(channel), frequency = target_frequency)
    pwm_handle.set_duty(channel_id = pwm.channel(channel), duty = target_duty)
    pwm_handle.enable(channel_id = pwm.channel(channel))

    time.sleep(wait_time)

    capture_handle.start(channel_id = capture.channel(channel))
    time.sleep(wait_time)
    capture_handle.stop(channel_id = capture.channel(channel))

    pwm_handle.disable(channel_id = pwm.channel(channel))

    capture_data = capture_handle.get_data(channel_id = capture.channel(channel))

    percentage_error = 100.0 * (target_frequency - capture_data.frequency)/(target_frequency)

    target_frequency = target_frequency
    target_duty = target_duty
    target_period_ns = (float(1000_000_000)/target_frequency)
    target_on_time = int(float(target_duty) * float(target_period_ns))
    target_off_time = int( (1.0 - float(target_duty)) * float(target_period_ns) )

    min_measured_frequency = ( (1 - (CAPTURE_TOLERANCE_PERCENTAGE/100.0)) * target_frequency )
    max_measured_frequency = ( (1 + (CAPTURE_TOLERANCE_PERCENTAGE/100.0)) * target_frequency )
    min_measured_duty = ( (1 - (CAPTURE_TOLERANCE_PERCENTAGE/100.0)) * target_duty )
    max_measured_duty = ( (1 + (CAPTURE_TOLERANCE_PERCENTAGE/100.0)) * target_duty )
    min_measured_period_ns = ( (1 - (CAPTURE_TOLERANCE_PERCENTAGE/100.0)) * target_period_ns )
    max_measured_period_ns = ( (1 + (CAPTURE_TOLERANCE_PERCENTAGE/100.0)) * target_period_ns )
    min_on_time_ns = ( (1 - (CAPTURE_TOLERANCE_PERCENTAGE/100.0)) * target_on_time )
    max_on_time_ns = ( (1 + (CAPTURE_TOLERANCE_PERCENTAGE/100.0)) * target_on_time )
    min_off_time_ns = ( (1 - (CAPTURE_TOLERANCE_PERCENTAGE/100.0)) * target_off_time )
    max_off_time_ns = ( (1 + (CAPTURE_TOLERANCE_PERCENTAGE/100.0)) * target_off_time )

    if (target_on_time - min_on_time_ns) < 100:
        min_on_time_ns = min_on_time_ns - 100

    if (target_off_time - min_off_time_ns) < 100:
        min_off_time_ns = min_off_time_ns - 100

    if (max_on_time_ns - target_on_time) < 100:
        max_on_time_ns = max_on_time_ns + 100

    if (max_off_time_ns - target_off_time) < 100:
        max_off_time_ns = max_off_time_ns + 100

    assert min_measured_frequency <= round(capture_data.frequency, 3) <= max_measured_frequency
    assert min_measured_duty <= round(capture_data.duty, 3) <= max_measured_duty
    assert min_measured_period_ns <= capture_data.period_ns <= max_measured_period_ns
    assert min_on_time_ns <= capture_data.on_time_ns <= max_on_time_ns
    assert min_off_time_ns <= capture_data.off_time_ns <= max_off_time_ns

@mark.parametrize('target_frequency', [                                 
        10000 + 100*i for i in range(0, 400)
])
@mark.parametrize('target_duty', [
                           0.05, 0.07, 0.09, 0.091, 0.093, 0.095, 0.097, 0.1,
                           0.11, 0.111, 0.113, 0.115, 0.117, 0.119, 0.12, 0.13, 0.15, 0.17, 0.19, 0.191, 0.193, 0.195, 0.197, 0.2,
                           0.21, 0.211, 0.213, 0.215, 0.217, 0.219, 0.22, 0.23, 0.25, 0.27, 0.29, 0.291, 0.293, 0.295, 0.297, 0.3,
                           0.31, 0.311, 0.313, 0.315, 0.317, 0.319, 0.32, 0.33, 0.35, 0.37, 0.39, 0.391, 0.393, 0.395, 0.397, 0.4,
                           0.41, 0.411, 0.413, 0.415, 0.417, 0.419, 0.42, 0.43, 0.45, 0.47, 0.49, 0.491, 0.493, 0.495, 0.497, 0.5,
                           0.51, 0.511, 0.513, 0.515, 0.517, 0.519, 0.52, 0.53, 0.55, 0.57, 0.59, 0.591, 0.593, 0.595, 0.597, 0.6,
                           0.61, 0.611, 0.613, 0.615, 0.617, 0.619, 0.62, 0.63, 0.65, 0.67, 0.69, 0.691, 0.693, 0.695, 0.697, 0.7,
                           0.71, 0.711, 0.713, 0.715, 0.717, 0.719, 0.72, 0.73, 0.75, 0.77, 0.79, 0.791, 0.793, 0.795, 0.797, 0.8,
                           0.81, 0.811, 0.813, 0.815, 0.817, 0.819, 0.82, 0.83, 0.85, 0.87, 0.89, 0.891, 0.893, 0.895, 0.897, 
                           0.9, 0.91, 0.911, 0.913, 0.915, 0.917, 0.919, 0.92, 0.93, 0.95
                           ])
@mark.parametrize('channel', [0, 1])
def test_capture_single_channel_range_2(test_harness, target_frequency, target_duty, channel):

    wait_time = 5.0/float(target_frequency)

    if wait_time < 0.001:
        wait_time = 0.001

    capture_handle = capture(test_harness.protocol_handle)

    pwm_handle = pwm(test_harness.protocol_handle)

    pwm_handle.set_frequency(channel_id = pwm.channel(channel), frequency = target_frequency)
    pwm_handle.set_duty(channel_id = pwm.channel(channel), duty = target_duty)
    pwm_handle.enable(channel_id = pwm.channel(channel))

    capture_handle.start(channel_id = capture.channel(channel))
    time.sleep(wait_time)
    capture_handle.stop(channel_id = capture.channel(channel))

    pwm_handle.disable(channel_id = pwm.channel(channel))

    capture_data = capture_handle.get_data(channel_id = capture.channel(channel))

    percentage_error = 100.0 * (target_frequency - capture_data.frequency)/(target_frequency)

    target_frequency = target_frequency
    target_duty = target_duty
    target_period_ns = (float(1000_000_000)/target_frequency)
    target_on_time = int(float(target_duty) * float(target_period_ns))
    target_off_time = int( (1.0 - float(target_duty)) * float(target_period_ns) )

    min_measured_frequency = ( (1 - (CAPTURE_TOLERANCE_PERCENTAGE/10.0)) * target_frequency )
    max_measured_frequency = ( (1 + (CAPTURE_TOLERANCE_PERCENTAGE/10.0)) * target_frequency )
    min_measured_duty = ( (1 - (CAPTURE_TOLERANCE_PERCENTAGE/10.0)) * target_duty )
    max_measured_duty = ( (1 + (CAPTURE_TOLERANCE_PERCENTAGE/10.0)) * target_duty )
    min_measured_period_ns = ( (1 - (CAPTURE_TOLERANCE_PERCENTAGE/10.0)) * target_period_ns )
    max_measured_period_ns = ( (1 + (CAPTURE_TOLERANCE_PERCENTAGE/10.0)) * target_period_ns )
    min_on_time_ns = ( (1 - (CAPTURE_TOLERANCE_PERCENTAGE/10.0)) * target_on_time )
    max_on_time_ns = ( (1 + (CAPTURE_TOLERANCE_PERCENTAGE/10.0)) * target_on_time )
    min_off_time_ns = ( (1 - (CAPTURE_TOLERANCE_PERCENTAGE/10.0)) * target_off_time )
    max_off_time_ns = ( (1 + (CAPTURE_TOLERANCE_PERCENTAGE/10.0)) * target_off_time )

    if (target_on_time - min_on_time_ns) < 100:
        min_on_time_ns = min_on_time_ns - 100

    if (target_off_time - min_off_time_ns) < 100:
        min_off_time_ns = min_off_time_ns - 100

    if (max_on_time_ns - target_on_time) < 100:
        max_on_time_ns = max_on_time_ns + 100

    if (max_off_time_ns - target_off_time) < 100:
        max_off_time_ns = max_off_time_ns + 100

    assert min_measured_frequency <= round(capture_data.frequency, 2) <= max_measured_frequency
    assert round(min_measured_duty,2) <= round(capture_data.duty, 2) <= round(max_measured_duty,2)
    assert min_measured_period_ns <= capture_data.period_ns <= max_measured_period_ns
    assert min_on_time_ns <= capture_data.on_time_ns <= max_on_time_ns
    assert min_off_time_ns <= capture_data.off_time_ns <= max_off_time_ns

@mark.parametrize('target_frequency', [                                 
        1 + 10*i for i in range(1, 10000)
])
@mark.parametrize('channel', [0, 1])
def test_capture_single_channel_50_percentage_duty_cycle(test_harness, target_frequency, channel):

    target_duty = 0.5

    wait_time = 5.0/float(target_frequency)

    if wait_time < 0.001:
        wait_time = 0.001

    capture_handle = capture(test_harness.protocol_handle)

    pwm_handle = pwm(test_harness.protocol_handle)

    pwm_handle.set_frequency(channel_id = pwm.channel(channel), frequency = target_frequency)
    pwm_handle.set_duty(channel_id = pwm.channel(channel), duty = target_duty)
    pwm_handle.enable(channel_id = pwm.channel(channel))

    capture_handle.start(channel_id = capture.channel(channel))
    time.sleep(wait_time)
    capture_handle.stop(channel_id = capture.channel(channel))

    pwm_handle.disable(channel_id = pwm.channel(channel))

    capture_data = capture_handle.get_data(channel_id = capture.channel(channel))

    percentage_error = 100.0 * (target_frequency - capture_data.frequency)/(target_frequency)

    target_frequency = target_frequency
    target_duty = target_duty
    target_period_ns = (float(1000_000_000)/target_frequency)
    target_on_time = int(float(target_duty) * float(target_period_ns))
    target_off_time = int( (1.0 - float(target_duty)) * float(target_period_ns) )

    min_measured_frequency = ( (1 - (CAPTURE_TOLERANCE_PERCENTAGE/10.0)) * target_frequency )
    max_measured_frequency = ( (1 + (CAPTURE_TOLERANCE_PERCENTAGE/10.0)) * target_frequency )
    min_measured_duty = ( (1 - (CAPTURE_TOLERANCE_PERCENTAGE/10.0)) * target_duty )
    max_measured_duty = ( (1 + (CAPTURE_TOLERANCE_PERCENTAGE/10.0)) * target_duty )
    min_measured_period_ns = ( (1 - (CAPTURE_TOLERANCE_PERCENTAGE/10.0)) * target_period_ns )
    max_measured_period_ns = ( (1 + (CAPTURE_TOLERANCE_PERCENTAGE/10.0)) * target_period_ns )
    min_on_time_ns = ( (1 - (CAPTURE_TOLERANCE_PERCENTAGE/10.0)) * target_on_time )
    max_on_time_ns = ( (1 + (CAPTURE_TOLERANCE_PERCENTAGE/10.0)) * target_on_time )
    min_off_time_ns = ( (1 - (CAPTURE_TOLERANCE_PERCENTAGE/10.0)) * target_off_time )
    max_off_time_ns = ( (1 + (CAPTURE_TOLERANCE_PERCENTAGE/10.0)) * target_off_time )

    if (target_on_time - min_on_time_ns) < 100:
        min_on_time_ns = min_on_time_ns - 100

    if (target_off_time - min_off_time_ns) < 100:
        min_off_time_ns = min_off_time_ns - 100

    if (max_on_time_ns - target_on_time) < 100:
        max_on_time_ns = max_on_time_ns + 100

    if (max_off_time_ns - target_off_time) < 100:
        max_off_time_ns = max_off_time_ns + 100

    assert min_measured_frequency <= round(capture_data.frequency, 2) <= max_measured_frequency
    assert round(min_measured_duty,2) <= round(capture_data.duty, 2) <= round(max_measured_duty,2)
    assert min_measured_period_ns <= capture_data.period_ns <= max_measured_period_ns
    assert min_on_time_ns <= capture_data.on_time_ns <= max_on_time_ns
    assert min_off_time_ns <= capture_data.off_time_ns <= max_off_time_ns

@mark.parametrize('target_frequency', [                                 
        1 + 10*i for i in range(1, 5000)
])
def test_capture_dual_channel_50_percentage_duty_cycle(test_harness, target_frequency):

    target_duty = 0.5

    wait_time = 5.0/float(min(target_frequency, target_frequency + 50000))

    if wait_time < 0.001:
        wait_time = 0.001

    capture_handle = capture(test_harness.protocol_handle)

    pwm_handle = pwm(test_harness.protocol_handle)

    pwm_handle.set_frequency(channel_id = pwm.channel(0), frequency = target_frequency)
    pwm_handle.set_duty(channel_id = pwm.channel(0), duty = target_duty)
    pwm_handle.enable(channel_id = pwm.channel(0))

    pwm_handle.set_frequency(channel_id = pwm.channel(1), frequency = 50000 + target_frequency)
    pwm_handle.set_duty(channel_id = pwm.channel(1), duty = target_duty)
    pwm_handle.enable(channel_id = pwm.channel(1))

    capture_handle.start(channel_id = capture.channel(0))
    capture_handle.start(channel_id = capture.channel(1))
    time.sleep(wait_time)
    capture_handle.stop(channel_id = capture.channel(0))
    capture_handle.stop(channel_id = capture.channel(1))

    pwm_handle.disable(channel_id = pwm.channel(0))
    pwm_handle.disable(channel_id = pwm.channel(1))

    capture_data_ch0 = capture_handle.get_data(channel_id = capture.channel(0))
    capture_data_ch1 = capture_handle.get_data(channel_id = capture.channel(1))

    index = 0

    for capture_data in [capture_data_ch0, capture_data_ch1]:

        percentage_error = 100.0 * (target_frequency + 50000*index - capture_data.frequency)/(target_frequency + 50000*index)

        target_frequency = target_frequency
        target_duty = target_duty
        target_period_ns = (float(1000_000_000)/(target_frequency+50000*index))
        target_on_time = int(float(target_duty) * float(target_period_ns))
        target_off_time = int( (1.0 - float(target_duty)) * float(target_period_ns) )

        min_measured_frequency = ( (1 - (CAPTURE_TOLERANCE_PERCENTAGE/10.0)) * (target_frequency + 50000*index) )
        max_measured_frequency = ( (1 + (CAPTURE_TOLERANCE_PERCENTAGE/10.0)) * (target_frequency + 50000*index) )
        min_measured_duty = ( (1 - (CAPTURE_TOLERANCE_PERCENTAGE/10.0)) * target_duty )
        max_measured_duty = ( (1 + (CAPTURE_TOLERANCE_PERCENTAGE/10.0)) * target_duty )
        min_measured_period_ns = ( (1 - (CAPTURE_TOLERANCE_PERCENTAGE/10.0)) * target_period_ns )
        max_measured_period_ns = ( (1 + (CAPTURE_TOLERANCE_PERCENTAGE/10.0)) * target_period_ns )
        min_on_time_ns = ( (1 - (CAPTURE_TOLERANCE_PERCENTAGE/10.0)) * target_on_time )
        max_on_time_ns = ( (1 + (CAPTURE_TOLERANCE_PERCENTAGE/10.0)) * target_on_time )
        min_off_time_ns = ( (1 - (CAPTURE_TOLERANCE_PERCENTAGE/10.0)) * target_off_time )
        max_off_time_ns = ( (1 + (CAPTURE_TOLERANCE_PERCENTAGE/10.0)) * target_off_time )

        if (target_on_time - min_on_time_ns) < 100:
            min_on_time_ns = min_on_time_ns - 100

        if (target_off_time - min_off_time_ns) < 100:
            min_off_time_ns = min_off_time_ns - 100

        if (max_on_time_ns - target_on_time) < 100:
            max_on_time_ns = max_on_time_ns + 100

        if (max_off_time_ns - target_off_time) < 100:
            max_off_time_ns = max_off_time_ns + 100

        assert min_measured_frequency <= round(capture_data.frequency, 2) <= max_measured_frequency
        assert round(min_measured_duty,2) <= round(capture_data.duty, 2) <= round(max_measured_duty,2)
        assert min_measured_period_ns <= capture_data.period_ns <= max_measured_period_ns
        assert min_on_time_ns <= capture_data.on_time_ns <= max_on_time_ns
        assert min_off_time_ns <= capture_data.off_time_ns <= max_off_time_ns

        index = index + 1

    target_duty = 0.5

    wait_time = 5.0/float(min(target_frequency, target_frequency + 50000))

    if wait_time < 0.001:
        wait_time = 0.001

    capture_handle = capture(test_harness.protocol_handle)

    pwm_handle = pwm(test_harness.protocol_handle)

    pwm_handle.set_frequency(channel_id = pwm.channel(1), frequency = target_frequency)
    pwm_handle.set_duty(channel_id = pwm.channel(1), duty = target_duty)
    pwm_handle.enable(channel_id = pwm.channel(1))

    pwm_handle.set_frequency(channel_id = pwm.channel(0), frequency = 50000 + target_frequency)
    pwm_handle.set_duty(channel_id = pwm.channel(0), duty = target_duty)
    pwm_handle.enable(channel_id = pwm.channel(0))

    capture_handle.start(channel_id = capture.channel(0))
    capture_handle.start(channel_id = capture.channel(1))
    time.sleep(wait_time)
    capture_handle.stop(channel_id = capture.channel(0))
    capture_handle.stop(channel_id = capture.channel(1))

    pwm_handle.disable(channel_id = pwm.channel(0))
    pwm_handle.disable(channel_id = pwm.channel(1))

    capture_data_ch0 = capture_handle.get_data(channel_id = capture.channel(0))
    capture_data_ch1 = capture_handle.get_data(channel_id = capture.channel(1))

    index = 0

    for capture_data in [capture_data_ch0, capture_data_ch1]:

        percentage_error = 100.0 * (target_frequency + 50000*(1-index) - capture_data.frequency)/(target_frequency + 50000*(1-index))

        target_frequency = target_frequency
        target_duty = target_duty
        target_period_ns = (float(1000_000_000)/(target_frequency + 50000*(1-index)))
        target_on_time = int(float(target_duty) * float(target_period_ns))
        target_off_time = int( (1.0 - float(target_duty)) * float(target_period_ns) )

        min_measured_frequency = ( (1 - (CAPTURE_TOLERANCE_PERCENTAGE/10.0)) * (target_frequency + 50000*(1-index)) )
        max_measured_frequency = ( (1 + (CAPTURE_TOLERANCE_PERCENTAGE/10.0)) * (target_frequency + 50000*(1-index)) )
        min_measured_duty = ( (1 - (CAPTURE_TOLERANCE_PERCENTAGE/10.0)) * target_duty )
        max_measured_duty = ( (1 + (CAPTURE_TOLERANCE_PERCENTAGE/10.0)) * target_duty )
        min_measured_period_ns = ( (1 - (CAPTURE_TOLERANCE_PERCENTAGE/10.0)) * target_period_ns )
        max_measured_period_ns = ( (1 + (CAPTURE_TOLERANCE_PERCENTAGE/10.0)) * target_period_ns )
        min_on_time_ns = ( (1 - (CAPTURE_TOLERANCE_PERCENTAGE/10.0)) * target_on_time )
        max_on_time_ns = ( (1 + (CAPTURE_TOLERANCE_PERCENTAGE/10.0)) * target_on_time )
        min_off_time_ns = ( (1 - (CAPTURE_TOLERANCE_PERCENTAGE/10.0)) * target_off_time )
        max_off_time_ns = ( (1 + (CAPTURE_TOLERANCE_PERCENTAGE/10.0)) * target_off_time )

        if (target_on_time - min_on_time_ns) < 100:
            min_on_time_ns = min_on_time_ns - 100

        if (target_off_time - min_off_time_ns) < 100:
            min_off_time_ns = min_off_time_ns - 100

        if (max_on_time_ns - target_on_time) < 100:
            max_on_time_ns = max_on_time_ns + 100

        if (max_off_time_ns - target_off_time) < 100:
            max_off_time_ns = max_off_time_ns + 100

        assert min_measured_frequency <= round(capture_data.frequency, 2) <= max_measured_frequency
        assert round(min_measured_duty,2) <= round(capture_data.duty, 2) <= round(max_measured_duty,2)
        assert min_measured_period_ns <= capture_data.period_ns <= max_measured_period_ns
        assert min_on_time_ns <= capture_data.on_time_ns <= max_on_time_ns
        assert min_off_time_ns <= capture_data.off_time_ns <= max_off_time_ns

        index = index + 1
