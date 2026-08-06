from enum import Enum
from . import protocol
import ast

HSADC_MAX_SAMPLE_LENGTH = 2048
HSADC_MAX_SAMPLE_CHUNK_LENGTH = 100

class adc:

    class channels(Enum):
        CHANNEL_0 = 0
        CHANNEL_1 = 1
        CHANNEL_BOTH = 2

    class sampling_rate(Enum):
        SAMPLING_RATE_1000KSPS = 0
        SAMPLING_RATE_500KSPS = 1
        SAMPLING_RATE_250KSPS = 2
        SAMPLING_RATE_125KSPS = 3
        SAMPLING_RATE_62K5SPS = 4
        SAMPLING_RATE_31K25SPS = 5
        SAMPLING_RATE_15K625SPS = 6

    class adc_samples:
        def __init__(self, samples_ch0 = [], samples_ch1 = []):
            self.channel_0 = samples_ch0
            self.channel_1 = samples_ch1

    def __init__(self, protocol_handle: protocol.protocol):
        self.protocol_handle: protocol.protocol = protocol_handle
        self.channel_selection = self.channels.CHANNEL_0

    def configure(self, channel: adc.channels, sampling_rate: adc.sampling_rate):

        self.protocol_handle.set_request_value('hsadc_configure:channels', int(channel.value))
        self.protocol_handle.set_request_value('hsadc_configure:sampling_rate', int(sampling_rate.value))

        self.protocol_handle.send_command('hsadc_configure')

        status = int(self.protocol_handle.get_response_value('hsadc_configure:status'))

        self.channel_selection = channel

        if status != 0:
            raise ValueError("Status not equal to zero !")

    def acquire(self):

        sample_length = HSADC_MAX_SAMPLE_LENGTH

        if self.channel_selection == adc.channels.CHANNEL_BOTH:
            sample_length //= 2

        samples: adc.adc_samples = adc.adc_samples(samples_ch0= [], samples_ch1= [])

        self.protocol_handle.set_request_value('hsadc_control:start_conversion', int(True))
        self.protocol_handle.send_command('hsadc_control')

        if(self.channel_selection == self.channels.CHANNEL_0 or self.channel_selection == self.channels.CHANNEL_BOTH):

            number_of_chunks = (
                    sample_length +
                    HSADC_MAX_SAMPLE_CHUNK_LENGTH - 1
                ) // HSADC_MAX_SAMPLE_CHUNK_LENGTH


            for chunk in range(0, number_of_chunks):

                is_last_chunk = (chunk == (number_of_chunks - 1))
                chunk_length = HSADC_MAX_SAMPLE_CHUNK_LENGTH if not is_last_chunk else (sample_length % HSADC_MAX_SAMPLE_CHUNK_LENGTH)

                self.protocol_handle.set_request_value('hsadc_get_samples:channel', int(0))
                self.protocol_handle.set_request_value('hsadc_get_samples:start_index', (chunk*HSADC_MAX_SAMPLE_CHUNK_LENGTH))
                self.protocol_handle.set_request_value('hsadc_get_samples:length', chunk_length)

                self.protocol_handle.send_command('hsadc_get_samples')            

                samples.channel_0 = samples.channel_0 + ast.literal_eval( self.protocol_handle.get_response_value('hsadc_get_samples:samples') )

        if(self.channel_selection == self.channels.CHANNEL_1 or self.channel_selection == self.channels.CHANNEL_BOTH):

            number_of_chunks = (
                sample_length +
                HSADC_MAX_SAMPLE_CHUNK_LENGTH - 1
            ) // HSADC_MAX_SAMPLE_CHUNK_LENGTH

            for chunk in range(0, number_of_chunks):

                is_last_chunk = (chunk == (number_of_chunks - 1))
                chunk_length = HSADC_MAX_SAMPLE_CHUNK_LENGTH if not is_last_chunk else (sample_length % HSADC_MAX_SAMPLE_CHUNK_LENGTH)

                self.protocol_handle.set_request_value('hsadc_get_samples:channel', int(1))
                self.protocol_handle.set_request_value('hsadc_get_samples:start_index', (chunk*HSADC_MAX_SAMPLE_CHUNK_LENGTH))
                self.protocol_handle.set_request_value('hsadc_get_samples:length', chunk_length)

                self.protocol_handle.send_command('hsadc_get_samples')            

                samples.channel_1 = samples.channel_1 + ( ast.literal_eval( self.protocol_handle.get_response_value('hsadc_get_samples:samples') ) )

        return samples