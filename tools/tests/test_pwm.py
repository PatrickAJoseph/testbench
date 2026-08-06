from testbench.pwm import pwm
from testbench.pwm import protocol

from pytest import mark

import os
import time
import subprocess
import re
import math

########################################### Logic analyzer utility functions #############################################

SIGROK_CLI_PATH = "C:\\Program Files\\sigrok\\sigrok-cli\\sigrok-cli.exe"

FREQUENCY_TOLERANCE_PERCENTAGE = 0.1
DUTY_CYCLE_TOLERANCE_PERCENTAGE = 0.1

sampling_rates = [20000, 25000, 50000, 100000, 200000, 250000, 500000, 1000000, 2000000, 3000000, 
                  6000000, 8000000, 12000000, 16000000, 24000000]
sampling_depths = [100, 200, 500, 1000, 2000, 5000, 10000, 20000, 50000, 100000, 
                   1000000, 2000000, 5000000, 10000000, 20000000, 50000000]

def logic_analyzer_get_settings(frequency: int):
    
    optimal_sampling_rate = 0
    optimal_sampling_depth = sampling_depths[-1]
    required_sampling_depth = 0

    required_sampling_rate = int( float(frequency) / (FREQUENCY_TOLERANCE_PERCENTAGE * 0.01) )

    optimal_sampling_rate = sampling_rates[len(sampling_rates) - 1]

    for _sampling_rate in sampling_rates:
        if _sampling_rate >= required_sampling_rate:
            optimal_sampling_rate = _sampling_rate
            break

    required_sampling_depth = (int(optimal_sampling_rate * 5)/ frequency)

    for _sampling_depth in sampling_depths:
        if _sampling_depth >= required_sampling_depth:
                optimal_sampling_depth = _sampling_depth
                break

    # print("get_logic_analyzer_settings: sampling_rate: {_rate}".format(_rate = optimal_sampling_rate))
    # print("get_logic_analyzer_settings: sampling_depth: {_depth}".format(_depth = optimal_sampling_depth))

    return (optimal_sampling_rate, optimal_sampling_depth)

def logic_analyzer_capture(frequency: int , pwm_channel: int):

    _frequency = frequency

    pwm_log_file = open('pwm_test_log.txt', 'w')

    (rate, depth) = logic_analyzer_get_settings(frequency)

    logic_analyzer_channel = 0

    if pwm_channel == 0:
         logic_analyzer_channel = 1

    if pwm_channel == 1:
         logic_analyzer_channel = 2

    if pwm_channel == 2:
         logic_analyzer_channel = 3

    if pwm_channel == 3:
         logic_analyzer_channel = 4

    subprocess.run([
        SIGROK_CLI_PATH,
        "-d", "fx2lafw",
        "--config", f"samplerate={rate}",
        "--samples", str(depth),
        "-o", "capture.sr",
    ], check=True)

    result = subprocess.run([
        SIGROK_CLI_PATH,
        "-i", "capture.sr",
        "-P", f"pwm:data=D{logic_analyzer_channel}",
    ], check=True, text=True, stdout = pwm_log_file)

    pwm_log_file.close()

    pwm_log_file = open('pwm_test_log.txt', 'r')

    # pwm_log_file_contents = pwm_log_file.read()

    duty_sum = 0.0
    duty_count = 0

    frequency_sum = 0.0
    frequency_count = 0

    with open('pwm_test_log.txt', "r") as f:
        for line in f:

            duty = re.search(r'([\d.]+)%', line)
            if duty:
                duty_sum += float(duty.group(1))
                duty_count += 1
                continue

            frequency = re.search(r'([\d.]+)\s*(Hz|kHz|MHz)', line)

            if frequency:
                value = float(frequency.group(1))
                unit = frequency.group(2)

                if unit == "kHz":
                    value *= 1000.0
                elif unit == "MHz":
                    value *= 1000000.0

                frequency_sum += value
                frequency_count += 1

    pwm_log_file.close()

    if frequency_count == 0 or duty_count == 0:
        return (frequency_sum, duty_sum)

    return (frequency_sum / frequency_count, duty_sum/ duty_count)

################################################# Start of test cases ##########################################

pwm_channels = [pwm.channel.CHANNEL_0, pwm.channel.CHANNEL_1, pwm.channel.CHANNEL_2, pwm.channel.CHANNEL_3]

def test_pwm_enable_and_disable(test_harness):

    pwm_handle = pwm(test_harness.protocol_handle)

    for channel in pwm_channels:
        pwm_handle.set_frequency(channel_id = channel, frequency = 1000)
        pwm_handle.set_duty(channel_id = channel, duty = 0.5)
        pwm_handle.enable(channel_id = channel)

        (frequency, duty) = logic_analyzer_capture(1000, pwm_channel = channel.value)

        assert int(frequency) == 1000

        pwm_handle.disable(channel_id = channel)

        (frequency, duty) = logic_analyzer_capture(1000, pwm_channel = channel.value)

        assert int(frequency) == 0

SYSTEM_CLOCK = 80_000_000
MEASUREMENT_ERROR_PERCENT = 0.1
LOGIC_ANALYZER_SAMPLING_RATE = 24_000_000.0
LOGIC_ANALYZER_MARGIN_SAMPLES = 1
MEASUREMENT_GUARD_PERCENTAGE = 0.05
AVERAGING_FACTOR = 4

