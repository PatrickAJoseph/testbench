from enum import Enum
from . import protocol
import ast

class capture:

    class channel(Enum):
        CHANNEL_0 = 0
        CHANNEL_1 = 1

    class capture_data:
        def __init__(self, frequency: int, duty: float, on_time_ns: int, off_time_ns: int, period_ns: int):
            self.frequency = frequency
            self.duty = duty
            self.on_time_ns = on_time_ns
            self.off_time_ns = off_time_ns
            self.period_ns = period_ns

    def __init__(self, protocol_handle: protocol.protocol):
        self.protocol_handle = protocol_handle

    def start(self, channel_id: capture.channel):

        self.protocol_handle.set_request_value('capture_control:start', int(True))
        self.protocol_handle.set_request_value('capture_control:stop', int(False))
        self.protocol_handle.set_request_value('capture_control:channel', int(channel_id.value))

        self.protocol_handle.send_command('capture_control')

    def stop(self, channel_id: capture.channel):

        self.protocol_handle.set_request_value('capture_control:start', int(False))
        self.protocol_handle.set_request_value('capture_control:stop', int(True))
        self.protocol_handle.set_request_value('capture_control:channel', int(channel_id.value))

        self.protocol_handle.send_command('capture_control')

    def get_data(self, channel_id: capture.channel):

        self.protocol_handle.set_request_value('capture_get_data:channel', int(channel_id.value))
        self.protocol_handle.send_command('capture_get_data')

        frequency = int(self.protocol_handle.get_response_value('capture_get_data:frequency'))
        duty = float(self.protocol_handle.get_response_value('capture_get_data:duty'))
        on_time_ns = int(self.protocol_handle.get_response_value('capture_get_data:on_time_ns'))
        off_time_ns = int(self.protocol_handle.get_response_value('capture_get_data:off_time_ns'))
        period_ns = int(self.protocol_handle.get_response_value('capture_get_data:period_ns'))

        capture_data = capture.capture_data(frequency = frequency, duty = duty, on_time_ns = on_time_ns, off_time_ns = off_time_ns, period_ns = period_ns)

        return capture_data
