#pragma once

#include "sensor_reader.h"

#include <stdint.h>
#include <stdbool.h>

// MOSFET Control States
typedef enum {
    MOSFET_OFF = 0,
    MOSFET_ON = 1
} mosfet_state_t;

typedef enum {
    STATE_LOW_VOLTAGE,
    STATE_NORMAL_VOLTAGE,
    STATE_HIGH_VOLTAGE
} voltage_state_t;

void reset_mosfet_control_logic(void);
void mosfet_init(void);
void mosfet_control_logic(sensor_values_t sensor_values);
float calculate_total_voltage(int16_t cell_voltages[], int num_cells);
bool is_charging();
bool is_discharging();
bool check_short_circuit(float current, uint32_t current_time);
bool check_overcurrent(float current, bool is_charging, bool is_discharging, uint32_t current_time);
bool check_body_diode_protection(float current, uint32_t current_time);
bool check_temperature_protection(float temperature, uint32_t current_time);
voltage_state_t check_voltage_state(sensor_values_t sensor_values);
void update_mosfet_states(voltage_state_t current_voltage_state, uint32_t current_time);
void set_charge_mosfet(mosfet_state_t state);
void set_discharge_mosfet(mosfet_state_t state);
void update_soc(float current, uint32_t current_time);
