from testbench.spi import spi
from testbench.protocol import protocol

protocol_handle = protocol(serial_port = 'COM24',
                                    baud_rate = 115200,
                                    timeout=10,
                                    device_address = 0x10,
                                    symfile = '../test_bench.yaml')

spi_handle = spi(protocol_handle)

spi_handle.configure(bitrate = 1000000, mode = spi.mode.SPI_CPOL0_CPHA0, bit_order = spi.bit_order.SPI_BITORDER_MSB_FIRST, cs_polarity = spi.cs_polarity.SPI_CS_POLARITY_ACTIVE_LOW )

transaction_1 = spi.consecutive_transfer_instance(write_data = b'\x01\x02\x03\x04', pause_time_us = 250)
transaction_2 = spi.consecutive_transfer_instance(write_data = b'\x05\x06\x07\x08', pause_time_us = 10000)

for i in range(0, 10000):
    spi_handle.consecutive_transfer([transaction_1, transaction_2])

print("transaction 1 data:")
print(transaction_1.read_data)
print("transaction 2 data:")
print(transaction_2.read_data)