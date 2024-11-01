#pragma once

#include <stdint.h>
#include "sensor_reader.h"

#define MODBUS_WAITING 0
#define MODBUS_FRAME_RECEIVED 1
#define MODBUS_WRONG_ADDRESS 2

/**
 * @brief Main Modbus polling function.
 *
 * This function handles Modbus state changes, receiving frames, validating CRC,
 * and sending responses. It processes incoming Modbus frames, validates them,
 * and sends the requested sensor data in response.
 *
 * @param sensor_data Pointer to the sensor values used in Modbus communication.
 */
void modbus_poll(sensor_values_t *sensor_data);

/**
 * @brief USART2 interrupt handler for Modbus communication.
 *
 * This interrupt handler is triggered when USART2 receives data. It determines whether
 * the received address is valid for this device and sets the Modbus state accordingly.
 */
void USART2_IRQHandler(void);
