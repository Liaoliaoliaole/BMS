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
    printf("##### Test MOSFET State Control with Sensor Values #####\n");
    
    reset_mosfet_control_logic();
    clear_faults();
    initialize_sensor_values();
    
    // Temperature Testing for Charging and Discharging
    int temperatures[] = {-35, -15, 20, 35, 45};
    for (int i = 0; i < 5; i++) {
        reset_mosfet_control_logic();
        clear_faults();
        initialize_sensor_values();
        sensor_values.battery_temperature = temperatures[i];
        sensor_values.battery_temperature_alt = temperatures[i];
        sensor_values.battery_current_charge = 1;
        sensor_values.battery_current_discharge = 1;
        for (int j = 0; j < 4; j++) {
            sensor_values.cell_voltage[j] = 3;  // Normal voltage range
        }
        printf("##### Test Case 1: Temperature %d degree #####\n", temperatures[i]);
        mosfet_control_logic(sensor_values);
        advance_time_ms(DEBOUNCE_DELAY_MS + 1);
        mosfet_control_logic(sensor_values);
        if (temperatures[i] == -35 || temperatures[i] == 45) {
            assert(charge_mosfet_pin_state == 0);
            assert(discharge_mosfet_pin_state == 0);
            printf("BOTH_OFF: Charge OFF, Discharge OFF\n");
        } else if (temperatures[i] == -15) {
            assert(charge_mosfet_pin_state == 0);
            assert(discharge_mosfet_pin_state == 1);
            printf("DISCHARGE_ONLY: Charge OFF, Discharge ON\n");
        } else if (temperatures[i] == 20) {
            assert(charge_mosfet_pin_state == 1);
            assert(discharge_mosfet_pin_state == 1);
            printf("NORMAL_OPERATION: Charge ON, Discharge ON\n");
        } else if (temperatures[i] == 35) {
            assert(charge_mosfet_pin_state == 0);
            assert(discharge_mosfet_pin_state == 1);
            printf("DISCHARGE_ONLY: Charge OFF, Discharge ON\n");
        }
    }

    // Overcurrent Testing for Normal Temperature Range
    printf("##### Test Case 2 :Overcurrent Testing for Normal Temperature Rang #####\n");
    float currents[] = {1.2, 10.0, 55.0};
    for (int i = 0; i < 3; i++) {
        reset_mosfet_control_logic();
        clear_faults();
        initialize_sensor_values();

        float current = currents[i];
        printf("##### Test Case 2: Overcurrent %f A #####\n", current);
        sensor_values.battery_current_charge = current;
        sensor_values.battery_current_discharge = 0;
        mosfet_control_logic(sensor_values);
        advance_time_ms(DEBOUNCE_DELAY_MS + 1);
        mosfet_control_logic(sensor_values);
        if (current >= OVERCURRENT_THRESHOLD) {
            assert(charge_mosfet_pin_state == 0);
            assert(discharge_mosfet_pin_state == 0);
            printf("OVERCURRENT: Charge OFF, Discharge OFF\n");
        } else {
            assert(charge_mosfet_pin_state == 1);
            assert(discharge_mosfet_pin_state == 1);
            printf("NORMAL_OPERATION: Charge ON, Discharge ON\n");
        }
    }

    // Voltage Protection Testing
    printf("##### Test Case 3: Voltage Protection #####\n");
    float voltages[] = {2, 4, 3};
    for (int i = 0; i < 3; i++) {
        reset_mosfet_control_logic();
        clear_faults();
        initialize_sensor_values();

        float voltage = voltages[i];
        for (int j = 0; j < 4; j++) {
            sensor_values.cell_voltage[j] = voltage;
        }
        mosfet_control_logic(sensor_values);
        advance_time_ms(DEBOUNCE_DELAY_MS + 1);
        mosfet_control_logic(sensor_values);
        if (voltage < LOW_VOLTAGE_THRESHOLD) {
            assert(charge_mosfet_pin_state == 1);
            assert(discharge_mosfet_pin_state == 0);
            printf("#3.1 UNDERVOLTAGE: Charge ON, Discharge OFF\n");
        } else if (voltage > HIGH_VOLTAGE_THRESHOLD) {
            assert(charge_mosfet_pin_state == 0);
            assert(discharge_mosfet_pin_state == 1);
            printf("#3.2 OVERVOLTAGE: Charge OFF, Discharge ON\n");
        } else {
            assert(charge_mosfet_pin_state == 1);
            assert(discharge_mosfet_pin_state == 1);
            printf("#3.3 VOLTAGE_NORMAL: Charge ON, Discharge ON\n");
        }
    }

    // Body Diode Protection Testing
    printf("##### Test Case 4: Body Diode Protection #####\n");
    reset_mosfet_control_logic();
    clear_faults();
    initialize_sensor_values();
    sensor_values.battery_temperature = 25;
    sensor_values.battery_temperature_alt = 25;
    sensor_values.battery_current_charge = 0;
    sensor_values.battery_current_discharge = 2;  // Above HIGH_CURRENT_THRESHOLD
    mosfet_control_logic(sensor_values);
    advance_time_ms(DEBOUNCE_DELAY_MS + 1);
    mosfet_control_logic(sensor_values);
    assert(charge_mosfet_pin_state == 1);
    assert(discharge_mosfet_pin_state == 1);
    printf("# 4.1 BODY_DIODE_PROTECTION_TRIGGERED: Charge ON, Discharge ON\n");

    // Test when body diode protection condition is cleared
    printf("##### Test Case 5: Body Diode Protection Cleared #####\n");
    sensor_values.battery_current_discharge = 0;  // Drop current below threshold
    mosfet_control_logic(sensor_values);
    advance_time_ms(DEBOUNCE_DELAY_MS + 1);
    mosfet_control_logic(sensor_values);
    assert(charge_mosfet_pin_state == 1);
    assert(discharge_mosfet_pin_state == 1);
    printf("#4.2 BODY_DIODE_PROTECTION_CLEARED: Charge ON, Discharge ON\n");

    printf("##### All Test Cases Passed #####\n\n");
}

int main() {
    test_mosfet_state_control_with_sensor_values();
    return 0;
}
