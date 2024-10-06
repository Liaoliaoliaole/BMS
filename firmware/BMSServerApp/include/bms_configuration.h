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
#define GAIN 33.0 // TODO not actual value
#define R_SHUNT 0.01
