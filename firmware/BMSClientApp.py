import time
import serial
import crcmod.predefined

# Calculate CRC
def calculate_crc(data):
    crc16 = crcmod.predefined.mkPredefinedCrcFun('modbus')
    return crc16(data)

def modbus_request(port):
    # Send request for 2 registers values.
    # hexadecimal_string = [0x01, 0x04, 0x00, 0x00, 0x00, 0x02, 0x71, 0xCB]

    # Send request for 6 registers values.
    hexadecimal_string = [0x01, 0x04, 0x00, 0x00, 0x00, 0x06]  
    crc = calculate_crc(bytearray(hexadecimal_string))
    hexadecimal_string += [crc & 0xFF, (crc >> 8) & 0xFF]  # Add CRC to the request
    print("Request:", hexadecimal_string)

    try:
        if not port.is_open:
            port.open()
        port.write(bytearray(hexadecimal_string))
        received = port.read(17)  # Read 17 bytes (response should include 12 bytes of data + CRC)
        
        print("Received bytes:", received)
        print("Received length:", len(received))

        if len(received) < 17:
            print("Error: Received data is shorter than expected.")
            return [0]*6  # Return default values on error
        
        # Calculate CRC of the received data
        response_crc = calculate_crc(received[:-2])  # Exclude CRC bytes for calculation
        received_crc = (received[-2] | (received[-1] << 8))

        # Verify CRC of the response
        if response_crc != received_crc:
            print("CRC verification failed.")
            return [0] * 6 # Return default values on failure
        else:
            print("CRC success")

        # Extract values from slave's response
        values = [0]*6

        # return value
        for i in range(6):
            values[i] = ((received[3 + (i * 2)] << 8) | received[4 + (i * 2)]) & 0xFFFF
            print(f"Sensor {i+1} Value: {values[i]}")

        return values

    except Exception as e:
        print(f"An error occurred: {e}")
        return [0] *6  # Return default values on error

    finally:
        if port.is_open:
            port.close()

# Initialize serial port
port = serial.Serial('COM6', baudrate=9600, timeout=1, dsrdtr=False, bytesize=serial.EIGHTBITS)

# Main loop
while True:
    try:
        values = modbus_request(port)
        for i in range(0, 6): 
            print(f"Sensor {i+1} Value: {values[i]}") #temp1,temp2,voltage1,2,3,4, maybe also current?

    except Exception as e:
        print(f"An error occurred: {e}")

    time.sleep(2)