def get_frequency_limits(target_frequency: float):

    divider = math.floor(SYSTEM_CLOCK / target_frequency)

    generation_error_percent = (
        ((SYSTEM_CLOCK / target_frequency) / divider) - 1.0
    ) * 100.0

    sampling_interval = 1.0 / LOGIC_ANALYZER_SAMPLING_RATE

    required_time_resolution = (
        (MEASUREMENT_ERROR_PERCENT / 100.0) /
        target_frequency
    )

    effective_time_resolution = max(
        required_time_resolution,
        sampling_interval / AVERAGING_FACTOR
    )

    measurement_tolerance = (
        effective_time_resolution *
        target_frequency *
        100.0
        + MEASUREMENT_GUARD_PERCENTAGE
    )

    total_tolerance_percent = (
        generation_error_percent +
        measurement_tolerance
    )

    lower = target_frequency * (
        1.0 - total_tolerance_percent / 100.0
    )

    upper = target_frequency * (
        1.0 + total_tolerance_percent / 100.0
    )

    return lower, upper, total_tolerance_percent

@mark.parametrize('frequency',
                  [i for i in range(1, 100)] +
                  [(100 + (1*i)) for i in range(1, 900)] +
                  [(1000 + (10*i)) for i in range(1, 900)] +
                  [ (10000 + (100*i)) for i in range(1, 900)] +
                  [(100000 + (1000*i)) for i in range(1, 900)]
                  )
def test_pwm_set_frequency(test_harness, frequency):

    pwm_handle = pwm(test_harness.protocol_handle)

    for channel in pwm_channels:

        pwm_handle.set_frequency(channel_id=channel, frequency=frequency)
        pwm_handle.set_duty(channel_id=channel, duty=0.5)
        pwm_handle.enable(channel_id=channel)

        measured_frequency, duty = logic_analyzer_capture(
            frequency,
            pwm_channel=channel.value
        )

        lower, upper, tolerance = get_frequency_limits(frequency)

        assert lower <= measured_frequency <= upper, (
            f"Target={frequency:.0f} Hz, "
            f"Measured={measured_frequency:.2f} Hz, "
            f"Allowed=[{lower:.2f}, {upper:.2f}] Hz, "
            f"Tolerance={tolerance:.4f}%"
        )

@mark.parametrize('frequency', [ 1, 3, 5, 7, 9, 
                                10, 30, 50, 70, 90, 
                                100, 300, 500, 700, 900, 
                                1000, 3000, 5000, 7000, 9000,
                                10000, 30000, 50000, 70000, 90000, 
                                100000, 300000, 500000, 700000, 900000, 1000000 ])
@mark.parametrize('duty', [0.01, 0.011, 0.013, 0.015, 0.017, 0.019, 0.02, 0.03, 0.05, 0.07, 0.09, 0.091, 0.093, 0.095, 0.097, 0.1,
                           0.11, 0.111, 0.113, 0.115, 0.117, 0.119, 0.12, 0.13, 0.15, 0.17, 0.19, 0.191, 0.193, 0.195, 0.197, 0.2,
                           0.21, 0.211, 0.213, 0.215, 0.217, 0.219, 0.22, 0.23, 0.25, 0.27, 0.29, 0.291, 0.293, 0.295, 0.297, 0.3,
                           0.31, 0.311, 0.313, 0.315, 0.317, 0.319, 0.32, 0.33, 0.35, 0.37, 0.39, 0.391, 0.393, 0.395, 0.397, 0.4,
                           0.41, 0.411, 0.413, 0.415, 0.417, 0.419, 0.42, 0.43, 0.45, 0.47, 0.49, 0.491, 0.493, 0.495, 0.497, 0.4,
                           0.51, 0.511, 0.513, 0.515, 0.517, 0.519, 0.52, 0.53, 0.55, 0.57, 0.59, 0.591, 0.593, 0.595, 0.597, 0.6,
                           0.61, 0.611, 0.613, 0.615, 0.617, 0.619, 0.62, 0.63, 0.65, 0.67, 0.69, 0.691, 0.693, 0.695, 0.697, 0.7,
                           0.71, 0.711, 0.713, 0.715, 0.717, 0.719, 0.72, 0.73, 0.75, 0.77, 0.79, 0.791, 0.793, 0.795, 0.797, 0.8,
                           0.81, 0.811, 0.813, 0.815, 0.817, 0.819, 0.82, 0.83, 0.85, 0.87, 0.89, 0.891, 0.893, 0.895, 0.897, 0.9,
                           0.91, 0.911, 0.913, 0.915, 0.917, 0.919, 0.92, 0.93, 0.95, 0.97, 0.99, 0.991, 0.993, 0.995, 0.997, 0.1,
                           ])
def test_pwm_set_duty(test_harness, frequency, duty):

    pwm_handle = pwm(test_harness.protocol_handle)

    target_duty = duty

    tolerance = 0.0015

    (optimal_sampling_rate, optimal_sampling_size) = logic_analyzer_get_settings(frequency)

    minimum_duty_cycle_resolution = float(frequency)/float(optimal_sampling_rate)

    duty_cycle_resolution = (DUTY_CYCLE_TOLERANCE_PERCENTAGE * 0.01)

    if(duty_cycle_resolution < minimum_duty_cycle_resolution):
        duty_cycle_resolution = minimum_duty_cycle_resolution

    tolerance = duty_cycle_resolution

    for channel in pwm_channels:
        pwm_handle.set_frequency(channel_id = channel, frequency = frequency)
        pwm_handle.set_duty(channel_id = channel, duty = target_duty)
        pwm_handle.enable(channel_id = channel)

        (measured_frequency, measured_duty) = logic_analyzer_capture(frequency, pwm_channel = channel.value)

        measured_duty = measured_duty * 0.01

        assert measured_duty <= ( tolerance + target_duty ) and measured_duty >= (target_duty - tolerance)
