#include <string.h>  // For memset function
#include "modbus.h"

#include "bms_configuration.h"
#include "crc.h"
#include "mux.h"
#include "usart2.h"
#include "utils.h"
#include "adc.h"
#include "adc_converter.h"
#include "modbus.h"

static uint8_t modbus_flag = 0;
static uint8_t received_slave_addr = 0;
static uint8_t modbus_rx_buffer[BUFFER_SIZE] = {0};
static int sensor_values_ready = 0;

static void read_sensor_values(int16_t *sensors);
static void handle_wrong_slave_address(void);
static void parse_register_request(const uint8_t* modbus_rx_buffer, uint16_t* reg_addr, uint16_t* reg_quantity);
static void construct_response_frame(const int16_t* sensor_values);

void read_sensor_values(int16_t *sensors) {
    //char buf[100];

    // Read ADC value for cell voltage 1
    const uint16_t adc_value = adc_read(0);
    uint16_t cell_voltage = adc_convert_cell_voltage(adc_value);
    //sprintf(buf, "Simulated cell voltage: %u mV, corresponding ADC value: %u\r\n", cell_voltage, adc_value);
    //USART2_send_string(buf);

    // Read ADC value for battery temperature
    const uint16_t lm35_o = adc_read(1);
    const uint16_t lm35_d = adc_read(2);
    int16_t battery_temp = adc_convert_battery_temp(lm35_o,lm35_d);
    //sprintf(buf, "Simulated Battery Temperature: %d degrees Celsius\r\n", battery_temp);
    //USART2_send_string(buf); // Send battery temperature information

    // Read ADC value for battery current
    const uint16_t adc_c = adc_read(4);
    uint16_t battery_current = adc_convert_battery_current(adc_c);
    //sprintf(buf, "Simulated Battery Current: %u mA\r\n", battery_current);
    //USART2_send_string(buf); // Send battery current information

    // Send the sensor values to the sensors array
    sensors[0] = battery_temp;    // Store battery temperature in sensors array
    sensors[1] = battery_temp-12;        // Store ADC value for battery temperature
    sensors[2] = cell_voltage;     // Store cell voltage in sensors array

    sensors[3] = cell_voltage+1;
    sensors[4] = cell_voltage-1;
    sensors[5] = cell_voltage+2;
    sensors[6] = battery_current;
}

// Main Modbus polling function
void modbus_poll(int16_t* sensor_values) {
    switch (modbus_flag) {
        case MODBUS_WAITING:  // Waiting for a valid frame
            memset(modbus_rx_buffer, 0, BUFFER_SIZE);
            break;

        case MODBUS_FRAME_RECEIVED: {  // Handle received frame
            modbus_rx_buffer[0] = SLAVE_ADDR;  // Set slave address
            for (uint8_t i = 1; i < BUFFER_SIZE; i++) {
                modbus_rx_buffer[i] = USART2_read();  // Read remaining bytes
            }

            uint16_t received_crc = CRC16(modbus_rx_buffer, 6);  // CRC calculation on first 6 bytes
            uint8_t crc_low = received_crc & 0xFF;
            uint8_t crc_high = (received_crc >> 8) & 0xFF;

            delay_ms(40);  // Small delay for reliability

            // Validate CRC
            if (crc_high == modbus_rx_buffer[7] && crc_low == modbus_rx_buffer[6]) {
                uint16_t reg_addr[MAX_REGISTERS] = {0};
                uint16_t reg_quantity = 0;
                parse_register_request(modbus_rx_buffer, reg_addr, &reg_quantity);  // Extract register info

                if (reg_quantity > 0) {
                    if (!sensor_values_ready) {
                        int16_t temp_values[NUM_REGISTERS] = {0};
                        read_sensor_values(temp_values);  // Populate sensor values

                        // Copy new values to sensor array
                        for (uint8_t i = START_ADDR; i < NUM_REGISTERS; i++) {
                            sensor_values[i] = temp_values[i];
                        }

                        sensor_values_ready = 1;  // Mark as updated
                    }

                    construct_response_frame(sensor_values);  // Send response
                }
            }
            delay_ms(20);
            modbus_flag = MODBUS_WAITING;
            USART2->CR1 |= 0x0020;  // Re-enable RX interrupt
            break;
        }

        case MODBUS_WRONG_ADDRESS:
            handle_wrong_slave_address();
            break;

        default:
            modbus_flag = MODBUS_WAITING;
            break;
    }
}

// Interrupt handler for USART2
void USART2_IRQHandler(void) {
    if (USART2->SR & 0x0020) {  // Check if data is available in DR register
        received_slave_addr = USART2->DR;  // Read the received address
    }
    if (received_slave_addr == SLAVE_ADDR) {  // Valid slave address
        modbus_flag = MODBUS_FRAME_RECEIVED;
    } else {  // Wrong address
        modbus_flag = MODBUS_WRONG_ADDRESS;
    }
    USART2->CR1 &= ~0x0020;  // Disable RX interrupt to avoid repeated triggers
}

// Handle wrong slave address by resetting the receiver
static void handle_wrong_slave_address(void) {
    USART2->CR1 &= ~0x00000004;  // Disable receiver
    delay_ms(10);                // Wait for erroneous address handling
    USART2->CR1 |= 0x00000004;   // Re-enable receiver
    USART2->CR1 |= 0x0020;       // Re-enable RX interrupt
    modbus_flag = MODBUS_WAITING;
}

// Parse the register request from the received frame
static void parse_register_request(const uint8_t* modbus_rx_buffer, uint16_t* reg_addr, uint16_t* reg_quantity) {
    uint16_t start_addr = (modbus_rx_buffer[2] << 8) | modbus_rx_buffer[3];
    *reg_quantity = (modbus_rx_buffer[4] << 8) | modbus_rx_buffer[5];

    if (*reg_quantity > MAX_REGISTERS) {
        //*reg_quantity = 0;  // Handle invalid quantity
    }

    // Assign register addresses
    for (uint8_t i = 0; i < *reg_quantity; i++) {
        reg_addr[i] = start_addr + i;
    }
}

// Construct and send the Modbus response frame
static void construct_response_frame(const int16_t* sensor_values) {
    uint8_t response_length = RESPONSE_BYTE_COUNT + 5;  // Total response length (data + CRC + header)
    uint8_t response_frame[RESPONSE_BYTE_COUNT + 5] = {0};

    response_frame[0] = SLAVE_ADDR;       // Set slave address
    response_frame[1] = FUNCTION_CODE;    // Function code
    response_frame[2] = RESPONSE_BYTE_COUNT;  // Byte count

    // Fill the response with sensor values (high and low bytes)
    for (uint8_t i = 0; i < NUM_REGISTERS; i++) {
        response_frame[2 * i + 3] = (sensor_values[i] >> 8) & 0xFF;  // High byte
        response_frame[2 * i + 4] = sensor_values[i] & 0xFF;         // Low byte
    }

    // Calculate and append CRC to the response
    uint16_t response_crc = CRC16(response_frame, response_length - 2);
    response_frame[response_length - 2] = response_crc & 0xFF;          // Low byte of CRC
    response_frame[response_length - 1] = (response_crc >> 8) & 0xFF;   // High byte of CRC

    // Send response frame over USART
    for (uint8_t i = 0; i < response_length; i++) {
        USART2_write(response_frame[i]);
    }

    delay_ms(10);  // Allow time for transmission
}
