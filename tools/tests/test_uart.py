from testbench.uart import uart
from testbench.uart import protocol
import threading

from pytest import mark

import serial
import time
import yaml

from queue import Queue

harness_serial_port_transmit_data_queue = Queue(16)

def get_pyserial_parity(parity):
    if parity == uart.parity.NONE:
        return serial.PARITY_NONE

    if parity == uart.parity.ODD:
        return serial.PARITY_ODD

    if parity == uart.parity.EVEN:
        return serial.PARITY_EVEN

    return None

def get_pyserial_stop_bits(stop_bits):
    if stop_bits == uart.stop_bits.ONE:
        return serial.STOPBITS_ONE

    if stop_bits == uart.stop_bits.TWO:
        return serial.STOPBITS_TWO

    return None

def harness_serial_port_transmit(port: serial.Serial):

    while(True):

        item = harness_serial_port_transmit_data_queue.get()

        if item == "STOP":
            break

        tx_bytes = item[0]
        wait_s = item[1]

        time.sleep(wait_s)
        port.write(tx_bytes)
        port.flush()

@mark.parametrize('baudrate', [300, 600, 900, 1200, 1800, 2400, 3600, 4800, 9600, 19200, 38400, 57600, 115200, 230400, 460800, 576000, 921600])
@mark.parametrize('payload_length', [1, 2, 4, 8, 16, 32, 64, 128, 256, 512, 1024])
@mark.parametrize('parity', [uart.parity.NONE, uart.parity.ODD, uart.parity.EVEN])
@mark.parametrize('stop_bits', [uart.stop_bits.ONE, uart.stop_bits.TWO])
@mark.description("Configure test bench with start baud rate, parity and stop bits. "
                  "Send bytes from user uart to harness serial port and checks of the harness serial port"
                  "has received all bytes properly")
def test_uart_configure(test_harness, baudrate, payload_length, parity: uart.parity, stop_bits: uart.stop_bits):
    print("test_uart_configure: started test for baud_rate = {_baudrate}, parity = {_parity}, stop bits = {_stop}".
          format(_baudrate = baudrate, _parity = parity, _stop = stop_bits))

    estimated_sleep_time = ((12.0 * float(payload_length)) / float(baudrate)) * float(2.0)            

    if estimated_sleep_time < 0.1:
        estimated_sleep_time = 0.1

    uart_handle = uart(test_harness.protocol_handle)

    uart_handle.configure( baudrate = baudrate, parity = parity, stop_bits = stop_bits )

    harness_serial_port = serial.Serial(port = test_harness.user_uart_serial_port_name,
                                        baudrate = baudrate,
                                        stopbits = get_pyserial_stop_bits(stop_bits),
                                        parity = get_pyserial_parity(parity))

    if(harness_serial_port.is_open):
        harness_serial_port.close()

    harness_serial_port.open()

    harness_serial_port.reset_input_buffer()
    harness_serial_port.reset_output_buffer()

    tx_data = bytes([ (i%256) for i in range(0, payload_length) ])

    uart_handle.write(bytes([ (i%256) for i in range(0, payload_length) ]))

    time.sleep(estimated_sleep_time)

    rx_bytes = harness_serial_port.read_all()

    harness_serial_port.close()

    assert len(rx_bytes) == payload_length
    assert rx_bytes == tx_data

@mark.parametrize('payload_length', [1, 2, 4, 8, 16, 32, 64, 128, 256, 512, 1024])
@mark.description("Make test bench send uart messages of different payload sizes"
                  "and verify that received bytes by harness serial port equals transmitted byets by test bench")
def test_uart_write(test_harness, payload_length: int):
    estimated_sleep_time = ((12.0 * float(payload_length)) / float(115200)) * float(2.0)

    uart_handle = uart(test_harness.protocol_handle)

    uart_handle.configure()

    harness_serial_port = serial.Serial(port = test_harness.user_uart_serial_port_name,
                                        baudrate = 115200)

    if(harness_serial_port.is_open):
        harness_serial_port.close()

    harness_serial_port.open()

    harness_serial_port.reset_input_buffer()
    harness_serial_port.reset_output_buffer()

    tx_data = bytes([ (i%256) for i in range(0, payload_length) ])

    uart_handle.write(bytes([ (i%256) for i in range(0, payload_length) ]))

    time.sleep(estimated_sleep_time)

    rx_bytes = harness_serial_port.read_all()

    harness_serial_port.close()

    assert len(rx_bytes) == payload_length
    assert rx_bytes == tx_data

