from enum import Enum
from . import protocol
import ast

MAX_TRANSFER_LENGTH = 1024
TRANSFER_CHUNK_SIZE = 200
MAX_CONSECUTIVE_TRANSFER_COUNT = 32

class i2c:

    class mode(Enum):
        I2C_MODE_STANDARD = 0
        I2C_MODE_FAST = 1

    class transfer_status(Enum):
        I2C_TRANSFER_STATUS_OK = 0
        I2C_TRANSFER_STATUS_TIMEOUT = 1
        I2C_TRANSFER_STATUS_NACK = 2

    class consecutive_transfer_instance:
        def __init__(self, write_data, read_size: int, pause_time_us:int = 1000):
            self.write_data = write_data
            self.read_data = b''
            self.read_size = read_size
            self.pause_time_us = pause_time_us
            self.status = i2c.transfer_status.I2C_TRANSFER_STATUS_OK

    def __init__(self, protocol_handle: protocol.protocol):
        self.protocol_handle = protocol_handle

    def __bytes_to_list(self,data: bytes):

        _data = []

        for byte in data:
            _data.append(int(byte))
        
        return _data

    def configure(self, mode: i2c.mode, address: int, timeout_us: int = 1000000):

        if( address > 127 ):
            raise ValueError("I2C address cannot be greater than 127")

        self.protocol_handle.set_request_value('i2c_configure:mode', mode.value)
        self.protocol_handle.set_request_value('i2c_configure:address', address)
        self.protocol_handle.set_request_value('i2c_configure:timeout_us', timeout_us)

        self.protocol_handle.send_command('i2c_configure')

    def transfer(self, write_bytes, read_count: int):

        _write_bytes: bytes = b''

        if(type(write_bytes) == type('0')):
            _write_bytes = write_bytes.encode('utf-8')
        
        if(type(write_bytes) == type(b'\x01')):
            _write_bytes = write_bytes

        if(type(write_bytes) == type([])):
            _write_bytes = bytes(write_bytes)

        payload = self.__bytes_to_list(_write_bytes)

        length = len(payload)

        self.protocol_handle.set_request_value('i2c_configure_single_transfer:write_count', len(write_bytes))
        self.protocol_handle.set_request_value('i2c_configure_single_transfer:read_count', read_count)

        self.protocol_handle.send_command('i2c_configure_single_transfer')

        if(length < TRANSFER_CHUNK_SIZE):
            self.protocol_handle.set_request_value('i2c_write_buffer_put:start_index', 0)
            self.protocol_handle.set_request_value('i2c_write_buffer_put:length', length)
            self.protocol_handle.set_request_value('i2c_write_buffer_put:payload', str(payload))

            self.protocol_handle.send_command('i2c_write_buffer_put')

        elif( (length % TRANSFER_CHUNK_SIZE) == 0 ):

            number_of_chunks = int(length / TRANSFER_CHUNK_SIZE)

            for chunk_number in range(0, number_of_chunks):
                self.protocol_handle.set_request_value('i2c_write_buffer_put:start_index', chunk_number * TRANSFER_CHUNK_SIZE)
                self.protocol_handle.set_request_value('i2c_write_buffer_put:length', TRANSFER_CHUNK_SIZE)
                self.protocol_handle.set_request_value('i2c_write_buffer_put:payload', str(payload[chunk_number*TRANSFER_CHUNK_SIZE:(chunk_number+1)*TRANSFER_CHUNK_SIZE]))

                self.protocol_handle.send_command('i2c_write_buffer_put')

        elif( (length % TRANSFER_CHUNK_SIZE) != 0 ):

            number_of_chunks = (length + TRANSFER_CHUNK_SIZE - 1) // TRANSFER_CHUNK_SIZE

            for chunk_number in range(0, number_of_chunks):
                is_last_chunk = chunk_number == (number_of_chunks - 1)

                self.protocol_handle.set_request_value('i2c_write_buffer_put:start_index', chunk_number * TRANSFER_CHUNK_SIZE)
                self.protocol_handle.set_request_value('i2c_write_buffer_put:length', TRANSFER_CHUNK_SIZE if not is_last_chunk else (length%TRANSFER_CHUNK_SIZE) )
                self.protocol_handle.set_request_value('i2c_write_buffer_put:payload', str(payload[chunk_number*TRANSFER_CHUNK_SIZE:(1+chunk_number)*TRANSFER_CHUNK_SIZE]))

                self.protocol_handle.send_command('i2c_write_buffer_put')

        self.protocol_handle.set_request_value('i2c_single_transfer:start', int(True))
        self.protocol_handle.send_command('i2c_single_transfer')

        transfer_status = i2c.transfer_status(int(self.protocol_handle.get_response_value('i2c_single_transfer:transfer_status')))
        response = b''

        if(read_count < TRANSFER_CHUNK_SIZE):
            self.protocol_handle.set_request_value('i2c_read_buffer_get:start_index', 0)
            self.protocol_handle.set_request_value('i2c_read_buffer_get:length', read_count)

            self.protocol_handle.send_command('i2c_read_buffer_get')

            response = bytes(ast.literal_eval(self.protocol_handle.get_response_value('i2c_read_buffer_get:payload')))
        elif ((read_count % TRANSFER_CHUNK_SIZE) == 0):
            number_of_chunks = read_count // TRANSFER_CHUNK_SIZE

            for chunk_number in range(0, number_of_chunks):
                self.protocol_handle.set_request_value('i2c_read_buffer_get:start_index', TRANSFER_CHUNK_SIZE * chunk_number)
                self.protocol_handle.set_request_value('i2c_read_buffer_get:length', TRANSFER_CHUNK_SIZE)

                self.protocol_handle.send_command('i2c_read_buffer_get')

                response = response + bytes(ast.literal_eval(self.protocol_handle.get_response_value('i2c_read_buffer_get:payload')))
        elif ((read_count % TRANSFER_CHUNK_SIZE) != 0):
            number_of_chunks = (read_count + TRANSFER_CHUNK_SIZE - 1) // TRANSFER_CHUNK_SIZE

            for chunk_number in range(0, number_of_chunks):

                is_last_chunk = (chunk_number == (number_of_chunks - 1))

                self.protocol_handle.set_request_value('i2c_read_buffer_get:start_index', TRANSFER_CHUNK_SIZE * chunk_number)
                self.protocol_handle.set_request_value('i2c_read_buffer_get:length', TRANSFER_CHUNK_SIZE if not is_last_chunk else (read_count % TRANSFER_CHUNK_SIZE))

                self.protocol_handle.send_command('i2c_read_buffer_get')

                response = response + bytes(ast.literal_eval(self.protocol_handle.get_response_value('i2c_read_buffer_get:payload')))

        return (response, transfer_status)

    def consecutive_transfer(self, transfers):

        number_of_transfers = len(transfers)

        if(number_of_transfers > MAX_CONSECUTIVE_TRANSFER_COUNT):
            raise ValueError("Maximum number of I2C consecutive transfers is {_value}".format(_value = MAX_CONSECUTIVE_TRANSFER_COUNT))

        index = 0

        for consecutive_transfer in transfers:
            self.protocol_handle.set_request_value('i2c_consecutive_transfer_configure:number_of_transfers', number_of_transfers)
            self.protocol_handle.set_request_value('i2c_consecutive_transfer_configure:transfer_index', index)
            self.protocol_handle.set_request_value('i2c_consecutive_transfer_configure:read_count', consecutive_transfer.read_size)
            self.protocol_handle.set_request_value('i2c_consecutive_transfer_configure:write_count', len(consecutive_transfer.write_data))
            self.protocol_handle.set_request_value('i2c_consecutive_transfer_configure:pause_time_us', consecutive_transfer.pause_time_us)

            self.protocol_handle.send_command('i2c_consecutive_transfer_configure')

            _write_bytes: bytes = b''

            write_bytes = consecutive_transfer.write_data

            if(type(write_bytes) == type('0')):
                _write_bytes = write_bytes.encode('utf-8')
            
            if(type(write_bytes) == type(b'\x01')):
                _write_bytes = write_bytes

            if(type(write_bytes) == type([])):
                _write_bytes = bytes(write_bytes)

            payload = self.__bytes_to_list(_write_bytes)

            payload_length = len(consecutive_transfer.write_data)

            if payload_length < TRANSFER_CHUNK_SIZE:
                self.protocol_handle.set_request_value('i2c_consecutive_transfer_write_buffer_put:transfer_index', index)
                self.protocol_handle.set_request_value('i2c_consecutive_transfer_write_buffer_put:start_index', 0)
                self.protocol_handle.set_request_value('i2c_consecutive_transfer_write_buffer_put:length', payload_length)
                self.protocol_handle.set_request_value('i2c_consecutive_transfer_write_buffer_put:payload', payload)

                self.protocol_handle.send_command('i2c_consecutive_transfer_write_buffer_put')

            elif( payload_length % TRANSFER_CHUNK_SIZE == 0 ):
                number_of_chunks = payload_length // TRANSFER_CHUNK_SIZE

                for chunk_number in range(0, number_of_chunks):
                    self.protocol_handle.set_request_value('i2c_consecutive_transfer_write_buffer_put:transfer_index', index)
                    self.protocol_handle.set_request_value('i2c_consecutive_transfer_write_buffer_put:start_index', chunk_number * TRANSFER_CHUNK_SIZE)
                    self.protocol_handle.set_request_value('i2c_consecutive_transfer_write_buffer_put:length', TRANSFER_CHUNK_SIZE)
                    self.protocol_handle.set_request_value('i2c_consecutive_transfer_write_buffer_put:payload', payload[chunk_number*TRANSFER_CHUNK_SIZE: (chunk_number+1)*TRANSFER_CHUNK_SIZE])

                    self.protocol_handle.send_command('i2c_consecutive_transfer_write_buffer_put')

            elif( payload_length % TRANSFER_CHUNK_SIZE != 0 ):
                number_of_chunks = (TRANSFER_CHUNK_SIZE + payload_length - 1)// TRANSFER_CHUNK_SIZE

                for chunk_number in range(0, number_of_chunks):
                    is_last_chunk = (chunk_number == (number_of_chunks - 1))
                    self.protocol_handle.set_request_value('i2c_consecutive_transfer_write_buffer_put:transfer_index', index)
                    self.protocol_handle.set_request_value('i2c_consecutive_transfer_write_buffer_put:start_index', chunk_number * TRANSFER_CHUNK_SIZE)
                    self.protocol_handle.set_request_value('i2c_consecutive_transfer_write_buffer_put:length', TRANSFER_CHUNK_SIZE if not is_last_chunk else (payload_length % TRANSFER_CHUNK_SIZE) )
                    self.protocol_handle.set_request_value('i2c_consecutive_transfer_write_buffer_put:payload', payload[chunk_number*TRANSFER_CHUNK_SIZE: (chunk_number+1)*TRANSFER_CHUNK_SIZE])

                    self.protocol_handle.send_command('i2c_consecutive_transfer_write_buffer_put')              

            index = index + 1

        self.protocol_handle.set_request_value('i2c_consecutive_transfer_start:number_of_transfers', number_of_transfers)
        self.protocol_handle.send_command('i2c_consecutive_transfer_start')

        transfer_status = ast.literal_eval(self.protocol_handle.get_response_value('i2c_consecutive_transfer_start:transfer_status'))

        index = 0

        for consecutive_transfer in transfers:

            consecutive_transfer.read_data = b''

            length = consecutive_transfer.read_size

            if(length < TRANSFER_CHUNK_SIZE):
                self.protocol_handle.set_request_value('i2c_consecutive_transfer_read_buffer_get:transfer_index', index)
                self.protocol_handle.set_request_value('i2c_consecutive_transfer_read_buffer_get:start_index', 0)
                self.protocol_handle.set_request_value('i2c_consecutive_transfer_read_buffer_get:length', length)

                self.protocol_handle.send_command('i2c_consecutive_transfer_read_buffer_get')

                consecutive_transfer.read_data = consecutive_transfer.read_data + bytes(ast.literal_eval(self.protocol_handle.get_response_value('i2c_consecutive_transfer_read_buffer_get:payload')))
            elif( length % TRANSFER_CHUNK_SIZE == 0 ):
                number_of_chunks = length // TRANSFER_CHUNK_SIZE

                for chunk_number in range(0, number_of_chunks):
                    self.protocol_handle.set_request_value('i2c_consecutive_transfer_read_buffer_get:transfer_index', index)
                    self.protocol_handle.set_request_value('i2c_consecutive_transfer_read_buffer_get:start_index', TRANSFER_CHUNK_SIZE * chunk_number)
                    self.protocol_handle.set_request_value('i2c_consecutive_transfer_read_buffer_get:length', TRANSFER_CHUNK_SIZE)

                    self.protocol_handle.send_command('i2c_consecutive_transfer_read_buffer_get')

                    consecutive_transfer.read_data = consecutive_transfer.read_data + bytes(ast.literal_eval(self.protocol_handle.get_response_value('i2c_consecutive_transfer_read_buffer_get:payload')))
            elif( length % TRANSFER_CHUNK_SIZE != 0 ):
                number_of_chunks = ( TRANSFER_CHUNK_SIZE + length - 1)// TRANSFER_CHUNK_SIZE

                for chunk_number in range(0, number_of_chunks):
                    is_last_chunk = (chunk_number == (number_of_chunks - 1))
                    self.protocol_handle.set_request_value('i2c_consecutive_transfer_read_buffer_get:transfer_index', index)
                    self.protocol_handle.set_request_value('i2c_consecutive_transfer_read_buffer_get:start_index', TRANSFER_CHUNK_SIZE * chunk_number)
                    self.protocol_handle.set_request_value('i2c_consecutive_transfer_read_buffer_get:length', TRANSFER_CHUNK_SIZE if not is_last_chunk else (length % TRANSFER_CHUNK_SIZE) )

                    self.protocol_handle.send_command('i2c_consecutive_transfer_read_buffer_get')

                    consecutive_transfer.read_data = consecutive_transfer.read_data + bytes(ast.literal_eval(self.protocol_handle.get_response_value('i2c_consecutive_transfer_read_buffer_get:payload')))

            consecutive_transfer.transfer_status = i2c.transfer_status(transfer_status[index])

            index = index + 1