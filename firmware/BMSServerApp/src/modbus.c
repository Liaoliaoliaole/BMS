#include <string.h>  // For memset function
#include <stdint.h>

#include "bms_configuration.h"
#include "crc.h"
#include "mux.h"
#include "usart2.h"
#include "utils.h"
#include "sensor_reader.h"
#include "system.h"
#include "modbus.h"

// Modbus-related state and buffers encapsulated within modbus
static uint8_t modbus_flag = 0; //Keeps track of the Modbus state
static uint8_t received_slave_addr = 0;
static uint8_t modbus_rx_buffer[BUFFER_SIZE] = {0};
static int sensor_values_ready = 0;

// Function declarations for internal Modbus handling
static void handle_wrong_slave_address(void);
static void parse_register_request(const uint8_t* modbus_rx_buffer, uint16_t* reg_addr, uint16_t* reg_quantity);
static void construct_response_frame(const sensor_values_t* sensor_data);


/**
 * @brief Main Modbus polling function.
 *
 * Handles different Modbus states including waiting for frames, processing received frames,
 * and handling incorrect addresses. This function communicates with the sensor reader to
 * get updated values and constructs appropriate responses.
 *
 * @param sensor_data Pointer to the sensor values used in Modbus communication.
 */
void modbus_poll(sensor_values_t *sensor_data) {
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
                    	 read_all_sensors(sensor_data);       // Populate sensor values
                        sensor_values_ready = 1;  // Mark as updated
                    }

                    construct_response_frame(sensor_data);  // Send response
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

/**
* @brief Interrupt handler for USART2.
*
* Checks for available data and determines whether the address matches the configured slave address.
* Sets the Modbus flag accordingly to handle valid or wrong addresses.
*/
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

/**
 * @brief Handle wrong slave address by resetting the receiver.
 *
 * This function ensures that the receiver is properly reset when an incorrect slave address is received.
 * It also re-enables the RX interrupt for further communication.
 */
static void handle_wrong_slave_address(void) {
    USART2->CR1 &= ~0x00000004;  // Disable receiver
    delay_ms(10);                // Wait for erroneous address handling
    USART2->CR1 |= 0x00000004;   // Re-enable receiver
    USART2->CR1 |= 0x0020;       // Re-enable RX interrupt
    modbus_flag = MODBUS_WAITING;
}

/**
 * @brief Parse the register request from the received frame.
 *
 * Extracts the start address and the number of registers requested from the received Modbus frame.
 * Updates the register addresses and the quantity requested.
 *
 * @param modbus_rx_buffer Buffer containing the received frame.
 * @param reg_addr Array to store register addresses.
 * @param reg_quantity Pointer to store the quantity of registers requested.
 */
static void parse_register_request(const uint8_t* modbus_rx_buffer, uint16_t* reg_addr, uint16_t* reg_quantity) {
    uint16_t start_addr = (modbus_rx_buffer[2] << 8) | modbus_rx_buffer[3];
    *reg_quantity = (modbus_rx_buffer[4] << 8) | modbus_rx_buffer[5];

    if (*reg_quantity > MAX_REGISTERS) {
    	*reg_quantity = MAX_REGISTERS;  // Ensure quantity does not exceed the maximum allowed registers
    }

    // Assign register addresses
    for (uint8_t i = 0; i < *reg_quantity; i++) {
        reg_addr[i] = start_addr + i;
    }
}

/**
 * @brief Construct and send the Modbus response frame.
 *
 * This function constructs the response frame by populating it with sensor data and appending
 * the CRC for data integrity. Finally, the response is sent over USART2.
 *
 * @param sensor_data Pointer to the sensor values to include in the response.
 */
static void construct_response_frame(const sensor_values_t* sensor_data) {
    uint8_t response_length = RESPONSE_BYTE_COUNT + 5;  // Total response length (data + CRC + header)
    uint8_t response_frame[RESPONSE_BYTE_COUNT + 5] = {0};

    response_frame[0] = SLAVE_ADDR;       // Set slave address
    response_frame[1] = FUNCTION_CODE;    // Function code
    response_frame[2] = RESPONSE_BYTE_COUNT;  // Byte count

    // Fill the response with sensor values (high and low bytes)
    uint8_t index = 3;  // Start filling from the 4th byte

    // Populate response with battery temperature values
    response_frame[index++] = (sensor_data->battery_temperature >> 8) & 0xFF;  // High byte
    response_frame[index++] = sensor_data->battery_temperature & 0xFF;         // Low byte

    response_frame[index++] = (sensor_data->battery_temperature_alt >> 8) & 0xFF;  // High byte
    response_frame[index++] = sensor_data->battery_temperature_alt & 0xFF;         // Low byte

    // Populate response with cell voltages (4 cells)
    for (int i = 0; i < 4; i++) {
        response_frame[index++] = (sensor_data->cell_voltage[i] >> 8) & 0xFF;  // High byte
        response_frame[index++] = sensor_data->cell_voltage[i] & 0xFF;         // Low byte
    }

    // Populate response with battery current values
    response_frame[index++] = (sensor_data->battery_current_charge >> 8) & 0xFF;  // High byte
    response_frame[index++] = sensor_data->battery_current_charge & 0xFF;         // Low byte

    response_frame[index++] = (sensor_data->battery_current_discharge >> 8) & 0xFF;  // High byte
    response_frame[index++] = sensor_data->battery_current_discharge & 0xFF;         // Low byte

    // Note: Flame sensor value is omitted based on the request to drop this sensor.

    // Calculate and append CRC to the response
    uint16_t response_crc = CRC16(response_frame, response_length - 2);
    response_frame[response_length - 2] = response_crc & 0xFF;          // Low byte of CRC
    response_frame[response_length - 1] = (response_crc >> 8) & 0xFF;   // High byte of CRC

    // Send response frame over USART
	//    for (uint8_t i = 0; i < response_length; i++) {
	//        USART2_write(response_frame[i]);
	//    }

    // Send response frame over USART using DMA for efficiency
     USART2_send_data_dma(response_frame, response_length);
     delay_ms(10);
}