@mark.parametrize('payload_length', [1, 2, 4, 8, 16, 32, 64, 128, 256, 512, 1024])
@mark.description("Make test bench monitor incoming bytes via UART. Send over defined number of bytes from" \
                  "harness serial port. Stop listening to incoming bytes from UART of test bench." \
                  "Verify if received bytes by test bench UART equals bytes transmitted by harness serial port.")
def test_uart_async_read(test_harness, payload_length: int):
    estimated_sleep_time = ((12.0 * float(payload_length)) / float(115200)) * float(2.0)

    uart_handle = uart(test_harness.protocol_handle)

    uart_handle.configure()

    harness_serial_port = serial.Serial(port = test_harness.user_uart_serial_port_name,
                                        baudrate = 115200)

    if(harness_serial_port.is_open):
        harness_serial_port.close()

    harness_serial_port.open()

    harness_serial_port.reset_input_buffer()
    harness_serial_port.reset_output_buffer()

    tx_data = bytes([ (i%256) for i in range(0, payload_length) ])

    uart_handle.start_read()

    harness_serial_port.write(tx_data)
    harness_serial_port.flush()

    time.sleep(estimated_sleep_time)

    (transfer_status, rx_bytes) = uart_handle.stop_read()

    harness_serial_port.close()

    assert len(rx_bytes) == payload_length
    assert rx_bytes == tx_data
    assert transfer_status == uart.transfer_status.OK

@mark.parametrize('parity', [uart.parity.NONE, uart.parity.ODD, uart.parity.EVEN])
@mark.parametrize('stop_bits', [uart.stop_bits.ONE, uart.stop_bits.TWO])
@mark.description("Make test bench monitor incoming bytes via UART. Send over defined number of bytes from" \
                  "harness serial port. Stop listening to incoming bytes from UART of test bench." \
                  "Verify if the test bench was able to identify parity, framing and overrun errors." \
                  "After error case is triggered, transmit UART frame with correct frame parameters.")
def test_uart_async_read_error_cases(test_harness, parity: uart.parity, stop_bits: uart.stop_bits):

    payload_length = 32

    estimated_sleep_time = ((12.0 * float(payload_length)) / float(115200)) * float(2.0)

    uart_handle = uart(test_harness.protocol_handle)

    uart_handle.configure(stop_bits = stop_bits, parity = parity)

    testbench_stop_bit_settings = [uart.stop_bits.ONE, uart.stop_bits.TWO]
    testbench_parity_settings = [uart.parity.NONE, uart.parity.ODD, uart.parity.EVEN]

    for harness_parity in testbench_parity_settings:
        user_uart_serial_port = serial.Serial(port = test_harness.user_uart_serial_port_name,
                                              parity = get_pyserial_parity(harness_parity),
                                              stopbits= get_pyserial_stop_bits(stop_bits),
                                              baudrate = 115200)

        user_uart_serial_port.reset_input_buffer()
        user_uart_serial_port.reset_output_buffer()

        uart_handle.configure(parity = parity, stop_bits = stop_bits)

        uart_handle.start_read()

        user_uart_serial_port.write(bytes([ (i%256) for i in range(0,payload_length) ]))

        time.sleep(estimated_sleep_time)

        (rx_status , rx_bytes) = uart_handle.stop_read()

        user_uart_serial_port.close()

        print("harness serial port parity: {_value}".format(_value = harness_parity))
        print("harness serial port stop bits: {_value}".format(_value = stop_bits))
        print("testbench UART parity: {_value}".format(_value = parity))
        print("testbench UART stop bits: {_value}".format(_value = stop_bits))
        print("testbench UART transfer status: {_value}".format(_value = rx_status))
        print("testbench UART transfer rx bytes: {_value}".format(_value = rx_bytes))

        if harness_parity == parity:
            assert rx_status == uart.transfer_status.OK
        else:
            if parity == uart.parity.NONE:
                assert rx_status == uart.transfer_status.FRAMING_ERROR
            else:
                if harness_parity == uart.parity.NONE:
                    assert rx_status == uart.transfer_status.FRAMING_ERROR
                else:
                    assert rx_status == uart.transfer_status.PARITY_ERROR

