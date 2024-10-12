#pragma once

#include <stdint.h>
#include <stdbool.h>
// Structure to hold sensor values
typedef struct {
    int16_t battery_temperature;     // Battery temperature value
    int16_t battery_temperature_alt; // Alternate temperature value for differential readings
    int16_t cell_voltage[4];         // Cell voltage readings for 4 cells
    int16_t battery_current_charge;  // Battery current value (charge)
    int16_t battery_current_discharge;        // Battery current value (discharge)
    int16_t flame_sensor;            // Flame sensor reading
} sensor_values_t;

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
static float calculate_total_voltage(int16_t cell_voltages[], int num_cells);
static bool is_charging(float charge_current);
static bool is_discharging(float discharge_current);
static bool check_short_circuit(float current, uint32_t current_time);
static bool check_overcurrent(float current, bool is_charging, bool is_discharging, uint32_t current_time);
static bool check_body_diode_protection(float current, uint32_t current_time);
static bool check_temperature_protection(float temperature, uint32_t current_time);
static voltage_state_t check_voltage_state(sensor_values_t sensor_values);
static void update_mosfet_states(voltage_state_t current_voltage_state, uint32_t current_time);
void set_charge_mosfet(mosfet_state_t state);
void set_discharge_mosfet(mosfet_state_t state);
