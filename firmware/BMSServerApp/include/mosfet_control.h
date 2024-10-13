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
    STATE_NORMAL_OPERATION,   // Charging and discharging allowed
    STATE_CHARGE_ONLY,        // Only charging allowed (e.g., low voltage)
    STATE_DISCHARGE_ONLY,     // Only discharging allowed (e.g., high voltage)
    STATE_BOTH_OFF,           // Both MOSFETs are off (e.g., critical fault condition)
} system_state_t;

void reset_mosfet_control_logic();
void mosfet_init(void);
void mosfet_control_logic(sensor_values_t sensor_values);
void short_circuit_isr();
void check_overcurrent(float current_max, time_t current_time);
void check_body_diode_protection(float current, time_t current_time);
void check_over_temperature_protection(float temperature, time_t current_time);
void check_low_temperature_protection(float temperature, time_t current_time);
void check_voltage_protection(int16_t cell_voltage[4]);
system_state_t determine_system_state();
void update_mosfet_states(system_state_t system_state, uint32_t current_time);
void set_charge_mosfet(mosfet_state_t state);
void set_discharge_mosfet(mosfet_state_t state);
