#pragma once

#include <stdint.h>
#include <stdbool.h>
#include <time.h>

// Test use
// Mock GPIO values for testing
#define USART2_send_string(str) printf(str)
// Extern declarations for MOSFET states
extern int charge_mosfet_pin_state;
extern int discharge_mosfet_pin_state;

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

// Structure to hold sensor values
typedef struct {
    int16_t battery_temperature;     // Battery temperature value
    int16_t battery_temperature_alt; // Alternate temperature value for differential readings
    int16_t cell_voltage[4];         // Cell voltage readings for 4 cells
    int16_t battery_current_charge;  // Battery current value (charge)
    int16_t battery_current_discharge;        // Battery current value (discharge)
    int16_t flame_sensor;            // Flame sensor reading
} sensor_values_t;

// TEST use
time_t get_current_time_ms();
void advance_time_ms(time_t ms);

void reset_mosfet_control_logic();
void clear_faults();
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

