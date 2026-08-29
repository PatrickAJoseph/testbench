from enum import Enum
from . import protocol
import ast

TRANSFER_CHUNK_SIZE = 200
MAX_CONSECUTIVE_TRANSFER_COUNT = 32

class spi:

    class mode(Enum):
        SPI_CPOL0_CPHA0 = 0
        SPI_CPOL0_CPHA1 = 1
        SPI_CPOL1_CPHA0 = 2
        SPI_CPOL1_CPHA1 = 3

    class bit_order(Enum):
        SPI_BITORDER_MSB_FIRST = 0
        SPI_BITORDER_LSB_FIRST = 1

    class cs_polarity(Enum):
        SPI_CS_POLARITY_ACTIVE_LOW = 0
        SPI_CS_POLARITY_ACTIVE_HIGH = 1

    class consecutive_transfer_instance:
        def __init__(self, write_data,  pause_time_us:int = 1000):
            self.write_data = write_data
            self.read_data = b''
            self.pause_time_us = pause_time_us

    def __init__(self, protocol_handle: protocol.protocol):
        self.protocol_handle = protocol_handle

    def __bytes_to_list(self,data: bytes):

        _data = []

        for byte in data:
            _data.append(int(byte))
        
        return _data

    def configure(self, bitrate:int, mode: spi.mode, bit_order: spi.bit_order, cs_polarity: spi.cs_polarity):

        self.protocol_handle.set_request_value('spi_configure:bitrate', bitrate)
        self.protocol_handle.set_request_value('spi_configure:mode', mode.value)
        self.protocol_handle.set_request_value('spi_configure:bitorder', bit_order.value)
        self.protocol_handle.set_request_value('spi_configure:cs_polarity', cs_polarity.value)

        self.protocol_handle.send_command('spi_configure')

    def transfer(self, tx_bytes):

        _write_bytes: bytes = b''

        write_bytes = tx_bytes

        if(type(write_bytes) == type('0')):
            _write_bytes = write_bytes.encode('utf-8')
        
        if(type(write_bytes) == type(b'\x01')):
            _write_bytes = write_bytes

        if(type(write_bytes) == type([])):
            _write_bytes = bytes(write_bytes)

        payload = self.__bytes_to_list(_write_bytes)

        length = len(payload)

        self.protocol_handle.set_request_value('spi_configure_single_transfer:transfer_count', length)
        self.protocol_handle.send_command('spi_configure_single_transfer')

        if(length < TRANSFER_CHUNK_SIZE):
            self.protocol_handle.set_request_value('spi_write_buffer_put:start_index', 0)
            self.protocol_handle.set_request_value('spi_write_buffer_put:length', length)
            self.protocol_handle.set_request_value('spi_write_buffer_put:payload', str(payload))

            self.protocol_handle.send_command('spi_write_buffer_put')

        elif( (length % TRANSFER_CHUNK_SIZE) == 0 ):

            number_of_chunks = int(length / TRANSFER_CHUNK_SIZE)

            for chunk_number in range(0, number_of_chunks):
                self.protocol_handle.set_request_value('spi_write_buffer_put:start_index', chunk_number * TRANSFER_CHUNK_SIZE)
                self.protocol_handle.set_request_value('spi_write_buffer_put:length', TRANSFER_CHUNK_SIZE)
                self.protocol_handle.set_request_value('spi_write_buffer_put:payload', str(payload[chunk_number*TRANSFER_CHUNK_SIZE:(chunk_number+1)*TRANSFER_CHUNK_SIZE]))

                self.protocol_handle.send_command('spi_write_buffer_put')

        elif( (length % TRANSFER_CHUNK_SIZE) != 0 ):

            number_of_chunks = (length + TRANSFER_CHUNK_SIZE - 1) // TRANSFER_CHUNK_SIZE

            for chunk_number in range(0, number_of_chunks):
                is_last_chunk = chunk_number == (number_of_chunks - 1)

                self.protocol_handle.set_request_value('spi_write_buffer_put:start_index', chunk_number * TRANSFER_CHUNK_SIZE)
                self.protocol_handle.set_request_value('spi_write_buffer_put:length', TRANSFER_CHUNK_SIZE if not is_last_chunk else (length%TRANSFER_CHUNK_SIZE) )
                self.protocol_handle.set_request_value('spi_write_buffer_put:payload', str(payload[chunk_number*TRANSFER_CHUNK_SIZE:(1+chunk_number)*TRANSFER_CHUNK_SIZE]))

                self.protocol_handle.send_command('spi_write_buffer_put')

        self.protocol_handle.set_request_value('spi_single_transfer:start', int(True))
        self.protocol_handle.send_command('spi_single_transfer')

        response = b''

        if(length < TRANSFER_CHUNK_SIZE):
            self.protocol_handle.set_request_value('spi_read_buffer_get:start_index', 0)
            self.protocol_handle.set_request_value('spi_read_buffer_get:length', length)

            self.protocol_handle.send_command('spi_read_buffer_get')

            response = bytes(ast.literal_eval(self.protocol_handle.get_response_value('spi_read_buffer_get:payload')))
        elif ((length % TRANSFER_CHUNK_SIZE) == 0):
            number_of_chunks = length // TRANSFER_CHUNK_SIZE

            for chunk_number in range(0, number_of_chunks):
                self.protocol_handle.set_request_value('spi_read_buffer_get:start_index', TRANSFER_CHUNK_SIZE * chunk_number)
                self.protocol_handle.set_request_value('spi_read_buffer_get:length', TRANSFER_CHUNK_SIZE)

                self.protocol_handle.send_command('spi_read_buffer_get')

                response = response + bytes(ast.literal_eval(self.protocol_handle.get_response_value('spi_read_buffer_get:payload')))
        elif ((length % TRANSFER_CHUNK_SIZE) != 0):
            number_of_chunks = (length + TRANSFER_CHUNK_SIZE - 1) // TRANSFER_CHUNK_SIZE

            for chunk_number in range(0, number_of_chunks):

                is_last_chunk = (chunk_number == (number_of_chunks - 1))

                self.protocol_handle.set_request_value('spi_read_buffer_get:start_index', TRANSFER_CHUNK_SIZE * chunk_number)
                self.protocol_handle.set_request_value('spi_read_buffer_get:length', TRANSFER_CHUNK_SIZE if not is_last_chunk else (read_count % TRANSFER_CHUNK_SIZE))

                self.protocol_handle.send_command('spi_read_buffer_get')

                response = response + bytes(ast.literal_eval(self.protocol_handle.get_response_value('spi_read_buffer_get:payload')))

        return response

    def consecutive_transfer(self, transfers):

        number_of_transfers = len(transfers)

        if(number_of_transfers > MAX_CONSECUTIVE_TRANSFER_COUNT):
            raise ValueError("Maximum number of SPI consecutive transfers is {_value}".format(_value = MAX_CONSECUTIVE_TRANSFER_COUNT))

        index = 0

        for consecutive_transfer in transfers:
            self.protocol_handle.set_request_value('spi_consecutive_transfer_configure:number_of_transfers', number_of_transfers)
            self.protocol_handle.set_request_value('spi_consecutive_transfer_configure:transfer_index', index)
            self.protocol_handle.set_request_value('spi_consecutive_transfer_configure:transfer_count', len(consecutive_transfer.write_data))
            self.protocol_handle.set_request_value('spi_consecutive_transfer_configure:pause_time_us', consecutive_transfer.pause_time_us)

            self.protocol_handle.send_command('spi_consecutive_transfer_configure')

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
                self.protocol_handle.set_request_value('spi_consecutive_transfer_write_buffer_put:transfer_index', index)
                self.protocol_handle.set_request_value('spi_consecutive_transfer_write_buffer_put:start_index', 0)
                self.protocol_handle.set_request_value('spi_consecutive_transfer_write_buffer_put:length', payload_length)
                self.protocol_handle.set_request_value('spi_consecutive_transfer_write_buffer_put:payload', payload)

                self.protocol_handle.send_command('spi_consecutive_transfer_write_buffer_put')

            elif( payload_length % TRANSFER_CHUNK_SIZE == 0 ):
                number_of_chunks = payload_length // TRANSFER_CHUNK_SIZE

                for chunk_number in range(0, number_of_chunks):
                    self.protocol_handle.set_request_value('spi_consecutive_transfer_write_buffer_put:transfer_index', index)
                    self.protocol_handle.set_request_value('spi_consecutive_transfer_write_buffer_put:start_index', chunk_number * TRANSFER_CHUNK_SIZE)
                    self.protocol_handle.set_request_value('spi_consecutive_transfer_write_buffer_put:length', TRANSFER_CHUNK_SIZE)
                    self.protocol_handle.set_request_value('spi_consecutive_transfer_write_buffer_put:payload', payload[chunk_number*TRANSFER_CHUNK_SIZE: (chunk_number+1)*TRANSFER_CHUNK_SIZE])

                    self.protocol_handle.send_command('spi_consecutive_transfer_write_buffer_put')

            elif( payload_length % TRANSFER_CHUNK_SIZE != 0 ):
                number_of_chunks = (TRANSFER_CHUNK_SIZE + payload_length - 1)// TRANSFER_CHUNK_SIZE

                for chunk_number in range(0, number_of_chunks):
                    is_last_chunk = (chunk_number == (number_of_chunks - 1))
                    self.protocol_handle.set_request_value('spi_consecutive_transfer_write_buffer_put:transfer_index', index)
                    self.protocol_handle.set_request_value('spi_consecutive_transfer_write_buffer_put:start_index', chunk_number * TRANSFER_CHUNK_SIZE)
                    self.protocol_handle.set_request_value('spi_consecutive_transfer_write_buffer_put:length', TRANSFER_CHUNK_SIZE if not is_last_chunk else (payload_length % TRANSFER_CHUNK_SIZE) )
                    self.protocol_handle.set_request_value('spi_consecutive_transfer_write_buffer_put:payload', payload[chunk_number*TRANSFER_CHUNK_SIZE: (chunk_number+1)*TRANSFER_CHUNK_SIZE])

                    self.protocol_handle.send_command('spi_consecutive_transfer_write_buffer_put')              

            index = index + 1

        self.protocol_handle.set_request_value('spi_consecutive_transfer_start:dummy', 1)
        self.protocol_handle.send_command('spi_consecutive_transfer_start')

        index = 0

        for consecutive_transfer in transfers:

            consecutive_transfer.read_data = b''

            length = len(consecutive_transfer.write_data)

            if(length < TRANSFER_CHUNK_SIZE):
                self.protocol_handle.set_request_value('spi_consecutive_transfer_read_buffer_get:transfer_index', index)
                self.protocol_handle.set_request_value('spi_consecutive_transfer_read_buffer_get:start_index', 0)
                self.protocol_handle.set_request_value('spi_consecutive_transfer_read_buffer_get:length', length)

                self.protocol_handle.send_command('spi_consecutive_transfer_read_buffer_get')

                consecutive_transfer.read_data = consecutive_transfer.read_data + bytes(ast.literal_eval(self.protocol_handle.get_response_value('spi_consecutive_transfer_read_buffer_get:payload')))
            elif( length % TRANSFER_CHUNK_SIZE == 0 ):
                number_of_chunks = length // TRANSFER_CHUNK_SIZE

                for chunk_number in range(0, number_of_chunks):
                    self.protocol_handle.set_request_value('spi_consecutive_transfer_read_buffer_get:transfer_index', index)
                    self.protocol_handle.set_request_value('spi_consecutive_transfer_read_buffer_get:start_index', TRANSFER_CHUNK_SIZE * chunk_number)
                    self.protocol_handle.set_request_value('spi_consecutive_transfer_read_buffer_get:length', TRANSFER_CHUNK_SIZE)

                    self.protocol_handle.send_command('spi_consecutive_transfer_read_buffer_get')

                    consecutive_transfer.read_data = consecutive_transfer.read_data + bytes(ast.literal_eval(self.protocol_handle.get_response_value('spi_consecutive_transfer_read_buffer_get:payload')))
            elif( length % TRANSFER_CHUNK_SIZE != 0 ):
                number_of_chunks = ( TRANSFER_CHUNK_SIZE + length - 1)// TRANSFER_CHUNK_SIZE

                for chunk_number in range(0, number_of_chunks):
                    is_last_chunk = (chunk_number == (number_of_chunks - 1))
                    self.protocol_handle.set_request_value('spi_consecutive_transfer_read_buffer_get:transfer_index', index)
                    self.protocol_handle.set_request_value('spi_consecutive_transfer_read_buffer_get:start_index', TRANSFER_CHUNK_SIZE * chunk_number)
                    self.protocol_handle.set_request_value('spi_consecutive_transfer_read_buffer_get:length', TRANSFER_CHUNK_SIZE if not is_last_chunk else (length % TRANSFER_CHUNK_SIZE) )

                    self.protocol_handle.send_command('spi_consecutive_transfer_read_buffer_get')

                    consecutive_transfer.read_data = consecutive_transfer.read_data + bytes(ast.literal_eval(self.protocol_handle.get_response_value('spi_consecutive_transfer_read_buffer_get:payload')))

            index = index + 1

        return transfers