@mark.parametrize('payload_length', [32, 64, 128, 256, 512, 1024])
@mark.description("Send over defined number of bytes over harness serial port." \
                  "Read the known number of bytes from test bench's UART." \
                  "Verify timeout event is detected properly." \
                  "If parity/framing error occurs before timeout, this event should take priority.")
def test_uart_read_till_count(test_harness, payload_length):

    user_uart_serial_port = serial.Serial(port = test_harness.user_uart_serial_port_name, baudrate = 115200)

    user_uart_serial_port.reset_input_buffer()
    user_uart_serial_port.reset_output_buffer()
    user_uart_serial_port.flush()

    uart_handle = uart(test_harness.protocol_handle)

    uart_handle.configure()

    tx_data = bytes([ (i%256) for i in range(0,payload_length) ])

    harness_serial_port_transmit_thread = threading.Thread( target = harness_serial_port_transmit, args = (user_uart_serial_port, ) )
    harness_serial_port_transmit_thread.start()

    harness_serial_port_transmit_data_queue.put((tx_data, 0.5))
    (rx_status, rx_data) = uart_handle.read_till_count(count = payload_length, timeout_us = 1000000)

    assert tx_data == rx_data
    assert rx_status == uart.transfer_status.OK

    harness_serial_port_transmit_data_queue.put("STOP")

    harness_serial_port_transmit_thread.join()

    user_uart_serial_port.close()

    user_uart_serial_port = serial.Serial(port = test_harness.user_uart_serial_port_name, baudrate = 115200)

    user_uart_serial_port.reset_input_buffer()
    user_uart_serial_port.reset_output_buffer()

    uart_handle = uart(test_harness.protocol_handle)

    uart_handle.configure(baudrate = 115200)

    tx_data = bytes([ (i%256) for i in range(0,payload_length-1) ])

    harness_serial_port_transmit_thread = threading.Thread( target = harness_serial_port_transmit, args = (user_uart_serial_port, ) )
    harness_serial_port_transmit_thread.start()

    harness_serial_port_transmit_data_queue.put((tx_data, 0.5))
    (rx_status, rx_data) = uart_handle.read_till_count(count = payload_length, timeout_us = 1000000)

    assert rx_data == tx_data[:payload_length - 1]
    assert rx_status == uart.transfer_status.TIMEOUT

    harness_serial_port_transmit_data_queue.put("STOP")

    harness_serial_port_transmit_thread.join()

    user_uart_serial_port.close()



@mark.description("Harness serial port transmits bytes from \x00 to \xFF and the termination character" \
                  "is incremented from \x01 to \xFF. Framing/parity errors must take priority over timeout events.")
def test_uart_read_till_char(test_harness):

    user_uart_serial_port = serial.Serial(port = test_harness.user_uart_serial_port_name, baudrate = 115200)

    uart_handle = uart(test_harness.protocol_handle)

    uart_handle.configure(baudrate = 115200)

    tx_data = bytes( [(i % 256) for i in range(0,256)] )

    harness_serial_port_transmit_thread = threading.Thread( target = harness_serial_port_transmit, args = (user_uart_serial_port, ) )
    harness_serial_port_transmit_thread.start()

    for termination_character in range(1,256):
        harness_serial_port_transmit_data_queue.put((tx_data, 0.1))
        time.sleep(0.05)
        (rx_status, rx_data) = uart_handle.read_till_char(rx_char = termination_character.to_bytes(), timeout_us = 1000000)
 
        print(rx_data)
        print(termination_character)

        assert rx_status == uart.transfer_status.OK
        assert rx_data == tx_data[:(termination_character + 1)]

    harness_serial_port_transmit_data_queue.put("STOP")

    harness_serial_port_transmit_thread.join()

    user_uart_serial_port.close()

    user_uart_serial_port = serial.Serial(port = test_harness.user_uart_serial_port_name, baudrate = 115200)

    uart_handle = uart(test_harness.protocol_handle)

    uart_handle.configure(baudrate = 115200)

    tx_data = bytes( [(i % 256) for i in range(0,128)] )

    harness_serial_port_transmit_thread = threading.Thread( target = harness_serial_port_transmit, args = (user_uart_serial_port, ) )
    harness_serial_port_transmit_thread.start()

    termination_character = 129

    harness_serial_port_transmit_data_queue.put((tx_data, 1.0))
    (rx_status, rx_data) = uart_handle.read_till_char(rx_char = termination_character.to_bytes(), timeout_us = 1500000)

    assert rx_status == uart.transfer_status.TIMEOUT
    assert rx_data == tx_data[:(termination_character - 1)]

    harness_serial_port_transmit_data_queue.put("STOP")

    harness_serial_port_transmit_thread.join()

    user_uart_serial_port.close()

