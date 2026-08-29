from testbench.gpio import gpio
from testbench.protocol import protocol
import time

protocol_handle = protocol(serial_port = 'COM24',
                                    baud_rate = 115200,
                                    timeout=10,
                                    device_address = 0x10,
                                    symfile = '../test_bench.yaml')

gpio_handle = gpio(protocol_handle)

write_data = [ (i%16) for i in range(0,20) ]

gpio_handle.pattern_generator_configure_repeated(length = len(write_data), rate = 100000, count = 5)
gpio_handle.pattern_generator_write_output_pattern_repeated(pattern = write_data)
gpio_handle.pattern_generator_start()
gpio_handle.pattern_generator_stop()
print(gpio_handle.pattern_generator_read_input_pattern_repeated())