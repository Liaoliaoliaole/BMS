#pragma once
#include "stm32l1xx.h"

/* MODBUS Configuration*/
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
#define CURRENT_CALIBRATION_GAIN 1.0f  // TODO Placeholder calibration gain
#define CURRENT_CALIBRATION_OFFSET 0.0f  //TODO Placeholder calibration offset

/* Mosfet Control Configuration*/
#define SHORT_CIRCUIT_THRESHOLD 50.0f          // Amperes
#define OVERCURRENT_THRESHOLD 9.0f             // Amperes
#define OVER_TEMPERATURE_THRESHOLD 45.0f       // Degrees Celsius
#define UNDER_TEMPERATURE_THRESHOLD -20.0f     // Degrees Celsius
#define LOW_VOLTAGE_THRESHOLD 2.8f             // Volts per cell
#define HIGH_VOLTAGE_THRESHOLD 3.2f            // Volts per cell
#define HIGH_CURRENT_THRESHOLD 1.0f            // Amperes (for body diode protection)
#ifdef TEST
#define DEBOUNCE_DELAY_MS 10                   // Milliseconds (reduced for testing)
#else
#define DEBOUNCE_DELAY_MS 1000                 // Milliseconds (normal operation)
#endif