@mark.description("Harness serial port sends over (payload_length/2) number of bytes and" \
                  "tetsbench uart sends over payload_length*2 number of bytes. Condition" \
                  "to pass is that harness serial port tx bytes equals testbench uart rx bytes" \
                  "and harness serial port rx bytes equals testbench uart tx bytes")
@mark.parametrize('payload_length', [2, 4, 8, 16, 32, 64, 128, 256, 512])
def test_uart_transfer_till_count(test_harness, payload_length):

    harness_serial_port = serial.Serial(port = test_harness.user_uart_serial_port_name, baudrate = 115200)

    uart_handle = uart(test_harness.protocol_handle)

    uart_handle.configure(baudrate = 115200)

    harness_tx_length = payload_length//2
    testbench_tx_length = payload_length * 2

    harness_tx_data = bytes([(i%256) for i in range(0, harness_tx_length)])
    testbench_tx_data = bytes([(i % 256) for i in range(0, testbench_tx_length)])

    harness_serial_port_transmit_thread = threading.Thread( target = harness_serial_port_transmit, args = (harness_serial_port, ) )
    harness_serial_port_transmit_thread.start()

    harness_serial_port_transmit_data_queue.put(( harness_tx_data, 0.25 ))
    (testbench_tx_status, testbench_rx_data) = uart_handle.transfer_till_count( tx_data = testbench_tx_data, rx_count = harness_tx_length, timeout_us = 2000000 )

    harness_rx_data = harness_serial_port.read_all()

    assert harness_tx_data == testbench_rx_data
    assert harness_rx_data == testbench_tx_data
    assert testbench_tx_status == uart.transfer_status.OK

    harness_serial_port_transmit_data_queue.put("STOP")
    harness_serial_port_transmit_thread.join()
    harness_serial_port.close()

@mark.description("Harness serial port sends over bytes 0 to x and" \
                  "tetsbench uart sends over 0 to 256-x number of bytes. Condition" \
                  "to pass is that harness serial port tx bytes equals testbench uart rx bytes" \
                  "and harness serial port rx bytes equals testbench uart tx bytes")
def test_uart_transfer_till_char(test_harness):

    harness_serial_port = serial.Serial(port = test_harness.user_uart_serial_port_name, baudrate = 115200)

    uart_handle = uart(test_harness.protocol_handle)

    uart_handle.configure(baudrate = 115200)

    harness_serial_port_transmit_thread = threading.Thread( target = harness_serial_port_transmit, args = (harness_serial_port, ) )
    harness_serial_port_transmit_thread.start()

    for termination_char in range(0, 255):

        harness_tx_length = termination_char + 1
        testbench_tx_length = 256 - (termination_char + 1)

        harness_tx_data = bytes([(i%256) for i in range(0, harness_tx_length)])
        testbench_tx_data = bytes([(i % 256) for i in range(0, testbench_tx_length)])

        harness_serial_port_transmit_data_queue.put(( harness_tx_data, 0.5 ))
        (testbench_tx_status, testbench_rx_data) = uart_handle.transfer_till_char( tx_data = testbench_tx_data, rx_char = termination_char.to_bytes(), timeout_us = 2000000 )

        harness_rx_data = harness_serial_port.read_all()

        assert harness_tx_data == testbench_rx_data
        assert harness_rx_data == testbench_tx_data
        assert testbench_tx_status == uart.transfer_status.OK

    harness_serial_port_transmit_data_queue.put("STOP")
    harness_serial_port_transmit_thread.join()
    harness_serial_port.close()

@mark.description("Create a list of consecutive transfers of variable TX and RX lengths."
                  "Verify if harness tx bytes == testbench rx bytes and vice versa.")
