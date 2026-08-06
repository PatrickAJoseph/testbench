from testbench.uart import uart
from testbench.uart import protocol

protocol_handle = protocol.protocol(serial_port = 'COM24',
                                    baud_rate = 115200,
                                    device_address = 0x10,
                                    symfile = '../test_bench.yaml')

uart_handle = uart(protocol_handle)

uart_handle.configure(baudrate = 921600)

for i in range(0,100):
    tx_string = f'TX_COUNT: {i}\r\n'
    uart_handle.write(tx_string.encode('utf-8'))