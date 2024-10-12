#include "test_mosfet.h"
#include "mosfet_control.h"
#include <stdio.h>
#include <assert.h>

// Mock Variables for GPIO state and USART outputs
static bool mock_charge_mosfet_state = false;
static bool mock_discharge_mosfet_state = false;
static char mock_usart_buffer[512];

// Mock Functions
void mock_set_charge_mosfet(bool state) {
    mock_charge_mosfet_state = state;
}

void mock_set_discharge_mosfet(bool state) {
    mock_discharge_mosfet_state = state;
}

void mock_USART2_send_string(const char* str) {
    // Store the last string sent to USART in the mock buffer for validation
    snprintf(mock_usart_buffer, sizeof(mock_usart_buffer), "%s", str);
}

// Test Functions
void run_mosfet_control_tests() {
    printf("Running MOSFET Control Logic Tests...\n");

    test_normal_operation();
    test_voltage_thresholds();
    test_current_thresholds();
    test_temperature_protection();
    test_state_transitions();

    printf("All Tests Passed Successfully!\n");
}

void test_normal_operation() {
    sensor_values_t sensor_values = {0};

    // Normal voltage and normal current
    sensor_values.cell_voltage[0] = 3000;  // 3.0V per cell
    sensor_values.cell_voltage[1] = 3000;
    sensor_values.cell_voltage[2] = 3000;
    sensor_values.cell_voltage[3] = 3000;
    sensor_values.battery_current_charge = 500;  // 0.5A (charging)
    sensor_values.battery_current_discharge = 0; // No discharging

    mock_set_charge_mosfet(false);
    mock_set_discharge_mosfet(false);
    reset_mosfet_control_logic();  // Ensure the initial state is consistent

    mosfet_control_logic(sensor_values);
    assert(mock_charge_mosfet_state == true);  // Charging MOSFET should be on
    assert(mock_discharge_mosfet_state == true); // Discharge MOSFET should also be on
    printf("Test Normal Operation: Passed\n");
}

void test_voltage_thresholds() {
    sensor_values_t sensor_values = {0};

    // Low voltage scenario
    sensor_values.cell_voltage[0] = 2700;  // 2.7V per cell (below threshold)
    sensor_values.cell_voltage[1] = 2700;
    sensor_values.cell_voltage[2] = 2700;
    sensor_values.cell_voltage[3] = 2700;
    sensor_values.battery_current_charge = 500;  // Charging
    sensor_values.battery_current_discharge = 0; // No discharging

    reset_mosfet_control_logic();

    mosfet_control_logic(sensor_values);
    assert(mock_charge_mosfet_state == true);  // Charge should be on
    assert(mock_discharge_mosfet_state == false); // Discharge should be off
    printf("Test Low Voltage: Passed\n");

    // High voltage scenario
    sensor_values.cell_voltage[0] = 3300;  // 3.3V per cell (above threshold)
    sensor_values.cell_voltage[1] = 3300;
    sensor_values.cell_voltage[2] = 3300;
    sensor_values.cell_voltage[3] = 3300;

    reset_mosfet_control_logic();

    mosfet_control_logic(sensor_values);
    assert(mock_charge_mosfet_state == false); // Charging should be disabled
    assert(mock_discharge_mosfet_state == true); // Discharging allowed
    printf("Test High Voltage: Passed\n");
}

void test_current_thresholds() {
    sensor_values_t sensor_values = {0};

    // Overcurrent charging
    sensor_values.cell_voltage[0] = 3000;
    sensor_values.battery_current_charge = 15000; // 15A (overcurrent)
    sensor_values.battery_current_discharge = 0;

    reset_mosfet_control_logic();

    mosfet_control_logic(sensor_values);
    assert(mock_charge_mosfet_state == false); // Charge MOSFET should be off
    printf("Test Overcurrent Charging: Passed\n");

    // High current, body diode protection
    sensor_values.battery_current_charge = 1200; // 1.2A (body diode threshold)
    reset_mosfet_control_logic();

    mosfet_control_logic(sensor_values);
    assert(mock_charge_mosfet_state == true);  // Both should be on for body diode
    assert(mock_discharge_mosfet_state == true);
    printf("Test Body Diode Protection: Passed\n");
}

void test_temperature_protection() {
    sensor_values_t sensor_values = {0};

    // Overtemperature scenario
    sensor_values.battery_temperature = 60; // Above threshold
    reset_mosfet_control_logic();

    mosfet_control_logic(sensor_values);
    assert(mock_charge_mosfet_state == false); // Both MOSFETs off
    assert(mock_discharge_mosfet_state == false);
    printf("Test Overtemperature Protection: Passed\n");

    // Undertemperature scenario
    sensor_values.battery_temperature = -25; // Below threshold
    reset_mosfet_control_logic();

    mosfet_control_logic(sensor_values);
    assert(mock_charge_mosfet_state == false); // Charging should be disabled
    printf("Test Undertemperature Protection: Passed\n");
}

void test_state_transitions() {
    sensor_values_t sensor_values = {0};

    // Test state transition with debounce
    mock_time_ms = 0;
    sensor_values.cell_voltage[0] = 3000;
    sensor_values.battery_current_charge = 500; // 0.5A charging
    reset_mosfet_control_logic();

    // Initial state, no debounce has passed
    mosfet_control_logic(sensor_values);
    assert(mock_charge_mosfet_state == true);  // Charge should be on
    assert(mock_discharge_mosfet_state == true); // Discharge should be on

    // Change to a low voltage, but no debounce yet
    mock_time_ms = 500; // Half debounce time
    sensor_values.cell_voltage[0] = 2700;
    mosfet_control_logic(sensor_values);
    assert(mock_charge_mosfet_state == true);  // Should maintain previous state
    assert(mock_discharge_mosfet_state == true);

    // Debounce passes
    mock_time_ms = 1500; // Beyond debounce delay
    mosfet_control_logic(sensor_values);
    assert(mock_charge_mosfet_state == true);  // Charge should be on
    assert(mock_discharge_mosfet_state == false); // Discharge should be off now
    printf("Test State Transitions: Passed\n");
}

int main() {
    run_mosfet_control_tests();
    return 0;
}
