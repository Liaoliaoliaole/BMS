#pragma once
#include "stm32l1xx.h"

/* ADC Channel Definitions */
// Pin configuration for ADC channels:
// PA0 (A0)  - Multiplexer Voltage Measurement - ADC Channel 0
// PA1 (A1)  - Temperature Sensor 1           - ADC Channel 1
// PA4 (A2)  - Temperature Sensor 2           - ADC Channel 4
// PA5 (A3)  - Shunt Resistor Current Measurement - ADC Channel 5
// PA6 (A5)  - Flame Sensor                   - ADC Channel 6
// PC1 (A4)  - Future Use (e.g., Current Measurement) - ADC Channel 11

/* MODBUS Configuration*/
#define MODBUS_WAITING 0
#define MODBUS_FRAME_RECEIVED 1
#define MODBUS_WRONG_ADDRESS 2

#define SLAVE_ADDR 0x01
#define FUNCTION_CODE 0x04
#define START_ADDR 0x00
#define NUM_REGISTERS 7
#define RESPONSE_BYTE_COUNT (NUM_REGISTERS*2) // 2 bytes per register
#define MAX_REGISTERS 10
#define BUFFER_SIZE 20

/*ADC Convert configuration*/
#define ADC_MAX_VALUE 4095.0 //12-bit ADC
#define REF_VOLT_ADC 3.3   // ADC reference voltage
#define GAIN 35.1 // 3.3/(9.4*0.01)
#define R_SHUNT 0.01

/* Mosfet Control Configuration*/
#define SHORT_CIRCUIT_THRESHOLD 50.0f          // Amperes
#define OVERCURRENT_THRESHOLD 9.0f             // Amperes
#define OVER_TEMPERATURE_THRESHOLD 50.0f       // Degrees Celsius
#define UNDER_TEMPERATURE_THRESHOLD -20.0f     // Degrees Celsius
#define LOW_VOLTAGE_THRESHOLD 2.8f             // Volts per cell
#define HIGH_VOLTAGE_THRESHOLD 3.2f            // Volts per cell
#define HIGH_CURRENT_THRESHOLD 1.1f            // Amperes (for body diode protection)
#define LOW_CURRENT_THRESHOLD 0.9f             // Amperes (for hysteresis)
#ifdef TEST
#define DEBOUNCE_DELAY_MS 10                   // Milliseconds (reduced for testing)
#else
#define DEBOUNCE_DELAY_MS 1000                 // Milliseconds (normal operation)
#endif

