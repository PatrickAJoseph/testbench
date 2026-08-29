from testbench.i2c import i2c
from testbench.protocol import protocol

protocol_handle = protocol(serial_port = 'COM24',
                                    baud_rate = 115200,
                                    timeout=10,
                                    device_address = 0x10,
                                    symfile = '../test_bench.yaml')

i2c_handle = i2c(protocol_handle)

i2c_handle.configure(mode = i2c.mode.I2C_MODE_STANDARD, address = 0x3C, timeout_us = 10000)
(response, status) = i2c_handle.transfer(write_bytes = b'\x00\x01\x02\x03', read_count = 4)
print(response)
print(status)

transfer_instance_1 = i2c.consecutive_transfer_instance(write_data = b'\x01\x02\x03\x04\x05',
                                                      read_size = 5,
                                                      pause_time_us = 27500)

transfer_instance_2 = i2c.consecutive_transfer_instance(write_data = b'\xB1\xA2\xC3\xB4\xA5',
                                                      read_size = 128,
                                                      pause_time_us = 2500)

for i in range(0,100):

    i2c_handle.consecutive_transfer([transfer_instance_1, transfer_instance_2])

    print("transfer instance 1: read data")
    print(transfer_instance_1.read_data)
    print("transfer instance 2: read data")
    print(transfer_instance_2.read_data)
    print("transfer instance 1: status")
    print(transfer_instance_1.status)
    print("transfer instance 2: status")
    print(transfer_instance_2.status)