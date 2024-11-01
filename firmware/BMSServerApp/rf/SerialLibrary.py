import time
import serial
from robot.api.deco import keyword

class SerialLibrary:
    def __init__(self):
        self.serial_port = None

    @keyword
    def open_serial_port(self, port, baudrate):
        self.serial_port = serial.Serial(port, baudrate=int(baudrate), timeout=0)

    @keyword
    def write_to_serial(self, data):
        if self.serial_port:
            self.serial_port.write(data.encode())

    @keyword
    def read_from_serial(self, timeout):
        if self.serial_port:
            end_time = time.time() + float(timeout)
            response = ''
            while time.time() < end_time:
                data = self.serial_port.read(1)
                if data:
                    response += data.decode('ISO-8859-1')
                    if data == b'\n':
                        break
                else:
                    time.sleep(0.1)
            response = response.strip()
            return response if response else None
        return None

    @keyword
    def close_serial_port(self):
        if self.serial_port:
            self.serial_port.close()
            self.serial_port = None
