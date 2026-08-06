from testbench.uart import uart
from testbench.uart import protocol
import time
import serial
import os

import ast
import threading
from queue import Queue

async_harness_tx_queue = Queue(16)

def async_harness_tx_task(port: serial.Serial):
    while(True):
        item = async_harness_tx_queue.get()

        if(item == "STOP"):
            break

        (tx_data, delay) = item

        time.sleep(delay)
        port.write(tx_data)



harness_uart_handle = serial.Serial(baudrate = 57600, port = 'COM11')

harness_uart_handle.reset_input_buffer()
harness_uart_handle.reset_output_buffer()

if harness_uart_handle.is_open:
    harness_uart_handle.close()

harness_uart_handle.open()

async_harness_tx_thread_handle = threading.Thread(target = async_harness_tx_task, args=(harness_uart_handle,))

async_harness_tx_thread_handle.start()


protocol_handle = protocol.protocol(serial_port = 'COM14', baud_rate = 115200, device_address = 0x10, timeout = 5.0, symfile = 'test_bench.yaml')

uart_handle = uart(protocol_handle)

uart_handle.configure(baudrate = 57600)

#uart_handle.write( data = [(i % 256) for i in range(0,1023)] )
#
#for x in range(1,1024):
#
#    data = [(i % 256) for i in range(0,x)]
#
#    uart_handle.start_read()
#    async_harness_tx_queue.put((bytes(data), 0.1))
#    time.sleep(0.2)
#    (status,rx_bytes) = uart_handle.stop_read()
#
#    assert x == len(rx_bytes)
#    assert rx_bytes == bytes(data)
#
#    os.system("cls")
#    print("uart.start_read: Test progress ({_iteration}/1024)  [{_percentage}%]".format(_iteration = x, _percentage = 100 * float(x) / float(1024) ))

#for x in range(1,1024):
#
#    data = [(i % 256) for i in range(0,x)]
#
#    async_harness_tx_queue.put((bytes(data), 0.2))
#    time.sleep(0.1)
#    (status,rx_bytes) = uart_handle.read_till_count(count = x)
#
#    assert x == len(rx_bytes)
#    assert rx_bytes == bytes(data)
#
#    os.system("cls")
#    print("uart.read_till_count: Test progress ({_iteration}/1024)  [{_percentage}%]".format(_iteration = x, _percentage = 100 * float(x) / float(1024) ))
#

#print(uart.transfer_status.OK)

#for x in range(2,128):
#
#    data = [(i % 256) for i in range(0,x+1)]
#
#    async_harness_tx_queue.put((bytes(data), 0.02))
#    time.sleep(0.01)
#    (status,rx_bytes) = uart_handle.read_till_char(character = chr(x), timeout_us = 100000)
#
#    os.system("cls")
#    print("uart.read_till_count: Test progress ({_iteration}/255)  [{_percentage}%]".format(_iteration = x, _percentage = 100 * float(x) / float(128) ))

#   assert x == len(rx_bytes) - 1
#    assert rx_bytes[:len(data)] == bytes(data)

#for i in range(2,1024):
#
#    harness_tx_data = [(y%256) for y in range(0,i)]
#    test_bench_tx_data = [((2*y)%256) for y in range(0,i)]
#
#    async_harness_tx_queue.put((harness_tx_data, ( ( float(10) * float(i) * 10 ) / float(115200) ) if (( ( float(10) * float(i) * 10 ) / float(115200) ) > 0.01) else 0.01 ))
#
#    (status, test_bench_rx_bytes) = uart_handle.transfer_till_count(test_bench_tx_data, rx_count = len(test_bench_tx_data), timeout_us = 1000000)
#
#    harness_rx_data = harness_uart_handle.read_all()
#    test_bench_tx_data = bytes(test_bench_tx_data)
#    harness_tx_data = bytes(harness_tx_data)
#    test_bench_rx_bytes = bytes(test_bench_rx_bytes)
#
#    assert harness_rx_data == test_bench_tx_data
#    assert len(harness_rx_data) == len(test_bench_tx_data)
#    assert harness_tx_data == test_bench_rx_bytes
#    assert len(harness_tx_data) == len(test_bench_rx_bytes)

# for i in range(2,255):
#
#    harness_tx_data = [(y%256) for y in range(0,i)]
#    test_bench_tx_data = [((y)%256) for y in range(0,i)]
#
#    async_harness_tx_queue.put((harness_tx_data, ( ( float(10) * float(i) * 10 ) / float(115200) ) if (( ( float(10) * float(i) * 10 ) / float(115200) ) > 0.01) else 0.01 ))
#
#    (status, test_bench_rx_bytes) = uart_handle.transfer_till_char(test_bench_tx_data, rx_char = (i-1).to_bytes(), timeout_us = 1000000)
#
#    harness_rx_data = harness_uart_handle.read_all()
#    test_bench_tx_data = bytes(test_bench_tx_data)
#    harness_tx_data = bytes(harness_tx_data)
#    test_bench_rx_bytes = bytes(test_bench_rx_bytes)
#
#    assert harness_rx_data == test_bench_tx_data
#    assert len(harness_rx_data) == len(test_bench_tx_data)
#    assert harness_tx_data == test_bench_rx_bytes
#    assert len(harness_tx_data) == len(test_bench_rx_bytes)

# async_harness_tx_queue.put((b'\x01\x02\x03\x04\x05\x06\x07\x08', 0.05))
# async_harness_tx_queue.put((b'\x01\x02\x03\x04\x05\x06', 0.05))
# async_harness_tx_queue.put((b'\x01\x02\x03\x04\x05\x06\x07\x08', 0.05))
# async_harness_tx_queue.put((b'\x01\x02\x03\x04\x05\x06', 0.05))


# transfer_1 = uart.consecutive_transfer_instance(tx_data = b'\x01\x02\x03\x04', target_rx_count = 8, timeout_us = 750000, pause_time_us = 1000)
# transfer_2 = uart.consecutive_transfer_instance(tx_data = b'\x01\x02\x03\x04\x05\x06\x07\x08', target_rx_count = 4, timeout_us = 1500000, pause_time_us = 1000)
# transfer_3 = uart.consecutive_transfer_instance(tx_data = b'\x01\x02\x03\x04\x05\x06\x07\x08\x09', target_rx_count = 4, timeout_us = 1500000, pause_time_us = 1000)
# transfer_4 = uart.consecutive_transfer_instance(tx_data = b'\x01\x02\x03\x04\x05\x06\x07\x08\x09\x0A', target_rx_count = 4, timeout_us = 1500000, pause_time_us = 1000)

# uart_handle.consecutive_transfer_till_char(transfers = [transfer_1, transfer_2, transfer_3, transfer_4], termination_char = b'\x05')

# print(transfer_1.rx_data)
# print(transfer_2.rx_data)
# print(transfer_3.rx_data)
# print(transfer_4.rx_data)

async_harness_tx_queue.put("STOP")

harness_uart_handle.close()