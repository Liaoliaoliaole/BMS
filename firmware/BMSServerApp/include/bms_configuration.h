#pragma once
#include "stm32l1xx.h"

/* MODBUS Configuration */
#define SLAVE_ADDR 0x01 // Modbus slave address for the device
#define FUNCTION_CODE 0x04 // Modbus function code to read input registers
#define START_ADDR 0x00 // Starting address of the register to read
#define NUM_REGISTERS 8 // Number of registers to read in a single request
#define RESPONSE_BYTE_COUNT (NUM_REGISTERS * 2) // Number of bytes in the response (2 bytes per register)
#define MAX_REGISTERS 10 // Maximum number of registers that can be handled
#define BUFFER_SIZE 20 // Buffer size for Modbus communication

/* ADC Convert Configuration */
#define ADC_MAX_VALUE 4095 // Maximum value for a 12-bit ADC (2^12 - 1)
#define REF_VOLT_ADC 3300  // Reference voltage for the ADC in millivolts
#define GAIN 351 // Amplifier gain multiplied by 10 to retain precision without using float (represents 35.1)
#define R_SHUNT 10 // Shunt resistance value in milliohms (0.01 ohms = 10 milliohms)
#define CURRENT_CALIBRATION_GAIN 993 // Calibration gain for current calculation (scaled to avoid floating-point operations)
#define CURRENT_CALIBRATION_OFFSET -5 // Calibration offset for current in milliamperes (scaled to avoid floating-point operations)

/* Mosfet Control Configuration */
#define SHORT_CIRCUIT_THRESHOLD 50000 // Threshold for short-circuit detection in milliamperes
#define OVERCURRENT_THRESHOLD 9000 // Threshold for overcurrent detection in milliamperes
#define OVER_TEMPERATURE_THRESHOLD 4500 // Over-temperature threshold in hundredths of degrees Celsius (e.g., 4500 = 45.00°C)
#define UNDER_TEMPERATURE_THRESHOLD -2000 // Under-temperature threshold in hundredths of degrees Celsius (e.g., -2000 = -20.00°C)
#define LOW_VOLTAGE_THRESHOLD 2800 // Low voltage threshold per cell in millivolts
#define HIGH_VOLTAGE_THRESHOLD 3200 // High voltage threshold per cell in millivolts
#define HIGH_CURRENT_THRESHOLD 1000 // Threshold for high current in milliamperes (for body diode protection)
#define DEEP_SLEEP_TEMP_THRESHOLD -4000 // -40.00°C in hundredths of a degree
#define WAKE_UP_TEMP_THRESHOLD -2000    // -20.00°C in hundredths of a degree

#ifdef TEST
#define DEBOUNCE_DELAY_MS 10 // Debounce delay in milliseconds for testing (reduced to speed up testing)
#else
#define DEBOUNCE_DELAY_MS 1000 // Debounce delay in milliseconds for normal operation
#endif
