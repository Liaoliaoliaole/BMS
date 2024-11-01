import time
import serial
import crcmod.predefined

# Initialize serial port
port = serial.Serial('COM7', baudrate=9600, timeout=1, dsrdtr=False, bytesize=serial.EIGHTBITS)

# Calculate CRC
def calculate_crc(data):
    crc16 = crcmod.predefined.mkPredefinedCrcFun('modbus')
    return crc16(data)

# Send modbus request and receive data
def modbus_request(port, slave_addr=0x01, function_code=0x04, start_addr=0x00, register_count=8):
    # Send request for register values.
    request = [slave_addr, function_code, (start_addr >> 8) & 0xFF, start_addr & 0xFF, (register_count >> 8) & 0xFF, register_count & 0xFF]
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
        return values

    except Exception as e:
        print(f"An error occurred during request: {e}")
        return [0] * register_count  # Return default values on error

try:
    # Main loop
    while True:
        # Request sensor values from the ServerApp
        values = modbus_request(port)

        # Display sensor values
        if values[0] != 0:  # Check if valid data is received
            print("\n--- Sensor Values ---")
            print(f"Battery Temperature 1: {values[0]} hundredths of a degree Celsius")
            print(f"Battery Temperature 2: {values[1]} hundredths of a degree Celsius")
            print(f"Voltage Cell 1: {values[2]} mV")
            print(f"Voltage Cell 2: {values[3]} mV")
            print(f"Voltage Cell 3: {values[4]} mV")
            print(f"Voltage Cell 4: {values[5]} mV")
            print(f"Current (Charge): {values[6]} mA")
            print(f"Current (Discharge): {values[7]} mA")
            print("---------------------\n")

        time.sleep(0.5)  # Delay before the next request; adjust as necessary

except Exception as e:
    print(f"An error occurred: {e}")

finally:
    if port.is_open:
        port.close()
