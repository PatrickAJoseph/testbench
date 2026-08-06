from enum import Enum
from . import protocol
import ast

class pwm:

    class channel(Enum):
        CHANNEL_0 = 0
        CHANNEL_1 = 1
        CHANNEL_2 = 2
        CHANNEL_3 = 3

    def __init__(self, protocol_handle: protocol.protocol):
        self.protocol_handle =  protocol_handle
        self.frequency = [1000, 1000, 1000, 1000]
        self.duty = [ 0.5, 0.5, 0.5, 0.5 ]
        self.enabled = [ False, False, False, False ]

        self.protocol_handle.set_request_value('pwm_config:channel', 0)
        self.protocol_handle.set_request_value('pwm_config:enable', 0)
        self.protocol_handle.set_request_value('pwm_config:frequency', 1000)
        self.protocol_handle.set_request_value('pwm_config:duty', 0.0)

    def enable(self, channel_id: channel):
        self.protocol_handle.set_request_value('pwm_config:channel', channel_id.value)
        self.protocol_handle.set_request_value('pwm_config:enable', int(True))
        self.protocol_handle.send_command('pwm_config')

    def disable(self, channel_id: channel):
        self.protocol_handle.set_request_value('pwm_config:channel', channel_id.value)
        self.protocol_handle.set_request_value('pwm_config:enable', int(False))
        self.protocol_handle.send_command('pwm_config')

    def set_frequency(self, channel_id: channel, frequency: int):
        self.protocol_handle.set_request_value('pwm_config:channel', channel_id.value)
        self.protocol_handle.set_request_value('pwm_config:frequency', frequency)
        self.protocol_handle.send_command('pwm_config')

    def set_duty(self, channel_id: channel, duty: float):
        self.protocol_handle.set_request_value('pwm_config:channel', channel_id.value)
        self.protocol_handle.set_request_value('pwm_config:duty', duty)
        self.protocol_handle.send_command('pwm_config')
