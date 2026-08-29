from enum import Enum
from . import protocol
import ast

MAX_SAMPLE_LENGTH = 4096
TRANSFER_CHUNK_SIZE = 200

class gpio:

    def __init__(self, protocol_handle: protocol.protocol):
        self.protocol_handle = protocol_handle
        self.pattern_length:int = 0
        self.repeat_count: int = 0

    def set_output_pin_state(self, pin: int, state: bool):

        if( pin > 7 ):
            raise ValueError("Pin index must be within 0 to 7 !")

        self.protocol_handle.set_request_value('gpio_set_pin_state:pin', pin)
        self.protocol_handle.set_request_value('gpio_set_pin_state:state', int(state))

        self.protocol_handle.send_command('gpio_set_pin_state')

    def get_input_pin_state(self, pin:int) -> bool:

        if( pin > 7 ):
            raise ValueError("Pin index must be within 0 to 7")

        self.protocol_handle.set_request_value('gpio_get_pin_state:pin', pin)

        self.protocol_handle.send_command('gpio_get_pin_state')

        return True if int(self.protocol_handle.get_response_value('gpio_get_pin_state:state')) else False

    def port_write(self, value: int):

        self.protocol_handle.set_request_value('gpio_write_port:value', value)
        self.protocol_handle.send_command('gpio_write_port')

    def port_read(self):

        self.protocol_handle.set_request_value('gpio_read_port:dummy', 0)
        self.protocol_handle.send_command('gpio_read_port')

        port_value = int(self.protocol_handle.get_response_value('gpio_read_port:value'))

        return port_value

    def pattern_generator_configure(self, length: int, rate: int):

        self.pattern_length = length

        if(length > MAX_SAMPLE_LENGTH):
            raise ValueError("Length of samples for pattern generator exceeded")

        self.protocol_handle.set_request_value('gpio_pattern_generator_configure:length', length)
        self.protocol_handle.set_request_value('gpio_pattern_generator_configure:rate', rate)

        self.protocol_handle.send_command('gpio_pattern_generator_configure')

    def pattern_generator_configure_repeated(self, length: int, count: int, rate: int):
        self.repeat_count = count
        self.pattern_generator_configure( length = (length * count), rate = rate )

    def pattern_generator_write_output_pattern(self, pattern):

        if(self.pattern_length < TRANSFER_CHUNK_SIZE):

            self.protocol_handle.set_request_value('gpio_pattern_generator_write_output_sample:start_index',0)
            self.protocol_handle.set_request_value('gpio_pattern_generator_write_output_sample:length', self.pattern_length)
            self.protocol_handle.set_request_value('gpio_pattern_generator_write_output_sample:samples', str(pattern))

            self.protocol_handle.send_command('gpio_pattern_generator_write_output_sample')

        elif( self.pattern_length % TRANSFER_CHUNK_SIZE == 0 ):

            number_of_chunks = self.pattern_length // TRANSFER_CHUNK_SIZE

            for chunk_number in range(0, number_of_chunks):

                self.protocol_handle.set_request_value('gpio_pattern_generator_write_output_sample:start_index',TRANSFER_CHUNK_SIZE*chunk_number)
                self.protocol_handle.set_request_value('gpio_pattern_generator_write_output_sample:length', TRANSFER_CHUNK_SIZE)
                self.protocol_handle.set_request_value('gpio_pattern_generator_write_output_sample:samples', str(pattern[chunk_number*TRANSFER_CHUNK_SIZE: (chunk_number+1)*TRANSFER_CHUNK_SIZE]))

                self.protocol_handle.send_command('gpio_pattern_generator_write_output_sample')

        elif( self.pattern_length % TRANSFER_CHUNK_SIZE != 0 ):

            number_of_chunks = (self.pattern_length + TRANSFER_CHUNK_SIZE - 1)// TRANSFER_CHUNK_SIZE

            for chunk_number in range(0, number_of_chunks):

                is_last_chunk = (chunk_number == (number_of_chunks - 1))

                self.protocol_handle.set_request_value('gpio_pattern_generator_write_output_sample:start_index',TRANSFER_CHUNK_SIZE*chunk_number)
                self.protocol_handle.set_request_value('gpio_pattern_generator_write_output_sample:length', TRANSFER_CHUNK_SIZE if not is_last_chunk else (self.pattern_length % TRANSFER_CHUNK_SIZE))
                self.protocol_handle.set_request_value('gpio_pattern_generator_write_output_sample:samples', str(pattern[chunk_number*TRANSFER_CHUNK_SIZE: (chunk_number+1)*TRANSFER_CHUNK_SIZE]))

                self.protocol_handle.send_command('gpio_pattern_generator_write_output_sample')

    def pattern_generator_write_output_pattern_repeated(self, pattern):

        _pattern = []

        for i in range(0, self.repeat_count):
            _pattern = _pattern + pattern

        self.pattern_generator_write_output_pattern(_pattern)

    def pattern_generator_read_input_pattern(self):

        length = self.pattern_length
        input_pattern = []

        if(length < TRANSFER_CHUNK_SIZE):
            self.protocol_handle.set_request_value('gpio_pattern_generator_read_input_sample:start_index', 0)
            self.protocol_handle.set_request_value('gpio_pattern_generator_read_input_sample:length', length)

            self.protocol_handle.send_command('gpio_pattern_generator_read_input_sample')

            input_pattern = input_pattern + ast.literal_eval(self.protocol_handle.get_response_value('gpio_pattern_generator_read_input_sample:samples'))
        elif( length % TRANSFER_CHUNK_SIZE == 0 ):

            number_of_chunks = length // TRANSFER_CHUNK_SIZE

            for chunk_number in range(0, number_of_chunks):
                self.protocol_handle.set_request_value('gpio_pattern_generator_read_input_sample:start_index', chunk_number * TRANSFER_CHUNK_SIZE)
                self.protocol_handle.set_request_value('gpio_pattern_generator_read_input_sample:length', TRANSFER_CHUNK_SIZE)

                self.protocol_handle.send_command('gpio_pattern_generator_read_input_sample')

                input_pattern = input_pattern + ast.literal_eval(self.protocol_handle.get_response_value('gpio_pattern_generator_read_input_sample:samples'))
        elif( length % TRANSFER_CHUNK_SIZE != 0 ):
            number_of_chunks = (length + TRANSFER_CHUNK_SIZE - 1) // TRANSFER_CHUNK_SIZE

            for chunk_number in range(0, number_of_chunks):

                is_last_chunk = ( chunk_number == (number_of_chunks - 1) )

                self.protocol_handle.set_request_value('gpio_pattern_generator_read_input_sample:start_index', chunk_number * TRANSFER_CHUNK_SIZE)
                self.protocol_handle.set_request_value('gpio_pattern_generator_read_input_sample:length', TRANSFER_CHUNK_SIZE if not is_last_chunk else (length % TRANSFER_CHUNK_SIZE))

                self.protocol_handle.send_command('gpio_pattern_generator_read_input_sample')

                input_pattern = input_pattern + ast.literal_eval(self.protocol_handle.get_response_value('gpio_pattern_generator_read_input_sample:samples'))

        return input_pattern

    def pattern_generator_read_input_pattern_repeated(self):
        _pattern = self.pattern_generator_read_input_pattern()
        _pattern_array = []

        element_length = self.pattern_length // self.repeat_count

        for i in range(0 , self.repeat_count):
            _pattern_array.append(_pattern[element_length*i : element_length*(i+1)])

        return _pattern_array

    def pattern_generator_start(self):
        self.protocol_handle.set_request_value('gpio_pattern_generator_start:start', 1)
        self.protocol_handle.send_command('gpio_pattern_generator_start')

    def pattern_generator_stop(self):
        self.protocol_handle.set_request_value('gpio_pattern_generator_stop:stop', 1)
        self.protocol_handle.send_command('gpio_pattern_generator_stop')