@mark.parametrize('number_of_transfers', [1, 2, 3, 4, 5, 6, 7, 8])
def test_uart_consecutive_transfer_till_count(test_harness, number_of_transfers):

    harness_serial_port = serial.Serial( test_harness.user_uart_serial_port_name, baudrate = 115200, timeout = 5.0 )

    uart_handle = uart(test_harness.protocol_handle)

    uart_handle.configure(baudrate = 115200)

    harness_serial_port_transmit_thread = threading.Thread( target = harness_serial_port_transmit, args = (harness_serial_port, ) )
    harness_serial_port_transmit_thread.start()

    testbench_tx_data = []
    harness_tx_data = []
    expected_rx_count = []
    transfers = []

    for i in range(0,number_of_transfers):
        testbench_tx_data_length = 2**i
        harness_tx_data_length = 2**(9-i)
        expected_rx_count.append(harness_tx_data_length)
        testbench_tx_data.append( bytes([ (i%256) for i in range(0, testbench_tx_data_length) ]) )
        harness_tx_data.append( bytes([ (i%256) for i in range(0, harness_tx_data_length) ]) )
        transfers.append( uart.consecutive_transfer_instance(tx_data = testbench_tx_data[i], 
                                                             target_rx_count = harness_tx_data_length,
                                                             timeout_us = 1000000,
                                                             pause_time_us = 10000) )

    for i in range(0, number_of_transfers):
        harness_serial_port_transmit_data_queue.put((harness_tx_data[i], 0.1))

    completed_transfers = uart_handle.consecutive_transfer_till_count(transfers)

    assert len(completed_transfers) == len(transfers)

    index = 0

    for transfer in completed_transfers:
        assert transfer.rx_data == harness_tx_data[index]
        assert transfer.rx_count == len(harness_tx_data[index])
        assert transfer.rx_status == uart.transfer_status.OK

        harness_rx_data = harness_serial_port.read(len(transfer.tx_data))

        assert transfer.tx_data == harness_rx_data

        index = index + 1

    harness_serial_port_transmit_data_queue.put("STOP")
    harness_serial_port_transmit_thread.join()
    harness_serial_port.close()

@mark.description("Create a list of consecutive transfers of variable TX and RX lengths."
                  "Verify if harness tx bytes == testbench rx bytes and vice versa."
                  "Termination character is parameterized")
@mark.parametrize('number_of_transfers', [1, 2, 3, 4, 5, 6, 7, 8])
@mark.parametrize('termination_character', [b'\x10', b'\x20', b'\x30', b'\x40', b'\x50', b'\x60', b'\x7F'])
def test_uart_consecutive_transfer_till_char(test_harness, number_of_transfers, termination_character):

    harness_serial_port = serial.Serial( test_harness.user_uart_serial_port_name, baudrate = 115200, timeout = 5.0 )

    uart_handle = uart(test_harness.protocol_handle)

    uart_handle.configure(baudrate = 115200)

    harness_serial_port_transmit_thread = threading.Thread( target = harness_serial_port_transmit, args = (harness_serial_port, ) )
    harness_serial_port_transmit_thread.start()

    testbench_tx_data = []
    harness_tx_data = []
    expected_rx_count = []
    transfers = []

    increment_in_steps_of_two = False

    for i in range(0,number_of_transfers):
        testbench_tx_data_length = 2**i

        harness_tx_data_length = int(termination_character[0]) + 1

        expected_rx_count.append(harness_tx_data_length)
        testbench_tx_data.append( bytes([ (i%256) for i in range(0, testbench_tx_data_length) ]) )
        harness_tx_data.append( bytes([ (i%256) for i in range(0, harness_tx_data_length) ]) )
        transfers.append( uart.consecutive_transfer_instance(tx_data = testbench_tx_data[i], 
                                                             timeout_us = 1000000,
                                                             pause_time_us = 10000) )

    for i in range(0, number_of_transfers):
        harness_serial_port_transmit_data_queue.put((harness_tx_data[i], 0.1))

    completed_transfers = uart_handle.consecutive_transfer_till_char(transfers, termination_character)

    assert len(completed_transfers) == len(transfers)

    index = 0

    for transfer in completed_transfers:
        assert transfer.rx_data == harness_tx_data[index]
        assert transfer.rx_count == len(harness_tx_data[index])
        assert transfer.rx_status == uart.transfer_status.OK

        harness_rx_data = harness_serial_port.read(len(transfer.tx_data))

        assert transfer.tx_data == harness_rx_data

        index = index + 1

    harness_serial_port_transmit_data_queue.put("STOP")
    harness_serial_port_transmit_thread.join()
    harness_serial_port.close()
