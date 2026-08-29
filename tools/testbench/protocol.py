import serial
import yaml
import time
from ieee754 import single
import struct

PROTOCOL_SOF = b'\xFF'
PROTOCOL_EOF = b'\xAA'

class protocol:
    def __init__(self, serial_port: str, baud_rate: int, device_address: int, symfile: str, timeout: float = 1.0):

        # Open serial port.
        self.serial = serial.Serial(serial_port, baud_rate)

        self.serial.timeout = timeout
        self.serial.write_timeout = timeout

        if self.serial.is_open:
            self.serial.close()

        self.serial.open()

        # Store the device address.
        self.device_address: int = device_address

        with open(symfile) as self.symfile_handle:
            self.symfile_data = yaml.safe_load(self.symfile_handle)

            self.command_list = []
            self.command_request_field_list = []
            self.command_response_field_list = []

            for command in self.symfile_data['commands']:
                self.command_list.append(command)

                for field in self.symfile_data['commands'][command]['request']['fields']:
                    self.command_request_field_list.append( [command + "::request::" + field['name'], "", ""] )

                for field in self.symfile_data['commands'][command]['response']['fields']:
                    self.command_response_field_list.append( [command + "::response::" + field['name'], "", ""] )
        
        self.session_start_time = time.perf_counter()

    def __crc16_ccitt(self, data) -> int:
        crc = 0xFFFF

        for byte in data:
            crc ^= (byte << 8)

            for _ in range(8):
                if crc & 0x8000:
                    crc = ((crc << 1) ^ 0x1021) & 0xFFFF
                else:
                    crc = (crc << 1) & 0xFFFF

        return crc

    def __form_frame(self, command: int, payload_length:int, payload: bytearray, crc: int):

        frame = b''

        # Add start of frame byte.
        frame = frame + PROTOCOL_SOF

        # Add address byte.
        frame = frame + self.device_address.to_bytes()

        # Add command byte.
        frame = frame + command.to_bytes()

        # Add payload length.
        frame = frame + payload_length.to_bytes()

        # Add payload bytes.
        for entry in payload:
            frame = frame + entry.to_bytes()
        
        # Append CRC bytes.
        frame = frame + ( (crc >> 8) & 255 ).to_bytes()
        frame = frame + ( (crc) & 255 ).to_bytes()

        # Append end of frame byte.
        frame = frame + PROTOCOL_EOF

        #print("form_frame: Formed frame: {_frame}".format(_frame = frame))

        return frame
    
    def __query(self, command: int, payload_length: int, payload: bytearray):

        crc_data = []

        crc_data.append(command)
        crc_data.append(payload_length)
        
        for entry in payload:
            crc_data.append(entry)

        crc_value = self.__crc16_ccitt(crc_data)

        frame = self.__form_frame(command, payload_length, payload, crc_value)

        self.serial.write(frame)

        response_frame = b''

        # Read start of frame.
        response_frame = response_frame + self.serial.read_until(PROTOCOL_SOF)

        # Read device ID.
        response_frame = response_frame + self.serial.read(1)

        # Read command ID.
        response_frame = response_frame + self.serial.read(1)

        # Read payload length.
        response_frame = response_frame + self.serial.read(1)

        # Read payload bytes.
        response_frame = response_frame + self.serial.read(response_frame[3])

        # Read CRC bytes.
        response_frame = response_frame + self.serial.read(2)

        # Read end of frame.
        response_frame = response_frame + self.serial.read_until(PROTOCOL_EOF)

        #print("query: response frame: {_frame}".format(_frame = response_frame))

        return response_frame
    
    def set_request_value(self, field: str, value):
        
        command = field[ : field.find(':') ]
        field_name = field[ field.find(':') + 1 : ]

        target_field_name = command + "::request::" + field_name

        found = False
        index = 0

        for entry in self.command_request_field_list:

            #print("set_request_value: entry in request list: {_val}".format(_val = entry))
            #print("set_request_value: target request field: {_val}".format(_val = target_field_name))

            if entry[0] == target_field_name:
                found = True
                break

            index = index + 1
        
        if(found == False):
            #print("set_request_value: command not found")
            return
        
        field_datatype = 'uint8'
        field_is_a_list = False

        for entry in self.symfile_data['commands'][command]['request']['fields']:
            if entry['name'] == field_name:
                field_datatype = entry['type']
                field_is_a_list = entry['is_a_list']

        field_datatype_valid = False

        if field_datatype == 'uint8' or field_datatype == 'int8':
            if type(value) == type(0):
                field_datatype_valid = True
    
        if field_datatype == 'uint16' or field_datatype == 'int16':
            if type(value) == type(0):
                field_datatype_valid = True

        if field_datatype == 'uint32' or field_datatype == 'int32':
            if type(value) == type(0):
                field_datatype_valid = True

        if field_datatype == 'uint64' or field_datatype == 'int64':
            if type(value) == type(0):
                field_datatype_valid = True

        if field_datatype == 'float':
            if type(value) == type(0.0):
                field_datatype_valid = True

        if field_datatype == 'string':
            if type(value) == type('string'):
                field_datatype_valid = True

        if field_is_a_list:
            field_datatype_valid = True

        #print("send_command: field data type: {_datatype}".format(_datatype = field_datatype))

        if field_datatype_valid is False:
            #print("set_request_value: datatype mismatch")
            return
        
        self.command_request_field_list[index][1] = str(value)

        #print("set_request_value: value of field {_field} set to {_value} successfully".format(_field = field, _value = value))
    
    def get_request_value(self, field: str):

        command = field[ : field.find(':') ]
        field_name = field[ field.find(':') + 1 : ]

        target_field_name = command + "::request::" + field_name

        found = False
        index = 0

        for entry in self.command_request_field_list:

            #print("get_request_value: entry in request list: {_val}".format(_val = entry))
            #print("get_request_value: target request field: {_val}".format(_val = target_field_name))

            if entry[0] == target_field_name:
                found = True
                break

            index = index + 1
        
        if(found == False):
            #print("get_request_value: command not found")
            return None
        
        return self.command_request_field_list[index][1]

    def set_response_value(self, field: str, value):
        
        command = field[ : field.find(':') ]
        field_name = field[ field.find(':') + 1 : ]

        target_field_name = command + "::response::" + field_name

        found = False
        index = 0

        for entry in self.command_response_field_list:

            #print("set_response_value: entry in response list: {_val}".format(_val = entry))
            #print("set_response_value: target response field: {_val}".format(_val = target_field_name))

            if entry[0] == target_field_name:
                found = True
                break

            index = index + 1
        
        if(found == False):
            #print("set_response_value: command not found")
            return
        
        field_datatype = 'uint8'

        for entry in self.symfile_data['commands'][command]['response']['fields']:
            if entry['name'] == field_name:
                field_datatype = entry['type']

        field_datatype_valid = False

        if field_datatype == 'uint8' or field_datatype == 'int8':
            if type(value) == type(0):
                field_datatype_valid = True
    
        if field_datatype == 'uint16' or field_datatype == 'int16':
            if type(value) == type(0):
                field_datatype_valid = True

        if field_datatype == 'uint32' or field_datatype == 'int32':
            if type(value) == type(0):
                field_datatype_valid = True

        if field_datatype == 'uint64' or field_datatype == 'int64':
            if type(value) == type(0):
                field_datatype_valid = True

        if field_datatype == 'float':
            if type(value) == type(0.0):
                field_datatype_valid = True

        if field_datatype == 'string':
            if type(value) == type('string'):
                field_datatype_valid = True

        if field_datatype_valid is False:
            #print("set_response_value: datatype mismatch")
            return
        
        self.command_response_field_list[index][1] = str(value)

    def get_response_value(self, field: str):

        command = field[ : field.find(':') ]
        field_name = field[ field.find(':') + 1 : ]

        target_field_name = command + "::response::" + field_name

        found = False
        index = 0

        for entry in self.command_response_field_list:

            #print("get_response_value: entry in response list: {_val}".format(_val = entry))
            #print("get_response_value: target response field: {_val}".format(_val = target_field_name))

            if entry[0] == target_field_name:
                found = True
                break

            index = index + 1
        
        if(found == False):
            #print("get_response_value: command not found")
            return None

        return self.command_response_field_list[index][1]
    
    def send_command(self, command: str):

        field_list = []

        for entry in self.command_request_field_list:

            if command == entry[0][ : entry[0].find(':') ]:
                #print("send_command: appending field to payload for command {_command}: {_entry}".format(_command = command, _entry = entry))
                field_list.append(entry)
            
        #print("send_command: List of fields under command: {_command}".format(_command = command))
        #print(field_list)

        payload = b''
        index = 0

        for entry in field_list:
            field_type = self.symfile_data['commands'][command]['request']['fields'][index]['type']
            field_is_a_list = self.symfile_data['commands'][command]['request']['fields'][index]['is_a_list']
            field_value_str = field_list[index][1]

            #print("send_command: field entry being processed: {_field}".format(_field = field_list[index]))

            if(field_is_a_list == False):
                
                if(field_type == 'uint8'):
                    field_value = int(field_value_str)
                    field_value = field_value & 255
                    payload = payload + field_value.to_bytes()

                if(field_type == 'uint16'):
                    field_value = int(field_value_str)
                    payload = payload + ( (field_value >> 8 ) & 255 ).to_bytes()
                    payload = payload + ( field_value & 255 ).to_bytes()

                if(field_type == 'uint32'):
                    field_value = int(field_value_str)
                    payload = payload + ( (field_value >> 24 ) & 255 ).to_bytes()
                    payload = payload + ( (field_value >> 16 ) & 255 ).to_bytes()
                    payload = payload + ( (field_value >> 8 ) & 255 ).to_bytes()
                    payload = payload + ( field_value & 255 ).to_bytes()

                if(field_type == 'uint64'):
                    field_value = int(field_value_str)
                    payload = payload + ( (field_value >> 56 ) & 255 ).to_bytes()
                    payload = payload + ( (field_value >> 48 ) & 255 ).to_bytes()
                    payload = payload + ( (field_value >> 40 ) & 255 ).to_bytes()
                    payload = payload + ( (field_value >> 32 ) & 255 ).to_bytes()
                    payload = payload + ( (field_value >> 24 ) & 255 ).to_bytes()
                    payload = payload + ( (field_value >> 16 ) & 255 ).to_bytes()
                    payload = payload + ( (field_value >> 8 ) & 255 ).to_bytes()
                    payload = payload + ( field_value & 255 ).to_bytes()

                if(field_type == 'int8'):
                    field_value = int(field_value_str)
                    payload = payload + field_value.to_bytes()
                
                if(field_type == 'int16'):
                    field_value = int(field_value_str)

                    if(field_value < 0):
                        field_value = -field_value
                        field_value = field_value ^ 0xFFFF
                        field_value = field_value + 0x1
                    
                    field_value = field_value & 0xFFFF

                    payload = payload + ( ( field_value >> 8 ) & 255 ).to_bytes()
                    payload = payload + ( field_value & 255 ).to_bytes()

                if(field_type == 'int32'):
                    field_value = int(field_value_str)

                    if(field_value < 0):
                        field_value = -field_value
                        field_value = field_value ^ 0xFFFFFFFF
                        field_value = field_value + 0x1
                    
                    field_value = field_value & 0xFFFFFFFF

                    payload = payload + ( ( field_value >> 24 ) & 255 ).to_bytes()
                    payload = payload + ( ( field_value >> 16 ) & 255 ).to_bytes()
                    payload = payload + ( ( field_value >> 8 ) & 255 ).to_bytes()
                    payload = payload + ( field_value & 255 ).to_bytes()

                if(field_type == 'int64'):
                    field_value = int(field_value_str)

                    if(field_value < 0):
                        field_value = -field_value
                        field_value = field_value ^ 0xFFFFFFFFFFFFFFFF
                        field_value = field_value + 0x1
                    
                    field_value = field_value & 0xFFFFFFFFFFFFFFFF

                    payload = payload + ( ( field_value >> 56 ) & 255 ).to_bytes()
                    payload = payload + ( ( field_value >> 48 ) & 255 ).to_bytes()
                    payload = payload + ( ( field_value >> 40 ) & 255 ).to_bytes()
                    payload = payload + ( ( field_value >> 32 ) & 255 ).to_bytes()
                    payload = payload + ( ( field_value >> 24 ) & 255 ).to_bytes()
                    payload = payload + ( ( field_value >> 16 ) & 255 ).to_bytes()
                    payload = payload + ( ( field_value >> 8 ) & 255 ).to_bytes()
                    payload = payload + ( field_value & 255 ).to_bytes()

                if(field_type == 'float'):
                    field_value = int(single(float(field_value_str)).hex()[0], 16)

                    payload = payload + ( ( field_value >> 24 ) & 255 ).to_bytes()
                    payload = payload + ( ( field_value >> 16 ) & 255 ).to_bytes()
                    payload = payload + ( ( field_value >> 8 ) & 255 ).to_bytes()
                    payload = payload + ( ( field_value ) & 255 ).to_bytes()

                if(field_type == 'string'):
                    for element in field_value_str:
                        payload = payload + element.encode('utf-8')

            else:

                # Field values are represented as list as strings
                # example: "[1,2,3,4,5]".
                # Remove the '[' and ']' before splitting the string elements.

                #print(field_value_str)

                field_list_value = field_value_str
                field_list_value = field_list_value[:-1]
                field_list_value = field_list_value[1:]

                field_values_str = field_list_value.split(',')

                for field_value_str in field_values_str:

                    if(field_type == 'uint8'):
                        field_value = int(field_value_str)
                        field_value = field_value & 255
                        payload = payload + field_value.to_bytes()

                    if(field_type == 'uint16'):
                        field_value = int(field_value_str)
                        payload = payload + ( (field_value >> 8 ) & 255 ).to_bytes()
                        payload = payload + ( field_value & 255 ).to_bytes()

                    if(field_type == 'uint32'):
                        field_value = int(field_value_str)
                        payload = payload + ( (field_value >> 24 ) & 255 ).to_bytes()
                        payload = payload + ( (field_value >> 16 ) & 255 ).to_bytes()
                        payload = payload + ( (field_value >> 8 ) & 255 ).to_bytes()
                        payload = payload + ( field_value & 255 ).to_bytes()

                    if(field_type == 'uint64'):
                        field_value = int(field_value_str)
                        payload = payload + ( (field_value >> 56 ) & 255 ).to_bytes()
                        payload = payload + ( (field_value >> 48 ) & 255 ).to_bytes()
                        payload = payload + ( (field_value >> 40 ) & 255 ).to_bytes()
                        payload = payload + ( (field_value >> 32 ) & 255 ).to_bytes()
                        payload = payload + ( (field_value >> 24 ) & 255 ).to_bytes()
                        payload = payload + ( (field_value >> 16 ) & 255 ).to_bytes()
                        payload = payload + ( (field_value >> 8 ) & 255 ).to_bytes()
                        payload = payload + ( field_value & 255 ).to_bytes()

                    if(field_type == 'int8'):
                        field_value = int(field_value_str)
                        payload = payload + field_value.to_bytes()

                
                    if(field_type == 'int16'):
                        field_value = int(field_value_str)

                        if(field_value < 0):
                            field_value = -field_value
                            field_value = field_value ^ 0xFFFF
                            field_value = field_value + 0x1
                    
                        field_value = field_value & 0xFFFF

                        payload = payload + ( ( field_value >> 8 ) & 255 ).to_bytes()
                        payload = payload + ( field_value & 255 ).to_bytes()

                    if(field_type == 'int32'):
                        field_value = int(field_value_str)

                        if(field_value < 0):
                            field_value = -field_value
                            field_value = field_value ^ 0xFFFFFFFF
                            field_value = field_value + 0x1
                    
                        field_value = field_value & 0xFFFFFFFF

                        payload = payload + ( ( field_value >> 24 ) & 255 ).to_bytes()
                        payload = payload + ( ( field_value >> 16 ) & 255 ).to_bytes()
                        payload = payload + ( ( field_value >> 8 ) & 255 ).to_bytes()
                        payload = payload + ( field_value & 255 ).to_bytes()

                    if(field_type == 'int64'):
                        field_value = int(field_value_str)

                        if(field_value < 0):
                            field_value = -field_value
                            field_value = field_value ^ 0xFFFFFFFFFFFFFFFF
                            field_value = field_value + 0x1
                    
                        field_value = field_value & 0xFFFFFFFFFFFFFFFF

                        payload = payload + ( ( field_value >> 56 ) & 255 ).to_bytes()
                        payload = payload + ( ( field_value >> 48 ) & 255 ).to_bytes()
                        payload = payload + ( ( field_value >> 40 ) & 255 ).to_bytes()
                        payload = payload + ( ( field_value >> 32 ) & 255 ).to_bytes()
                        payload = payload + ( ( field_value >> 24 ) & 255 ).to_bytes()
                        payload = payload + ( ( field_value >> 16 ) & 255 ).to_bytes()
                        payload = payload + ( ( field_value >> 8 ) & 255 ).to_bytes()
                        payload = payload + ( field_value & 255 ).to_bytes()

                    if(field_type == 'float'):
                        field_value = int(single(float(field_value_str)).hex()[0], 16)

                        payload = payload + ( ( field_value >> 24 ) & 255 ).to_bytes()
                        payload = payload + ( ( field_value >> 16 ) & 255 ).to_bytes()
                        payload = payload + ( ( field_value >> 8 ) & 255 ).to_bytes()
                        payload = payload + ( ( field_value ) & 255 ).to_bytes()

                    if(field_type == 'str'):
                        for element in field_value_str:
                            payload = payload + element.encode('utf-8')

            index = index + 1
        
        #print("send_command: payload: {_payload}".format(_payload = payload))

        response_frame = self.__query(self.symfile_data['commands'][command]['id'], len(payload), payload)

        #print("send_command: response_frame: {_response_frame}".format(_response_frame = response_frame))

        # Process the response frame.

        response_frame_length = len(response_frame)

        if response_frame[0].to_bytes() != b'\xff' or response_frame[response_frame_length - 1].to_bytes() != b'\xaa':
            #print("send_command: start of frame / end of frame for response frame is incorrect.")
            return

        response_frame_crc = self.__crc16_ccitt(response_frame[2:response_frame_length-1])

        if response_frame == b'\xFF\xFF\x00\x00\x00\xAA':
            raise ValueError("send_command: target has not processed previous command")
            return False

        if response_frame_crc != 0:
            #print("send_command: crc error")
            return
        
        response_frame = response_frame[3:response_frame_length-2]

        response_length = response_frame[0]

        #print("send_command: response length: {_response_length}".format(_response_length = response_length))

        response_payload = response_frame[1:1+response_length]

        #print("send_command: response payload: {_response_payload}".format(_response_payload = response_payload))

        response_field_list = []

        for entry in self.command_response_field_list:
            if command == entry[0][ : entry[0].find(':') ]:
                #print("send_command: appending field to payload for response for command {_command}: {_entry}".format(_command = command, _entry = entry))
                response_field_list.append(entry)

        #print("send_command: List of fields under response for command: {_command}".format(_command = command))
        #print(response_field_list)   

        response_field_index = 0
        response_payload_byte_index = 0

        for entry in response_field_list:
            field_type = self.symfile_data['commands'][command]['response']['fields'][response_field_index]['type']
            field_is_a_list = self.symfile_data['commands'][command]['response']['fields'][response_field_index]['is_a_list']

            list_length_field = ''
            list_length = 0
            string_length_field = ''
            string_length = 0

            try:
                list_length_field = self.symfile_data['commands'][command]['response']['fields'][response_field_index]['list_length_field']
                
                #print("send command: List length field: {_value}".format( _value = list_length_field))

                for list_length_entry in response_field_list:
                    if list_length_field in list_length_entry[0]:
                        list_length = int(list_length_entry[1])
                        break
            except:
                list_length_field = ''

            try:
                string_length_field = self.symfile_data['commands'][command]['response']['fields'][response_field_index]['string_length_field']
                
                for list_length_entry in response_field_list:
                    if string_length_field in list_length_field[0]:
                        string_length = str(string_length_field[1])
                        break
            except:
                string_length_field = ''

            #print("send_command: response field entry being processed: {_field}".format(_field = response_field_list[response_field_index]))

            if(field_is_a_list == False):
                
                if(field_type == 'uint8'):
                    response_value = int(response_payload[response_payload_byte_index])
                    response_field_list[response_field_index][1] = str(response_value)
                    response_payload_byte_index = response_payload_byte_index + 1

                if(field_type == 'int8'):
                    response_value = int(response_payload[response_payload_byte_index])

                    if(response_value >= ( 1 << 7 )):
                        response_value = -response_value

                    response_field_list[response_field_index][1] = str(response_value)
                    response_payload_byte_index = response_payload_byte_index + 1

                if(field_type == 'uint16'):
                    response_value = ( int(response_payload[response_payload_byte_index]) << 8 )
                    response_value |=  int(response_payload[response_payload_byte_index + 1])
                    response_field_list[response_field_index][1] = str(response_value)
                    response_payload_byte_index = response_payload_byte_index + 2

                if(field_type == 'int16'):
                    response_value = ( int(response_payload[response_payload_byte_index]) << 8 )
                    response_value |=  int(response_payload[response_payload_byte_index + 1])

                    if(response_value >= ( 1 << 15 )):
                        response_value = -response_value

                    response_field_list[response_field_index][1] = str(response_value)
                    response_payload_byte_index = response_payload_byte_index + 2

                if(field_type == 'uint32'):
                    response_value = ( int(response_payload[response_payload_byte_index]) << 24 )
                    response_value |= ( int(response_payload[response_payload_byte_index + 1]) << 16 )
                    response_value |= ( int(response_payload[response_payload_byte_index + 2]) << 8 )
                    response_value |=  int(response_payload[response_payload_byte_index + 3])
                    response_field_list[response_field_index][1] = str(response_value)
                    response_payload_byte_index = response_payload_byte_index + 4

                if(field_type == 'int32'):
                    response_value = ( int(response_payload[response_payload_byte_index]) << 24 )
                    response_value |= ( int(response_payload[response_payload_byte_index + 1]) << 16 )
                    response_value |= ( int(response_payload[response_payload_byte_index + 2]) << 8 )
                    response_value |=  int(response_payload[response_payload_byte_index + 3])

                    if(response_value >= ( 1 << 31 )):
                        response_value = -response_value

                    response_field_list[response_field_index][1] = str(response_value)
                    response_payload_byte_index = response_payload_byte_index + 4

                if(field_type == 'uint64'):
                    response_value = ( int(response_payload[response_payload_byte_index]) << 56 )
                    response_value |= ( int(response_payload[response_payload_byte_index + 1]) << 48 )
                    response_value |= ( int(response_payload[response_payload_byte_index + 2]) << 40 )
                    response_value |= ( int(response_payload[response_payload_byte_index + 3]) << 32 )
                    response_value |= ( int(response_payload[response_payload_byte_index + 4]) << 24 )
                    response_value |= ( int(response_payload[response_payload_byte_index + 5]) << 16 )
                    response_value |= ( int(response_payload[response_payload_byte_index + 6]) << 8 )
                    response_value |=  int(response_payload[response_payload_byte_index + 7])
                    response_field_list[response_field_index][1] = str(response_value)
                    response_payload_byte_index = response_payload_byte_index + 8

                if(field_type == 'int64'):
                    response_value = ( int(response_payload[response_payload_byte_index]) << 56 )
                    response_value |= ( int(response_payload[response_payload_byte_index + 1]) << 48 )
                    response_value |= ( int(response_payload[response_payload_byte_index + 2]) << 40 )
                    response_value |= ( int(response_payload[response_payload_byte_index + 3]) << 32 )
                    response_value |= ( int(response_payload[response_payload_byte_index + 4]) << 24 )
                    response_value |= ( int(response_payload[response_payload_byte_index + 5]) << 16 )
                    response_value |= ( int(response_payload[response_payload_byte_index + 6]) << 8 )
                    response_value |=  int(response_payload[response_payload_byte_index + 7])

                    if(response_value >= ( 1 << 63 )):
                        response_value = -response_value

                    response_field_list[response_field_index][1] = str(response_value)
                    response_payload_byte_index = response_payload_byte_index + 8

                if(field_type == 'float'):
                    response_value = ( int(response_payload[response_payload_byte_index]) << 24 )
                    response_value |= ( int(response_payload[response_payload_byte_index + 1]) << 16 )
                    response_value |= ( int(response_payload[response_payload_byte_index + 2]) << 8 )
                    response_value |=  int(response_payload[response_payload_byte_index + 3])
                    value = struct.unpack('>f', struct.pack('>I', response_value))[0]
                    response_field_list[response_field_index][1] = str(value)
                    response_payload_byte_index = response_payload_byte_index + 4

                if(field_type == 'string'):
                   value = ''
                   for char_index in range(0, string_length):
                       value = value + chr(response_payload[response_payload_byte_index])
                       response_payload_byte_index = response_payload_byte_index + 1

            else:

                field_values = []

                for field_list_index in range(0, list_length):

                    if(field_type == 'uint8'):
                        response_value = int(response_payload[response_payload_byte_index])
                        field_values.append(str(response_value))
                        response_payload_byte_index = response_payload_byte_index + 1

                    if(field_type == 'int8'):
                        response_value = int(response_payload[response_payload_byte_index])

                        if(response_value >= ( 1 << 7 )):
                            response_value = -response_value

                        field_values.append(str(response_value))
                        response_payload_byte_index = response_payload_byte_index + 1

                    if(field_type == 'uint16'):
                        response_value = ( int(response_payload[response_payload_byte_index]) << 8 )
                        response_value |=  int(response_payload[response_payload_byte_index + 1])
                        field_values.append(str(response_value))
                        response_payload_byte_index = response_payload_byte_index + 2

                    if(field_type == 'int16'):
                        response_value = ( int(response_payload[response_payload_byte_index]) << 8 )
                        response_value |=  int(response_payload[response_payload_byte_index + 1])

                        if(response_value >= ( 1 << 15 )):
                            response_value = -response_value

                        field_values.append(str(response_value))
                        response_payload_byte_index = response_payload_byte_index + 2

                    if(field_type == 'uint32'):
                        response_value = ( int(response_payload[response_payload_byte_index]) << 24 )
                        response_value |= ( int(response_payload[response_payload_byte_index + 1]) << 16 )
                        response_value |= ( int(response_payload[response_payload_byte_index + 2]) << 8 )
                        response_value |=  int(response_payload[response_payload_byte_index + 3])
                        field_values.append(str(response_value))
                        response_payload_byte_index = response_payload_byte_index + 4

                    if(field_type == 'int32'):
                        response_value = ( int(response_payload[response_payload_byte_index]) << 24 )
                        response_value |= ( int(response_payload[response_payload_byte_index + 1]) << 16 )
                        response_value |= ( int(response_payload[response_payload_byte_index + 2]) << 8 )
                        response_value |=  int(response_payload[response_payload_byte_index + 3])

                        if(response_value >= ( 1 << 31 )):
                            response_value = -response_value

                        field_values.append(str(response_value))
                        response_payload_byte_index = response_payload_byte_index + 4

                    if(field_type == 'uint64'):
                        response_value = ( int(response_payload[response_payload_byte_index]) << 56 )
                        response_value |= ( int(response_payload[response_payload_byte_index + 1]) << 48 )
                        response_value |= ( int(response_payload[response_payload_byte_index + 2]) << 40 )
                        response_value |= ( int(response_payload[response_payload_byte_index + 3]) << 32 )
                        response_value |= ( int(response_payload[response_payload_byte_index + 4]) << 24 )
                        response_value |= ( int(response_payload[response_payload_byte_index + 5]) << 16 )
                        response_value |= ( int(response_payload[response_payload_byte_index + 6]) << 8 )
                        response_value |=  int(response_payload[response_payload_byte_index + 7])
                        field_values.append(str(response_value))
                        response_payload_byte_index = response_payload_byte_index + 8

                    if(field_type == 'int64'):
                        response_value = ( int(response_payload[response_payload_byte_index]) << 56 )
                        response_value |= ( int(response_payload[response_payload_byte_index + 1]) << 48 )
                        response_value |= ( int(response_payload[response_payload_byte_index + 2]) << 40 )
                        response_value |= ( int(response_payload[response_payload_byte_index + 3]) << 32 )
                        response_value |= ( int(response_payload[response_payload_byte_index + 4]) << 24 )
                        response_value |= ( int(response_payload[response_payload_byte_index + 5]) << 16 )
                        response_value |= ( int(response_payload[response_payload_byte_index + 6]) << 8 )
                        response_value |=  int(response_payload[response_payload_byte_index + 7])

                        if(response_value >= ( 1 << 63 )):
                            response_value = -response_value

                        field_values.append(str(response_value))
                        response_payload_byte_index = response_payload_byte_index + 8

                    if(field_type == 'float'):
                        response_value = ( int(response_payload[response_payload_byte_index]) << 24 )
                        response_value |= ( int(response_payload[response_payload_byte_index + 1]) << 16 )
                        response_value |= ( int(response_payload[response_payload_byte_index + 2]) << 8 )
                        response_value |=  int(response_payload[response_payload_byte_index + 3])
                        value = struct.unpack('>f', struct.pack('>I', response_value))[0]
                        #response_field_list[response_field_index][1] = str(value)
                        field_values.append(str(response_value))
                        response_payload_byte_index = response_payload_byte_index + 4
                
                field_list_values_str = ''

                field_list_str_index = 0

                if len(field_values) == 1:
                    field_list_values_str = field_list_values_str + '['
                    field_list_values_str = field_list_values_str + str(field_values[0])
                    field_list_values_str = field_list_values_str + ']'

                else:
                    field_list_values_str = field_list_values_str + '['

                    for field_list_value in field_values[:len(field_values) - 1]:
                        field_list_values_str = field_list_values_str + str(field_list_value) + ','

                    field_list_values_str = field_list_values_str + field_values[len(field_values) - 1] + ']'

                response_field_list[response_field_index][1] = field_list_values_str

            response_field_index = response_field_index + 1

        #print(response_field_list)

        return True