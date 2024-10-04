import time
import serial
import crcmod.predefined

# Calculate CRC
def calculate_crc(data):
    crc16 = crcmod.predefined.mkPredefinedCrcFun('modbus')
    return crc16(data)

def modbus_request(port, register_count=6):
    # Send request for register values.
    request = [0x01, 0x04, 0x00, 0x00, 0x00, register_count]
    crc = calculate_crc(bytearray(request))
    request += [crc & 0xFF, (crc >> 8) & 0xFF]  # Add CRC to the request
    print("Request:", request)

    try:
        port.write(bytearray(request))
        
        # Add a delay after sending the request
        time.sleep(0.5)  # 100 ms delay; adjust as necessary
        
        expected_length = 3 + 2 * register_count + 2  # Function code + byte count + data + CRC
        received = port.read(expected_length)
        
        print("Received bytes:", list(received))
        print("Received length:", len(received))

        if len(received) < expected_length:
            print("Error: Received data is shorter than expected.")
            return [0] * register_count  # Return default values on error
        
        # Calculate CRC of the received data
        response_crc = calculate_crc(received[:-2])  # Exclude CRC bytes for calculation
        received_crc = (received[-2] | (received[-1] << 8))

        # Verify CRC of the response
        if response_crc != received_crc:
            print("CRC verification failed.")
            return [0] * register_count  # Return default values on failure
        else:
            print("CRC success")

        # Extract values from the slave's response
        values = [(received[3 + (i * 2)] << 8) | received[4 + (i * 2)] for i in range(register_count)]
        for i in range(register_count):
            print(f"Sensor {i+1} Value: {values[i]}")

        return values

    except Exception as e:
        print(f"An error occurred during request: {e}")
        return [0] * register_count  # Return default values on error

# Initialize serial port
port = serial.Serial('COM6', baudrate=9600, timeout=1, dsrdtr=False, bytesize=serial.EIGHTBITS)

try:
    # Main loop
    while True:
        values = modbus_request(port)
        for i in range(len(values)): 
            print(f"Sensor {i+1} Value: {values[i]}")  # Display sensor values

        time.sleep(0.2)  # Delay before the next request; adjust as necessary

except Exception as e:
    print(f"An error occurred: {e}")

finally:
    if port.is_open:
        port.close()
