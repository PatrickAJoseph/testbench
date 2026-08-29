from enum import Enum
from . import protocol
import ast

MAX_TRANSFER_LENGTH = 1024
TRANSFER_CHUNK_SIZE = 200

class uart:

    class parity(Enum):
        NONE = 0
        ODD = 1
        EVEN = 2
        
    class stop_bits(Enum):
        ONE = 0
        TWO = 1

    class transfer_status(Enum):
        OK = 0
        TIMEOUT = 1
        RECEIVE_BUFFER_FULL = 2
        FRAMING_ERROR = 3
        PARITY_ERROR = 4
        OVERRUN_ERROR = 5
    
    class consecutive_transfer_instance:
        
        def __init__(self, tx_data: bytes, timeout_us: int, pause_time_us: int, target_rx_count: int = 0):
            self.tx_data = tx_data
            self.rx_data = b''
            self.target_rx_count = target_rx_count
            self.timeout_us = timeout_us
            self.pause_time_us = pause_time_us
            self.rx_count = 0
            self.rx_status = uart.transfer_status.OK

    def __init__(self, protocol_handle: protocol.protocol):
        self.protocol_handle: protocol.protocol = protocol_handle
        self.baudrate: int = 115200
        self.parity = self.parity.NONE
        self.stop_bits = self.stop_bits.ONE
    
    def __bytes_to_list(self,data: bytes):

        _data = []

        for byte in data:
            _data.append(int(byte))
        
        return _data

    def configure(self, baudrate: int = 115200 , parity: parity = parity.NONE , stop_bits: stop_bits = stop_bits.ONE):
        
        self.protocol_handle.set_request_value('uart_configure:baud_rate', baudrate)
        self.protocol_handle.set_request_value('uart_configure:parity', parity.value)
        self.protocol_handle.set_request_value('uart_configure:stop_bits', stop_bits.value)

        self.protocol_handle.send_command('uart_configure')
    
    def write(self, data = b''):

        _data: bytes = b''

        if(type(data) == type('0')):
            _data = data.encode('utf-8')
        
        if(type(data) == type(b'\x01')):
            _data = data

        if(type(data) == type([])):
            _data = bytes(data)

        payload = self.__bytes_to_list(_data)

        length = len(payload)

        if(length > MAX_TRANSFER_LENGTH):
            raise ValueError("uart:write: number of bytes to be written is greater than maximum transfer length {_length}".format(_length = MAX_TRANSFER_LENGTH))

        if(length < TRANSFER_CHUNK_SIZE):
            self.protocol_handle.set_request_value('uart_write:reset', int(True))
            self.protocol_handle.set_request_value('uart_write:start', 1)
            self.protocol_handle.set_request_value('uart_write:length', length)
            self.protocol_handle.set_request_value('uart_write:payload', str(payload))
            self.protocol_handle.send_command('uart_write')
        elif( length % TRANSFER_CHUNK_SIZE != 0 ):
            number_of_chunks = int((length + TRANSFER_CHUNK_SIZE - 1)// TRANSFER_CHUNK_SIZE)

            for chunk in range(0, number_of_chunks):
                is_last_chunk = chunk == (number_of_chunks - 1)
                self.protocol_handle.set_request_value('uart_write:reset', int(chunk == 0))
                self.protocol_handle.set_request_value('uart_write:start', int( is_last_chunk ))
                self.protocol_handle.set_request_value('uart_write:length', TRANSFER_CHUNK_SIZE if not is_last_chunk else (length % TRANSFER_CHUNK_SIZE) )
                self.protocol_handle.set_request_value('uart_write:payload', str( payload[(TRANSFER_CHUNK_SIZE*chunk): (TRANSFER_CHUNK_SIZE*(chunk+1))] if not is_last_chunk else payload[(TRANSFER_CHUNK_SIZE*chunk): (TRANSFER_CHUNK_SIZE*(chunk)) + (length % TRANSFER_CHUNK_SIZE)] ))
                self.protocol_handle.send_command('uart_write')

        else:
            number_of_chunks = int(length/TRANSFER_CHUNK_SIZE)

            for chunk in range(0, number_of_chunks):
                is_last_chunk = chunk == (number_of_chunks - 1)
                self.protocol_handle.set_request_value('uart_write:reset', int(chunk == 0))
                self.protocol_handle.set_request_value('uart_write:start', int( is_last_chunk ))
                self.protocol_handle.set_request_value('uart_write:length', TRANSFER_CHUNK_SIZE)
                self.protocol_handle.set_request_value('uart_write:payload', str( payload[(TRANSFER_CHUNK_SIZE*chunk): (TRANSFER_CHUNK_SIZE*(chunk+1))] ))
                self.protocol_handle.send_command('uart_write')
    
        status = int(self.protocol_handle.get_response_value('uart_write:status'))

        return status

    def start_read(self):

        self.protocol_handle.set_request_value('uart_read:reset', int(True))
        self.protocol_handle.set_request_value('uart_read:start', int(True))
        self.protocol_handle.set_request_value('uart_read:stop', int(False))
        self.protocol_handle.set_request_value('uart_read:blocking_mode', int(False))
        self.protocol_handle.set_request_value('uart_read:character_termination_mode', int(False))
        self.protocol_handle.set_request_value('uart_read:terminal_byte', 0)
        self.protocol_handle.set_request_value('uart_read:length', 0)
        self.protocol_handle.set_request_value('uart_read:timeout', 0)

        self.protocol_handle.send_command('uart_read')
    
    def stop_read(self):

        self.protocol_handle.set_request_value('uart_read:reset', int(False))
        self.protocol_handle.set_request_value('uart_read:start', int(False))
        self.protocol_handle.set_request_value('uart_read:stop', int(True))
        self.protocol_handle.set_request_value('uart_read:blocking_mode', int(False))
        self.protocol_handle.set_request_value('uart_read:character_termination_mode', int(False))
        self.protocol_handle.set_request_value('uart_read:terminal_byte', 0)
        self.protocol_handle.set_request_value('uart_read:length', 0)
        self.protocol_handle.set_request_value('uart_read:timeout', 0)

        self.protocol_handle.send_command('uart_read')

        status = int(self.protocol_handle.get_response_value('uart_read:status'))
        length = int(self.protocol_handle.get_response_value('uart_read:length'))

        rx_bytes = b''
        rx_length: int = 0

        if length < TRANSFER_CHUNK_SIZE:

            self.protocol_handle.set_request_value('uart_get_from_read_buffer:start_index', 0)
            self.protocol_handle.set_request_value('uart_get_from_read_buffer:length', length)

            self.protocol_handle.send_command('uart_get_from_read_buffer')

            rx_bytes = bytes(ast.literal_eval(self.protocol_handle.get_response_value('uart_get_from_read_buffer:received_bytes')))
            rx_length = rx_length + int(self.protocol_handle.get_response_value('uart_get_from_read_buffer:length'))

        elif (length % TRANSFER_CHUNK_SIZE != 0):

            number_of_chunks = ( length + TRANSFER_CHUNK_SIZE - 1 )// TRANSFER_CHUNK_SIZE
            is_last_chunk = False

            for chunk in range(0, number_of_chunks):

                if(chunk == (number_of_chunks - 1)):
                    is_last_chunk = True

                self.protocol_handle.set_request_value('uart_get_from_read_buffer:start_index', TRANSFER_CHUNK_SIZE * chunk)

                if(is_last_chunk):
                    self.protocol_handle.set_request_value('uart_get_from_read_buffer:length', (length % TRANSFER_CHUNK_SIZE))
                else:
                    self.protocol_handle.set_request_value('uart_get_from_read_buffer:length', TRANSFER_CHUNK_SIZE)

                self.protocol_handle.send_command('uart_get_from_read_buffer')

                rx_bytes = rx_bytes + bytes(ast.literal_eval(self.protocol_handle.get_response_value('uart_get_from_read_buffer:received_bytes')))
                rx_length = rx_length + int(self.protocol_handle.get_response_value('uart_get_from_read_buffer:length'))

        elif (length % TRANSFER_CHUNK_SIZE == 0):

            number_of_chunks = length // TRANSFER_CHUNK_SIZE

            for chunk in range(0, number_of_chunks):

                self.protocol_handle.set_request_value('uart_get_from_read_buffer:start_index', TRANSFER_CHUNK_SIZE * chunk)

                self.protocol_handle.set_request_value('uart_get_from_read_buffer:length', TRANSFER_CHUNK_SIZE)
                
                self.protocol_handle.send_command('uart_get_from_read_buffer')

                rx_bytes = rx_bytes + bytes(ast.literal_eval(self.protocol_handle.get_response_value('uart_get_from_read_buffer:received_bytes')))
                rx_length = rx_length + int(self.protocol_handle.get_response_value('uart_get_from_read_buffer:length'))

        return (self.transfer_status(status),bytes(rx_bytes))
    
    def read_till_count(self, count: int = 0, timeout_us: int = 1000000):

        if(count > MAX_TRANSFER_LENGTH):
            raise ValueError("read_till_count: target number of bytes to be read > {_length}".format(_length = MAX_TRANSFER_LENGTH))

        self.protocol_handle.set_request_value('uart_read:reset', int(True))
        self.protocol_handle.set_request_value('uart_read:start', int(True))
        self.protocol_handle.set_request_value('uart_read:stop', int(False))
        self.protocol_handle.set_request_value('uart_read:blocking_mode', int(True))
        self.protocol_handle.set_request_value('uart_read:character_termination_mode', int(False))
        self.protocol_handle.set_request_value('uart_read:terminal_byte', 0)
        self.protocol_handle.set_request_value('uart_read:length', count)
        self.protocol_handle.set_request_value('uart_read:timeout', timeout_us)

        self.protocol_handle.send_command('uart_read')

        status = int(self.protocol_handle.get_response_value('uart_read:status'))
        length = int(self.protocol_handle.get_response_value('uart_read:length'))

        rx_bytes = b''
        rx_length: int = 0

        if length < TRANSFER_CHUNK_SIZE:

            self.protocol_handle.set_request_value('uart_get_from_read_buffer:start_index', 0)
            self.protocol_handle.set_request_value('uart_get_from_read_buffer:length', length)

            self.protocol_handle.send_command('uart_get_from_read_buffer')

            rx_bytes = bytes(ast.literal_eval(self.protocol_handle.get_response_value('uart_get_from_read_buffer:received_bytes')))
            rx_length = rx_length + int(self.protocol_handle.get_response_value('uart_get_from_read_buffer:length'))

        elif (length % TRANSFER_CHUNK_SIZE != 0):

            number_of_chunks = ( length + TRANSFER_CHUNK_SIZE - 1 )// TRANSFER_CHUNK_SIZE
            is_last_chunk = False

            for chunk in range(0, number_of_chunks):

                if(chunk == (number_of_chunks - 1)):
                    is_last_chunk = True

                self.protocol_handle.set_request_value('uart_get_from_read_buffer:start_index', TRANSFER_CHUNK_SIZE * chunk)

                if(is_last_chunk):
                    self.protocol_handle.set_request_value('uart_get_from_read_buffer:length', (length % TRANSFER_CHUNK_SIZE))
                else:
                    self.protocol_handle.set_request_value('uart_get_from_read_buffer:length', TRANSFER_CHUNK_SIZE)

                self.protocol_handle.send_command('uart_get_from_read_buffer')

                rx_bytes = rx_bytes + bytes(ast.literal_eval(self.protocol_handle.get_response_value('uart_get_from_read_buffer:received_bytes')))
                rx_length = rx_length + int(self.protocol_handle.get_response_value('uart_get_from_read_buffer:length'))

        elif (length % TRANSFER_CHUNK_SIZE == 0):

            number_of_chunks = length // TRANSFER_CHUNK_SIZE

            for chunk in range(0, number_of_chunks):

                self.protocol_handle.set_request_value('uart_get_from_read_buffer:start_index', TRANSFER_CHUNK_SIZE * chunk)

                self.protocol_handle.set_request_value('uart_get_from_read_buffer:length', TRANSFER_CHUNK_SIZE)
                    
                self.protocol_handle.send_command('uart_get_from_read_buffer')

                rx_bytes = rx_bytes + bytes(ast.literal_eval(self.protocol_handle.get_response_value('uart_get_from_read_buffer:received_bytes')))
                rx_length = rx_length + int(self.protocol_handle.get_response_value('uart_get_from_read_buffer:length'))

        return (self.transfer_status(status),bytes(rx_bytes))

    def read_till_char(self, rx_char, timeout_us: int = 1000000):

        self.protocol_handle.set_request_value('uart_read:reset', int(True))
        self.protocol_handle.set_request_value('uart_read:start', int(True))
        self.protocol_handle.set_request_value('uart_read:stop', int(False))
        self.protocol_handle.set_request_value('uart_read:blocking_mode', int(True))
        self.protocol_handle.set_request_value('uart_read:character_termination_mode', int(True))
        self.protocol_handle.set_request_value('uart_read:terminal_byte', int(rx_char[0]))
        self.protocol_handle.set_request_value('uart_read:length', 0)
        self.protocol_handle.set_request_value('uart_read:timeout', timeout_us)

        self.protocol_handle.send_command('uart_read')

        status = int(self.protocol_handle.get_response_value('uart_read:status'))
        length = int(self.protocol_handle.get_response_value('uart_read:length'))

        rx_bytes = b''
        rx_length: int = 0

        if length < TRANSFER_CHUNK_SIZE:

            self.protocol_handle.set_request_value('uart_get_from_read_buffer:start_index', 0)
            self.protocol_handle.set_request_value('uart_get_from_read_buffer:length', length)

            self.protocol_handle.send_command('uart_get_from_read_buffer')

            rx_bytes = bytes(ast.literal_eval(self.protocol_handle.get_response_value('uart_get_from_read_buffer:received_bytes')))
            rx_length = rx_length + int(self.protocol_handle.get_response_value('uart_get_from_read_buffer:length'))

        elif (length % TRANSFER_CHUNK_SIZE != 0):

            number_of_chunks = ( length + TRANSFER_CHUNK_SIZE - 1 )// TRANSFER_CHUNK_SIZE
            is_last_chunk = False

            for chunk in range(0, number_of_chunks):

                if(chunk == (number_of_chunks - 1)):
                    is_last_chunk = True

                self.protocol_handle.set_request_value('uart_get_from_read_buffer:start_index', TRANSFER_CHUNK_SIZE * chunk)

                if(is_last_chunk):
                    self.protocol_handle.set_request_value('uart_get_from_read_buffer:length', (length % TRANSFER_CHUNK_SIZE))
                else:
                    self.protocol_handle.set_request_value('uart_get_from_read_buffer:length', TRANSFER_CHUNK_SIZE)

                self.protocol_handle.send_command('uart_get_from_read_buffer')

                rx_bytes = rx_bytes + bytes(ast.literal_eval(self.protocol_handle.get_response_value('uart_get_from_read_buffer:received_bytes')))
                rx_length = rx_length + int(self.protocol_handle.get_response_value('uart_get_from_read_buffer:length'))

        elif (length % TRANSFER_CHUNK_SIZE == 0):

            number_of_chunks = length // TRANSFER_CHUNK_SIZE

            for chunk in range(0, number_of_chunks):

                self.protocol_handle.set_request_value('uart_get_from_read_buffer:start_index', TRANSFER_CHUNK_SIZE * chunk)

                self.protocol_handle.set_request_value('uart_get_from_read_buffer:length', TRANSFER_CHUNK_SIZE)
                    
                self.protocol_handle.send_command('uart_get_from_read_buffer')

                rx_bytes = rx_bytes + bytes(ast.literal_eval(self.protocol_handle.get_response_value('uart_get_from_read_buffer:received_bytes')))
                rx_length = rx_length + int(self.protocol_handle.get_response_value('uart_get_from_read_buffer:length'))

        return (self.transfer_status(status),bytes(rx_bytes))
    
    def transfer_till_count(self, tx_data, rx_count: int = 0, timeout_us: int = 1000000):

        _data: bytes = b''

        if(type(tx_data) == type('0')):
            _data = tx_data.encode('utf-8')
        
        if(type(tx_data) == type(b'\x01')):
            _data = tx_data

        if(type(tx_data) == type([])):
            _data = bytes(tx_data)

        payload = self.__bytes_to_list(_data)

        tx_length = len(tx_data)

        if len(tx_data) < TRANSFER_CHUNK_SIZE:

            self.protocol_handle.set_request_value('uart_put_into_write_buffer:reset_buffer', int(True))
            self.protocol_handle.set_request_value('uart_put_into_write_buffer:start_index', 0)
            self.protocol_handle.set_request_value('uart_put_into_write_buffer:length', len(tx_data))
            self.protocol_handle.set_request_value('uart_put_into_write_buffer:payload', str(payload))
            self.protocol_handle.send_command('uart_put_into_write_buffer')


        elif( ( len(tx_data) % TRANSFER_CHUNK_SIZE ) == 0 ):

            number_of_chunks = len(tx_data) // TRANSFER_CHUNK_SIZE

            for chunk in range(0, number_of_chunks):
                self.protocol_handle.set_request_value('uart_put_into_write_buffer:reset_buffer', int(chunk == 0))
                self.protocol_handle.set_request_value('uart_put_into_write_buffer:start_index', TRANSFER_CHUNK_SIZE*chunk)
                self.protocol_handle.set_request_value('uart_put_into_write_buffer:length', TRANSFER_CHUNK_SIZE)
                self.protocol_handle.set_request_value('uart_put_into_write_buffer:payload', str(payload[TRANSFER_CHUNK_SIZE*chunk:TRANSFER_CHUNK_SIZE*(chunk+1)]))
                self.protocol_handle.send_command('uart_put_into_write_buffer')

        else:            
            number_of_chunks = int((tx_length + TRANSFER_CHUNK_SIZE - 1)// TRANSFER_CHUNK_SIZE)

            for chunk in range(0, number_of_chunks):
                is_last_chunk = chunk == (number_of_chunks - 1)
                self.protocol_handle.set_request_value('uart_put_into_write_buffer:reset_buffer', int(chunk == 0))
                self.protocol_handle.set_request_value('uart_put_into_write_buffer:start_index', TRANSFER_CHUNK_SIZE*chunk)
                self.protocol_handle.set_request_value('uart_put_into_write_buffer:length', TRANSFER_CHUNK_SIZE if not is_last_chunk else (tx_length % TRANSFER_CHUNK_SIZE) )
                self.protocol_handle.set_request_value('uart_put_into_write_buffer:payload', str( payload[(TRANSFER_CHUNK_SIZE*chunk): (TRANSFER_CHUNK_SIZE*(chunk+1))] if not is_last_chunk else payload[(TRANSFER_CHUNK_SIZE*chunk): (TRANSFER_CHUNK_SIZE*(chunk)) + (tx_length % TRANSFER_CHUNK_SIZE)] ))
                self.protocol_handle.send_command('uart_put_into_write_buffer')

        self.protocol_handle.set_request_value('uart_single_transfer:count_limit_mode', int(True))
        self.protocol_handle.set_request_value('uart_single_transfer:termination_character_mode', int(False))
        self.protocol_handle.set_request_value('uart_single_transfer:count_limit', rx_count)
        self.protocol_handle.set_request_value('uart_single_transfer:termination_character', 0)
        self.protocol_handle.set_request_value('uart_single_transfer:timeout_us', timeout_us)

        self.protocol_handle.send_command('uart_single_transfer')

        status = int(self.protocol_handle.get_response_value('uart_single_transfer:status'))
        length = int(self.protocol_handle.get_response_value('uart_single_transfer:length'))

        rx_bytes = b''
        rx_length: int = 0

        if length < TRANSFER_CHUNK_SIZE:

            self.protocol_handle.set_request_value('uart_get_from_read_buffer:start_index', 0)
            self.protocol_handle.set_request_value('uart_get_from_read_buffer:length', length)

            self.protocol_handle.send_command('uart_get_from_read_buffer')

            rx_bytes = bytes(ast.literal_eval(self.protocol_handle.get_response_value('uart_get_from_read_buffer:received_bytes')))
            rx_length = rx_length + int(self.protocol_handle.get_response_value('uart_get_from_read_buffer:length'))

        elif (length % TRANSFER_CHUNK_SIZE != 0):

            number_of_chunks = ( length + TRANSFER_CHUNK_SIZE - 1 )// TRANSFER_CHUNK_SIZE
            is_last_chunk = False

            for chunk in range(0, number_of_chunks):

                if(chunk == (number_of_chunks - 1)):
                    is_last_chunk = True

                self.protocol_handle.set_request_value('uart_get_from_read_buffer:start_index', TRANSFER_CHUNK_SIZE * chunk)

                if(is_last_chunk):
                    self.protocol_handle.set_request_value('uart_get_from_read_buffer:length', (length % TRANSFER_CHUNK_SIZE))
                else:
                    self.protocol_handle.set_request_value('uart_get_from_read_buffer:length', TRANSFER_CHUNK_SIZE)

                self.protocol_handle.send_command('uart_get_from_read_buffer')

                rx_bytes = rx_bytes + bytes(ast.literal_eval(self.protocol_handle.get_response_value('uart_get_from_read_buffer:received_bytes')))
                rx_length = rx_length + int(self.protocol_handle.get_response_value('uart_get_from_read_buffer:length'))

        elif (length % TRANSFER_CHUNK_SIZE == 0):

            number_of_chunks = length // TRANSFER_CHUNK_SIZE

            for chunk in range(0, number_of_chunks):

                self.protocol_handle.set_request_value('uart_get_from_read_buffer:start_index', TRANSFER_CHUNK_SIZE * chunk)

                self.protocol_handle.set_request_value('uart_get_from_read_buffer:length', TRANSFER_CHUNK_SIZE)
                    
                self.protocol_handle.send_command('uart_get_from_read_buffer')

                rx_bytes = rx_bytes + bytes(ast.literal_eval(self.protocol_handle.get_response_value('uart_get_from_read_buffer:received_bytes')))
                rx_length = rx_length + int(self.protocol_handle.get_response_value('uart_get_from_read_buffer:length'))

        return (self.transfer_status(status),bytes(rx_bytes))

    def transfer_till_char(self, tx_data, rx_char, timeout_us: int = 1000000):

        _data: bytes = b''

        if(type(tx_data) == type('0')):
            _data = tx_data.encode('utf-8')
        
        if(type(tx_data) == type(b'\x01')):
            _data = tx_data

        if(type(tx_data) == type([])):
            _data = bytes(tx_data)

        payload = self.__bytes_to_list(_data)

        tx_length = len(tx_data)

        if len(tx_data) < TRANSFER_CHUNK_SIZE:

            self.protocol_handle.set_request_value('uart_put_into_write_buffer:reset_buffer', int(True))
            self.protocol_handle.set_request_value('uart_put_into_write_buffer:start_index', 0)
            self.protocol_handle.set_request_value('uart_put_into_write_buffer:length', len(tx_data))
            self.protocol_handle.set_request_value('uart_put_into_write_buffer:payload', str(payload))
            self.protocol_handle.send_command('uart_put_into_write_buffer')

        elif( ( len(tx_data) % TRANSFER_CHUNK_SIZE ) == 0 ):

            number_of_chunks = len(tx_data) // TRANSFER_CHUNK_SIZE

            for chunk in range(0, number_of_chunks):
                self.protocol_handle.set_request_value('uart_put_into_write_buffer:reset_buffer', int(chunk == 0))
                self.protocol_handle.set_request_value('uart_put_into_write_buffer:start_index', (TRANSFER_CHUNK_SIZE*chunk))
                self.protocol_handle.set_request_value('uart_put_into_write_buffer:length', TRANSFER_CHUNK_SIZE)
                self.protocol_handle.set_request_value('uart_put_into_write_buffer:payload', str(payload[TRANSFER_CHUNK_SIZE*chunk:TRANSFER_CHUNK_SIZE*(chunk+1)]))
                self.protocol_handle.send_command('uart_put_into_write_buffer')

        else:            
            number_of_chunks = int((tx_length + TRANSFER_CHUNK_SIZE - 1)// TRANSFER_CHUNK_SIZE)

            for chunk in range(0, number_of_chunks):
                is_last_chunk = chunk == (number_of_chunks - 1)
                self.protocol_handle.set_request_value('uart_put_into_write_buffer:reset_buffer', int(chunk == 0))
                self.protocol_handle.set_request_value('uart_put_into_write_buffer:start_index', TRANSFER_CHUNK_SIZE*chunk)
                self.protocol_handle.set_request_value('uart_put_into_write_buffer:length', TRANSFER_CHUNK_SIZE if not is_last_chunk else (tx_length % TRANSFER_CHUNK_SIZE) )
                self.protocol_handle.set_request_value('uart_put_into_write_buffer:payload', str( payload[(TRANSFER_CHUNK_SIZE*chunk): (TRANSFER_CHUNK_SIZE*(chunk+1))] if not is_last_chunk else payload[(TRANSFER_CHUNK_SIZE*chunk): (TRANSFER_CHUNK_SIZE*(chunk)) + (tx_length % TRANSFER_CHUNK_SIZE)] ))
                self.protocol_handle.send_command('uart_put_into_write_buffer')

        self.protocol_handle.set_request_value('uart_single_transfer:count_limit_mode', int(False))
        self.protocol_handle.set_request_value('uart_single_transfer:termination_character_mode', int(True))
        self.protocol_handle.set_request_value('uart_single_transfer:count_limit', 0)
        self.protocol_handle.set_request_value('uart_single_transfer:termination_character', int(rx_char[0]))
        self.protocol_handle.set_request_value('uart_single_transfer:timeout_us', timeout_us)

        self.protocol_handle.send_command('uart_single_transfer')

        status = int(self.protocol_handle.get_response_value('uart_single_transfer:status'))
        length = int(self.protocol_handle.get_response_value('uart_single_transfer:length'))

        rx_bytes = b''
        rx_length: int = 0

        if length < TRANSFER_CHUNK_SIZE:

            self.protocol_handle.set_request_value('uart_get_from_read_buffer:start_index', 0)
            self.protocol_handle.set_request_value('uart_get_from_read_buffer:length', length)

            self.protocol_handle.send_command('uart_get_from_read_buffer')

            rx_bytes = bytes(ast.literal_eval(self.protocol_handle.get_response_value('uart_get_from_read_buffer:received_bytes')))
            rx_length = rx_length + int(self.protocol_handle.get_response_value('uart_get_from_read_buffer:length'))

        elif (length % TRANSFER_CHUNK_SIZE != 0):

            number_of_chunks = ( length + TRANSFER_CHUNK_SIZE - 1 )// TRANSFER_CHUNK_SIZE
            is_last_chunk = False

            for chunk in range(0, number_of_chunks):

                if(chunk == (number_of_chunks - 1)):
                    is_last_chunk = True

                self.protocol_handle.set_request_value('uart_get_from_read_buffer:start_index', TRANSFER_CHUNK_SIZE * chunk)

                if(is_last_chunk):
                    self.protocol_handle.set_request_value('uart_get_from_read_buffer:length', (length % TRANSFER_CHUNK_SIZE))
                else:
                    self.protocol_handle.set_request_value('uart_get_from_read_buffer:length', TRANSFER_CHUNK_SIZE)

                self.protocol_handle.send_command('uart_get_from_read_buffer')

                rx_bytes = rx_bytes + bytes(ast.literal_eval(self.protocol_handle.get_response_value('uart_get_from_read_buffer:received_bytes')))
                rx_length = rx_length + int(self.protocol_handle.get_response_value('uart_get_from_read_buffer:length'))

        elif (length % TRANSFER_CHUNK_SIZE == 0):

            number_of_chunks = length // TRANSFER_CHUNK_SIZE

            for chunk in range(0, number_of_chunks):

                self.protocol_handle.set_request_value('uart_get_from_read_buffer:start_index', TRANSFER_CHUNK_SIZE * chunk)

                self.protocol_handle.set_request_value('uart_get_from_read_buffer:length', TRANSFER_CHUNK_SIZE)
                    
                self.protocol_handle.send_command('uart_get_from_read_buffer')

                rx_bytes = rx_bytes + bytes(ast.literal_eval(self.protocol_handle.get_response_value('uart_get_from_read_buffer:received_bytes')))
                rx_length = rx_length + int(self.protocol_handle.get_response_value('uart_get_from_read_buffer:length'))

        return (self.transfer_status(status),bytes(rx_bytes))
    
    def consecutive_transfer_till_count(self, transfers: list["uart.consecutive_transfer_instance"]):

        number_of_transfers = len(transfers)

        self.protocol_handle.set_request_value('uart_consecutive_transfers_configure:number_of_transfers', number_of_transfers)
        self.protocol_handle.set_request_value('uart_consecutive_transfers_configure:character_termination_mode', 0)
        self.protocol_handle.set_request_value('uart_consecutive_transfers_configure:termination_character', 0)

        self.protocol_handle.send_command('uart_consecutive_transfers_configure')

        transfer_index = 0

        for transfer in transfers:

            self.protocol_handle.set_request_value('uart_configure_consecutive_transfer_instance:consecutive_transfer_index', transfer_index)
            self.protocol_handle.set_request_value('uart_configure_consecutive_transfer_instance:timeout_us', transfer.timeout_us)
            self.protocol_handle.set_request_value('uart_configure_consecutive_transfer_instance:pause_us', transfer.pause_time_us)
            self.protocol_handle.set_request_value('uart_configure_consecutive_transfer_instance:write_count', len(transfer.tx_data))
            self.protocol_handle.set_request_value('uart_configure_consecutive_transfer_instance:target_read_count', transfer.target_rx_count)
            
            self.protocol_handle.send_command('uart_configure_consecutive_transfer_instance')

            tx_length = len(transfer.tx_data)

            if(tx_length < TRANSFER_CHUNK_SIZE):
                self.protocol_handle.set_request_value('uart_consecutive_transfer_instance_write_bytes:transfer_instance', transfer_index)
                self.protocol_handle.set_request_value('uart_consecutive_transfer_instance_write_bytes:start_index', 0)
                self.protocol_handle.set_request_value('uart_consecutive_transfer_instance_write_bytes:length', tx_length)
                self.protocol_handle.set_request_value('uart_consecutive_transfer_instance_write_bytes:payload', str(list(transfer.tx_data)) )
                
                self.protocol_handle.send_command('uart_consecutive_transfer_instance_write_bytes')
            elif( ( tx_length % TRANSFER_CHUNK_SIZE ) == 0 ):

                number_of_chunks = tx_length // TRANSFER_CHUNK_SIZE

                for chunk in range(0, number_of_chunks):

                    self.protocol_handle.set_request_value('uart_consecutive_transfer_instance_write_bytes:transfer_instance', transfer_index)
                    self.protocol_handle.set_request_value('uart_consecutive_transfer_instance_write_bytes:start_index', (TRANSFER_CHUNK_SIZE*chunk))
                    self.protocol_handle.set_request_value('uart_consecutive_transfer_instance_write_bytes:length', TRANSFER_CHUNK_SIZE)
                    self.protocol_handle.set_request_value('uart_consecutive_transfer_instance_write_bytes:payload', str(list(transfer.tx_data)[TRANSFER_CHUNK_SIZE*chunk : TRANSFER_CHUNK_SIZE*(chunk+1)]) )

                    self.protocol_handle.send_command('uart_consecutive_transfer_instance_write_bytes')

            else:            
                number_of_chunks = int((tx_length + TRANSFER_CHUNK_SIZE - 1)// TRANSFER_CHUNK_SIZE)

                for chunk in range(0, number_of_chunks):
                    is_last_chunk = chunk == (number_of_chunks - 1)

                    self.protocol_handle.set_request_value('uart_consecutive_transfer_instance_write_bytes:transfer_instance', transfer_index)
                    self.protocol_handle.set_request_value('uart_consecutive_transfer_instance_write_bytes:start_index', (TRANSFER_CHUNK_SIZE*chunk))
                    self.protocol_handle.set_request_value('uart_consecutive_transfer_instance_write_bytes:length', TRANSFER_CHUNK_SIZE if not is_last_chunk else (tx_length % TRANSFER_CHUNK_SIZE))
                    self.protocol_handle.set_request_value('uart_consecutive_transfer_instance_write_bytes:payload', str( list(transfer.tx_data)[(TRANSFER_CHUNK_SIZE*chunk): (TRANSFER_CHUNK_SIZE*(chunk+1))] if not is_last_chunk else list(transfer.tx_data)[(TRANSFER_CHUNK_SIZE*chunk): (TRANSFER_CHUNK_SIZE*(chunk)) + (tx_length % TRANSFER_CHUNK_SIZE)] ) )

                    self.protocol_handle.send_command('uart_consecutive_transfer_instance_write_bytes')

            transfer_index = transfer_index + 1

        self.protocol_handle.set_request_value('uart_consecutive_transfer_start:start', 1)
        self.protocol_handle.send_command('uart_consecutive_transfer_start')

        transfer_index = 0

        for transfer in transfers:

            self.protocol_handle.set_request_value('uart_consecutive_transfer_instance_get_read_count_and_status:transfer_instance', transfer_index)

            self.protocol_handle.send_command('uart_consecutive_transfer_instance_get_read_count_and_status')

            transfer.rx_count = int(self.protocol_handle.get_response_value('uart_consecutive_transfer_instance_get_read_count_and_status:rx_count'))
            transfer.rx_status = uart.transfer_status( int(self.protocol_handle.get_response_value('uart_consecutive_transfer_instance_get_read_count_and_status:status')) )

            transfer.rx_data = b''

            if transfer.rx_count < TRANSFER_CHUNK_SIZE:
                self.protocol_handle.set_request_value('uart_consecutive_transfer_instance_read_bytes:transfer_instance', transfer_index)
                self.protocol_handle.set_request_value('uart_consecutive_transfer_instance_read_bytes:start_index', 0)
                self.protocol_handle.set_request_value('uart_consecutive_transfer_instance_read_bytes:length', transfer.rx_count)

                self.protocol_handle.send_command('uart_consecutive_transfer_instance_read_bytes')

                transfer.rx_data = bytes(ast.literal_eval(self.protocol_handle.get_response_value('uart_consecutive_transfer_instance_read_bytes:payload')))

            elif( ( transfer.rx_count % TRANSFER_CHUNK_SIZE ) == 0 ):

                number_of_chunks = transfer.rx_count // TRANSFER_CHUNK_SIZE

                for chunk in range(0, number_of_chunks):
                    self.protocol_handle.set_request_value('uart_consecutive_transfer_instance_read_bytes:transfer_instance', transfer_index)
                    self.protocol_handle.set_request_value('uart_consecutive_transfer_instance_read_bytes:start_index', (chunk * TRANSFER_CHUNK_SIZE))
                    self.protocol_handle.set_request_value('uart_consecutive_transfer_instance_read_bytes:length', TRANSFER_CHUNK_SIZE)

                    self.protocol_handle.send_command('uart_consecutive_transfer_instance_read_bytes')

                    transfer.rx_data = transfer.rx_data + bytes(ast.literal_eval(self.protocol_handle.get_response_value('uart_consecutive_transfer_instance_read_bytes:payload')))
            elif( ( transfer.rx_count % TRANSFER_CHUNK_SIZE ) != 0 ):
                
                number_of_chunks = ( transfer.rx_count + TRANSFER_CHUNK_SIZE - 1 )// TRANSFER_CHUNK_SIZE
                is_last_chunk = False

                for chunk in range(0, number_of_chunks):
                
                    is_last_chunk = chunk == (number_of_chunks - 1)

                    self.protocol_handle.set_request_value('uart_consecutive_transfer_instance_read_bytes:transfer_instance', transfer_index)
                    self.protocol_handle.set_request_value('uart_consecutive_transfer_instance_read_bytes:start_index', (chunk * TRANSFER_CHUNK_SIZE))
                    self.protocol_handle.set_request_value('uart_consecutive_transfer_instance_read_bytes:length', TRANSFER_CHUNK_SIZE if not is_last_chunk else (transfer.rx_count % TRANSFER_CHUNK_SIZE))

                    self.protocol_handle.send_command('uart_consecutive_transfer_instance_read_bytes')

                    transfer.rx_data = transfer.rx_data + bytes(ast.literal_eval(self.protocol_handle.get_response_value('uart_consecutive_transfer_instance_read_bytes:payload')))

            transfer_index = transfer_index + 1
        
        return transfers

    def consecutive_transfer_till_char(self, transfers: list["uart.consecutive_transfer_instance"], termination_char):

        number_of_transfers = len(transfers)

        self.protocol_handle.set_request_value('uart_consecutive_transfers_configure:number_of_transfers', number_of_transfers)
        self.protocol_handle.set_request_value('uart_consecutive_transfers_configure:character_termination_mode', 1)
        self.protocol_handle.set_request_value('uart_consecutive_transfers_configure:termination_character', bytes(termination_char)[0])

        self.protocol_handle.send_command('uart_consecutive_transfers_configure')

        transfer_index = 0

        for transfer in transfers:

            self.protocol_handle.set_request_value('uart_configure_consecutive_transfer_instance:consecutive_transfer_index', transfer_index)
            self.protocol_handle.set_request_value('uart_configure_consecutive_transfer_instance:timeout_us', transfer.timeout_us)
            self.protocol_handle.set_request_value('uart_configure_consecutive_transfer_instance:pause_us', transfer.pause_time_us)
            self.protocol_handle.set_request_value('uart_configure_consecutive_transfer_instance:write_count', len(transfer.tx_data))
            self.protocol_handle.set_request_value('uart_configure_consecutive_transfer_instance:target_read_count', 0)
            
            self.protocol_handle.send_command('uart_configure_consecutive_transfer_instance')

            tx_length = len(transfer.tx_data)

            if(tx_length < TRANSFER_CHUNK_SIZE):
                self.protocol_handle.set_request_value('uart_consecutive_transfer_instance_write_bytes:transfer_instance', transfer_index)
                self.protocol_handle.set_request_value('uart_consecutive_transfer_instance_write_bytes:start_index', 0)
                self.protocol_handle.set_request_value('uart_consecutive_transfer_instance_write_bytes:length', tx_length)
                self.protocol_handle.set_request_value('uart_consecutive_transfer_instance_write_bytes:payload', str(list(transfer.tx_data)) )
                
                self.protocol_handle.send_command('uart_consecutive_transfer_instance_write_bytes')
            elif( ( tx_length % TRANSFER_CHUNK_SIZE ) == 0 ):

                number_of_chunks = tx_length // TRANSFER_CHUNK_SIZE

                for chunk in range(0, number_of_chunks):

                    self.protocol_handle.set_request_value('uart_consecutive_transfer_instance_write_bytes:transfer_instance', transfer_index)
                    self.protocol_handle.set_request_value('uart_consecutive_transfer_instance_write_bytes:start_index', (TRANSFER_CHUNK_SIZE*chunk))
                    self.protocol_handle.set_request_value('uart_consecutive_transfer_instance_write_bytes:length', TRANSFER_CHUNK_SIZE)
                    self.protocol_handle.set_request_value('uart_consecutive_transfer_instance_write_bytes:payload', str(list(transfer.tx_data)[TRANSFER_CHUNK_SIZE*chunk : TRANSFER_CHUNK_SIZE*(chunk+1)]) )

                    self.protocol_handle.send_command('uart_consecutive_transfer_instance_write_bytes')

            else:            
                number_of_chunks = int((tx_length + TRANSFER_CHUNK_SIZE - 1)// TRANSFER_CHUNK_SIZE)

                for chunk in range(0, number_of_chunks):
                    is_last_chunk = chunk == (number_of_chunks - 1)

                    self.protocol_handle.set_request_value('uart_consecutive_transfer_instance_write_bytes:transfer_instance', transfer_index)
                    self.protocol_handle.set_request_value('uart_consecutive_transfer_instance_write_bytes:start_index', (TRANSFER_CHUNK_SIZE*chunk))
                    self.protocol_handle.set_request_value('uart_consecutive_transfer_instance_write_bytes:length', TRANSFER_CHUNK_SIZE if not is_last_chunk else (tx_length % TRANSFER_CHUNK_SIZE))
                    self.protocol_handle.set_request_value('uart_consecutive_transfer_instance_write_bytes:payload', str( list(transfer.tx_data)[(TRANSFER_CHUNK_SIZE*chunk): (TRANSFER_CHUNK_SIZE*(chunk+1))] if not is_last_chunk else list(transfer.tx_data)[(TRANSFER_CHUNK_SIZE*chunk): (TRANSFER_CHUNK_SIZE*(chunk)) + (tx_length % TRANSFER_CHUNK_SIZE)] ) )

                    self.protocol_handle.send_command('uart_consecutive_transfer_instance_write_bytes')

            transfer_index = transfer_index + 1

        self.protocol_handle.set_request_value('uart_consecutive_transfer_start:start', 1)
        self.protocol_handle.send_command('uart_consecutive_transfer_start')

        transfer_index = 0

        for transfer in transfers:

            self.protocol_handle.set_request_value('uart_consecutive_transfer_instance_get_read_count_and_status:transfer_instance', transfer_index)

            self.protocol_handle.send_command('uart_consecutive_transfer_instance_get_read_count_and_status')

            transfer.rx_count = int(self.protocol_handle.get_response_value('uart_consecutive_transfer_instance_get_read_count_and_status:rx_count'))
            transfer.rx_status = uart.transfer_status( int(self.protocol_handle.get_response_value('uart_consecutive_transfer_instance_get_read_count_and_status:status')) )

            transfer.rx_data = b''

            if transfer.rx_count < TRANSFER_CHUNK_SIZE:
                self.protocol_handle.set_request_value('uart_consecutive_transfer_instance_read_bytes:transfer_instance', transfer_index)
                self.protocol_handle.set_request_value('uart_consecutive_transfer_instance_read_bytes:start_index', 0)
                self.protocol_handle.set_request_value('uart_consecutive_transfer_instance_read_bytes:length', transfer.rx_count)

                self.protocol_handle.send_command('uart_consecutive_transfer_instance_read_bytes')

                transfer.rx_data = bytes(ast.literal_eval(self.protocol_handle.get_response_value('uart_consecutive_transfer_instance_read_bytes:payload')))

            elif( ( transfer.rx_count % TRANSFER_CHUNK_SIZE ) == 0 ):

                number_of_chunks = transfer.rx_count // TRANSFER_CHUNK_SIZE

                for chunk in range(0, number_of_chunks):
                    self.protocol_handle.set_request_value('uart_consecutive_transfer_instance_read_bytes:transfer_instance', transfer_index)
                    self.protocol_handle.set_request_value('uart_consecutive_transfer_instance_read_bytes:start_index', (chunk * TRANSFER_CHUNK_SIZE))
                    self.protocol_handle.set_request_value('uart_consecutive_transfer_instance_read_bytes:length', TRANSFER_CHUNK_SIZE)

                    self.protocol_handle.send_command('uart_consecutive_transfer_instance_read_bytes')

                    transfer.rx_data = transfer.rx_data + bytes(ast.literal_eval(self.protocol_handle.get_response_value('uart_consecutive_transfer_instance_read_bytes:payload')))
            elif( ( transfer.rx_count % TRANSFER_CHUNK_SIZE ) != 0 ):
                
                number_of_chunks = ( transfer.rx_count + TRANSFER_CHUNK_SIZE - 1 )// TRANSFER_CHUNK_SIZE
                is_last_chunk = False

                for chunk in range(0, number_of_chunks):
                
                    is_last_chunk = chunk == (number_of_chunks - 1)

                    self.protocol_handle.set_request_value('uart_consecutive_transfer_instance_read_bytes:transfer_instance', transfer_index)
                    self.protocol_handle.set_request_value('uart_consecutive_transfer_instance_read_bytes:start_index', (chunk * TRANSFER_CHUNK_SIZE))
                    self.protocol_handle.set_request_value('uart_consecutive_transfer_instance_read_bytes:length', TRANSFER_CHUNK_SIZE if not is_last_chunk else (transfer.rx_count % TRANSFER_CHUNK_SIZE))

                    self.protocol_handle.send_command('uart_consecutive_transfer_instance_read_bytes')

                    transfer.rx_data = transfer.rx_data + bytes(ast.literal_eval(self.protocol_handle.get_response_value('uart_consecutive_transfer_instance_read_bytes:payload')))

            transfer_index = transfer_index + 1
        
        return transfers