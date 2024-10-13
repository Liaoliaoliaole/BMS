#include "bms_configuration.h"
#include "mosfet_control.h"

#include <assert.h>
#include <stdio.h>

// Mock mosfet control state globally
int charge_mosfet_pin_state = 0;
int discharge_mosfet_pin_state = 0;

sensor_values_t sensor_values;

// Function to initialize sensor values to default states
void initialize_sensor_values() {
    sensor_values.battery_temperature = 25;
    sensor_values.battery_temperature_alt = 25;
    sensor_values.battery_current_charge = 1;
    sensor_values.battery_current_discharge = 1;
    sensor_values.flame_sensor = 0;
    for (int i = 0; i < 4; i++) {
        sensor_values.cell_voltage[i] = 3;  // Set default voltage to 3.0V per cell
    }
}

void test_mosfet_state_control_with_sensor_values() {
    printf("##### Test Case 1: MOSFET State Control with Sensor Values #####\n");
    
    reset_mosfet_control_logic();
    //initialize_sensor_values();
    
    // Set initial safe conditions
    sensor_values.battery_temperature = 25;
    sensor_values.battery_temperature_alt = 25;
    sensor_values.battery_current_charge = 5;  
    sensor_values.battery_current_discharge = 0; 
    for (int i = 0; i < 4; i++) {
        sensor_values.cell_voltage[i] = 3;
    }

    // Test Normal Operation
    mosfet_control_logic(sensor_values);
    advance_time_ms(DEBOUNCE_DELAY_MS + 1);
    mosfet_control_logic(sensor_values);
    assert(charge_mosfet_pin_state == 1);
    assert(discharge_mosfet_pin_state == 1);
    printf("#####\n\r");

    // Set low temperature to trigger protection (below -20 degrees Celsius)
    sensor_values.battery_temperature = -25;
    sensor_values.battery_temperature_alt = -24;
    mosfet_control_logic(sensor_values);
    advance_time_ms(DEBOUNCE_DELAY_MS + 1);
    mosfet_control_logic(sensor_values);
    assert(charge_mosfet_pin_state == 0);
    assert(discharge_mosfet_pin_state == 0);
    printf("#####\n\r");

    // Reset temperature to safe level and verify MOSFETs turn on again
    sensor_values.battery_temperature = 25;
    sensor_values.battery_temperature_alt = 20;
    mosfet_control_logic(sensor_values);
    advance_time_ms(DEBOUNCE_DELAY_MS + 1);
    mosfet_control_logic(sensor_values);
    assert(charge_mosfet_pin_state == 1);
    assert(discharge_mosfet_pin_state == 1);

    // Set overcurrent condition to trigger protection (above 9A)
    sensor_values.battery_current_charge = 10;  // 10A, exceeding overcurrent threshold
    mosfet_control_logic(sensor_values);
    advance_time_ms(DEBOUNCE_DELAY_MS + 1);
    mosfet_control_logic(sensor_values);
    assert(charge_mosfet_pin_state == 0);
    assert(discharge_mosfet_pin_state == 0);

    // Reset current to normal and verify MOSFETs turn on again
    sensor_values.battery_current_charge = 5;  // 0.5A, back to normal
    mosfet_control_logic(sensor_values);
    advance_time_ms(DEBOUNCE_DELAY_MS + 1);
    mosfet_control_logic(sensor_values);
    assert(charge_mosfet_pin_state == 1);
    assert(discharge_mosfet_pin_state == 1);

    // Set cell voltage to a high value to trigger high voltage protection (above 3.2V)
    sensor_values.battery_current_charge = 1;
    sensor_values.battery_current_discharge = 1;
    for (int i = 0; i < 4; i++) {
        sensor_values.cell_voltage[i] = 4;  // 3.3V, exceeding the high voltage threshold
    }
    mosfet_control_logic(sensor_values);
    advance_time_ms(DEBOUNCE_DELAY_MS + 1);
    mosfet_control_logic(sensor_values);
    assert(charge_mosfet_pin_state == 0);  // Charging should be disabled due to high voltage
    assert(discharge_mosfet_pin_state == 1);  // Discharging should still be allowed

    // Reset voltage to normal and verify MOSFETs return to normal operation
    for (int i = 0; i < 4; i++) {
        sensor_values.cell_voltage[i] = 3;  // Back to 3.0V per cell
    }
    mosfet_control_logic(sensor_values);
    advance_time_ms(DEBOUNCE_DELAY_MS + 1);
    mosfet_control_logic(sensor_values);
    assert(charge_mosfet_pin_state == 1);
    assert(discharge_mosfet_pin_state == 1);

    printf("##### Test Case 12 Passed #####\n\n");
}

int main() {
    test_mosfet_state_control_with_sensor_values();
    return 0;
}
