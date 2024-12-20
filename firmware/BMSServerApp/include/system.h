#pragma once
#include "stm32l1xx.h"
#include "sensor_reader.h"

#define HSI_VALUE    ((uint32_t)16000000)

/* Mosfet PIN setting */
//PB0 (Charge MOSFET) and PB1 (Discharge MOSFET) as output
#define CHARGE_MOSFET_PIN 1  // PB1 for Charge MOSFET
#define DISCHARGE_MOSFET_PIN 2  // PB2 for Discharge MOSFET

/* MUX PIN setting */
// Battery Voltage Measurement Multiplexer 1
#define MUX_BATT1_SELECT_PIN1 (1U << 3)  // PB3
#define MUX_BATT1_SELECT_PIN2 (1U << 4)  // PB4
#define MUX_BATT1_SELECT_PIN3 (1U << 5)  // PB5
#define MUX_BATT1_SELECT_PIN_MASK (MUX_BATT1_SELECT_PIN1 | MUX_BATT1_SELECT_PIN2 | MUX_BATT1_SELECT_PIN3)  // Mask for all Battery 1 MUX pins

// Battery Voltage Measurement Multiplexer 2
#define MUX_BATT2_SELECT_PIN1 (1U << 6)  // PB6
#define MUX_BATT2_SELECT_PIN2 (1U << 7)  // PB7
#define MUX_BATT2_SELECT_PIN3 (1U << 8)  // PB8
#define MUX_BATT2_SELECT_PIN_MASK (MUX_BATT2_SELECT_PIN1 | MUX_BATT2_SELECT_PIN2 | MUX_BATT2_SELECT_PIN3)  // Mask for all Battery 2 MUX pins

// Temperature Measurement Multiplexer
#define MUX_TEMP_SELECT_PIN1 (1U << 9)   // PB9
#define MUX_TEMP_SELECT_PIN2 (1U << 10)  // PB10
#define MUX_TEMP_SELECT_PIN3 (1U << 11)  // PB11
#define MUX_TEMP_SELECT_PIN_MASK (MUX_TEMP_SELECT_PIN1 | MUX_TEMP_SELECT_PIN2 | MUX_TEMP_SELECT_PIN3)  // Mask for all Temperature MUX pins

// Current Measurement Multiplexer
#define MUX_CURR_SELECT_PIN1 (1U << 12)  // PB12
#define MUX_CURR_SELECT_PIN2 (1U << 13)  // PB13
#define MUX_CURR_SELECT_PIN3 (1U << 14)  // PB14
#define MUX_CURR_SELECT_PIN_MASK (MUX_CURR_SELECT_PIN1 | MUX_CURR_SELECT_PIN2 | MUX_CURR_SELECT_PIN3)  // Mask for all Current MUX pins

// Common Bit position masks for all multiplexers
#define MUX_MODER_MASK ((0x3 << 6) | (0x3 << 8) | (0x3 << 10) | (0x3 << 12) | (0x3 << 14) | (0x3 << 16) | (0x3 << 18) | (0x3 << 20) | (0x3 << 22) | (0x3 << 24) | (0x3 << 26) | (0x3 << 28)) // MODER bits for PB3 to PB14
#define MUX_OTYPER_MASK (MUX_BATT1_SELECT_PIN_MASK | MUX_BATT2_SELECT_PIN_MASK | MUX_TEMP_SELECT_PIN_MASK | MUX_CURR_SELECT_PIN_MASK)  // OTYPER bits for PB3 to PB14
#define MUX_OSPEEDR_MASK ((0x3 << 6) | (0x3 << 8) | (0x3 << 10) | (0x3 << 12) | (0x3 << 14) | (0x3 << 16) | (0x3 << 18) | (0x3 << 20) | (0x3 << 22) | (0x3 << 24) | (0x3 << 26) | (0x3 << 28))  // OSPEEDR bits for PB3 to PB14
#define MUX_PUPDR_MASK ((0x3 << 6) | (0x3 << 8) | (0x3 << 10) | (0x3 << 12) | (0x3 << 14) | (0x3 << 16) | (0x3 << 18) | (0x3 << 20) | (0x3 << 22) | (0x3 << 24) | (0x3 << 26) | (0x3 << 28))   // PUPDR bits for PB3 to PB14

#define MUX_SELECT_PIN_MASK (MUX_SELECT_PIN1 | MUX_SELECT_PIN2 | MUX_SELECT_PIN3)  // Mask for all MUX pins

/* ADC Channel Definitions */
// Pin configuration for ADC channels:
#define ADC_CHANNEL_VOLTAGE_MEASUREMENT_1 		0  // PA0 (A0)  - Multiplexer Voltage Measurement - ADC Channel 0
#define ADC_CHANNEL_VOLTAGE_MEASUREMENT_2       1  // PA1 (A1)  - Multiplexer Voltage Measurement - ADC Channel 1
#define ADC_CHANNEL_TEMP_SENSOR       			4  // PA4 (A2)  - Temperature Sensor              - ADC Channel 4
#define ADC_CHANNEL_SHUNT_CURRENT       		5  // PA5 (A3)  - Shunt Resistor Current Measurement - ADC Channel 5
#define ADC_CHANNEL_FLAME_SENSOR        		6  // PA6 (A5)  - Flame Sensor                   - ADC Channel 6
#define ADC_CHANNEL_FUTURE_USE         			11  // PC1 (A4)  - Future Use (e.g., Current Measurement) - ADC Channel 11

extern uint8_t system_in_deep_sleep_mode; // 0: Awake, 1: Deep Sleep

void enter_deep_sleep(void);
void exit_deep_sleep(void);
void check_deep_sleep_condition(sensor_values_t* sensor_values);
