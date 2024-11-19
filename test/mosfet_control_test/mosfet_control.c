#include "mosfet_control.h"
#include "bms_configuration.h"
#include <stdio.h>
#include <math.h>

// Static variables for maintaining states
static system_state_t previous_system_state = STATE_BOTH_OFF;
static mosfet_state_t previous_charge_mosfet_state = MOSFET_OFF;
static mosfet_state_t previous_discharge_mosfet_state = MOSFET_OFF;
// Static variables for maintaining states
static bool body_diode_fault_flag = false;
static bool immediate_action_required = false;

static uint8_t fault_bitmask = 0x00;
static time_t last_switch_time = 0;
time_t current_mock_time = 0;

// Mocked time function to simulate passage of time
time_t get_current_time_ms() {
    return current_mock_time;
}

void advance_time_ms(time_t ms) {
    current_mock_time += ms;
}

void clear_faults() {
    fault_bitmask = 0x00;
    body_diode_fault_flag = false;
    USART2_send_string("All faults cleared.\n\r");
}

void reset_mosfet_control_logic() {
    previous_charge_mosfet_state = MOSFET_OFF;
    previous_discharge_mosfet_state = MOSFET_OFF;
    last_switch_time = get_current_time_ms();
    clear_faults();
    // Synchronize hardware MOSFET states with software states
    set_charge_mosfet(MOSFET_OFF);
    set_discharge_mosfet(MOSFET_OFF);
}

// Main MOSFET control logic function
void mosfet_control_logic(sensor_values_t sensor_values) {
    time_t current_time = get_current_time_ms();
    bool debounce_passed = (current_time - last_switch_time) >= DEBOUNCE_DELAY_MS;

    printf("Debounce Passed: %d\n", debounce_passed);

    // Use the higher of charge or discharge current for protections
    float current_max = fmaxf(sensor_values.battery_current_charge, sensor_values.battery_current_discharge);
    float temperature_max = fmaxf(sensor_values.battery_temperature, sensor_values.battery_temperature_alt);
    float temperature_min = fminf(sensor_values.battery_temperature, sensor_values.battery_temperature_alt);

    // Update fault flags based on sensor values
    check_body_diode_protection(current_max); // Body diode protection first priority
    check_overcurrent(current_max);
    check_over_temperature_protection(temperature_max);
    check_low_temperature_protection(temperature_min);
    check_voltage_protection(sensor_values.cell_voltage);

    // Determine system state based on all available sensor data
    system_state_t current_system_state = determine_system_state();

    // Update MOSFET states if debounce has passed and there are no active faults
    if (immediate_action_required) {
        update_mosfet_states(current_system_state, current_time);
        previous_system_state = current_system_state;
        immediate_action_required = false; // Reset the flag
        last_switch_time = current_time;   // Update last switch time
    } else if (debounce_passed) {
        if (current_system_state != previous_system_state) {
            update_mosfet_states(current_system_state, current_time);
            previous_system_state = current_system_state;
            last_switch_time = current_time;
        }
    } else {
        // Debounce not passed, revert to previous MOSFET states
        if (previous_charge_mosfet_state != charge_mosfet_pin_state) {
            set_charge_mosfet(previous_charge_mosfet_state);
        }
        if (previous_discharge_mosfet_state != discharge_mosfet_pin_state) {
            set_discharge_mosfet(previous_discharge_mosfet_state);
        }
    }
}

// Interrupt service routine for short-circuit protection
void short_circuit_isr() {
    // Set the fault bit for short-circuit detection immediately
    fault_bitmask |= (1 << 0);
    USART2_send_string("Short-circuit detected. Setting fault flag via interrupt.\n\r");

    // Immediately turn off both MOSFETs for safety
    set_charge_mosfet(MOSFET_OFF);
    set_discharge_mosfet(MOSFET_OFF);
    previous_charge_mosfet_state = MOSFET_OFF;
    previous_discharge_mosfet_state = MOSFET_OFF;
}

// Check for body diode protection
void check_body_diode_protection(float current) {
    if (current >= HIGH_CURRENT_THRESHOLD) {
        if (!body_diode_fault_flag) {
            body_diode_fault_flag = true;
            immediate_action_required = true;
            USART2_send_string("High current detected. Body diode protection activated. Turning on both MOSFETs to protect diode.\n\r");
        }
    } else {
        if (body_diode_fault_flag) {
            body_diode_fault_flag = false;
            immediate_action_required = true;
            USART2_send_string("Current normal. Body diode protection deactivated.\n\r");
        }
    }
}

// Check for overcurrent condition
void check_overcurrent(float current_max) {
    if (current_max >= OVERCURRENT_THRESHOLD) {
        fault_bitmask |= (1 << 1);
        USART2_send_string("Overcurrent detected. Setting fault flag.\n\r");
    } else {
        fault_bitmask &= ~(1 << 1);
    }
}

// Check for temperature protection, updated to handle charge and discharge ranges separately
void check_over_temperature_protection(float temperature) {
    if (temperature > DISCHARGE_OVER_TEMPERATURE_THRESHOLD) {
        fault_bitmask |= (1 << 2);
        USART2_send_string("Temperature above 40°C. Setting fault flag.\n\r");
    } else {
        fault_bitmask &= ~(1 << 2);
    }
    if (temperature > CHARGE_OVER_TEMPERATURE_THRESHOLD && temperature <= DISCHARGE_OVER_TEMPERATURE_THRESHOLD) {
        fault_bitmask |= (1 << 3);
        USART2_send_string("Temperature between 30 C and 40 C. Discharge only allowed.\n\r");
    } else {
        fault_bitmask &= ~(1 << 3);
    }
}

void check_low_temperature_protection(float temperature) {
    if (temperature < DISCHARGE_UNDER_TEMPERATURE_THRESHOLD) {
        fault_bitmask |= (1 << 4);
        USART2_send_string("Temperature below -20 C. Setting fault flag.\n\r");
    } else {
        fault_bitmask &= ~(1 << 4);
    }
    if (temperature >= DISCHARGE_UNDER_TEMPERATURE_THRESHOLD && temperature < CHARGE_UNDER_TEMPERATURE_THRESHOLD) {
        fault_bitmask |= (1 << 5);
        USART2_send_string("Temperature between -20 C and 5 C. Discharge only allowed.\n\r");
    } else {
        fault_bitmask &= ~(1 << 5);
    }
}

// Check for voltage protection
void check_voltage_protection(int16_t cell_voltage[4]) {
    bool overvoltage = false;
    bool undervoltage = false;

    for (int i = 0; i < 4; i++) {
        float voltage = cell_voltage[i]; // Convert mV to V
        if (voltage >= HIGH_VOLTAGE_THRESHOLD) {
            overvoltage = true;
        }
        if (voltage <= LOW_VOLTAGE_THRESHOLD) {
            undervoltage = true;
        }
    }

    if (overvoltage) {
        fault_bitmask |= (1 << 6);
        USART2_send_string("Overvoltage detected. Setting fault flag.\n\r");
    } else {
        fault_bitmask &= ~(1 << 6); 
    }

    if (undervoltage) {
        fault_bitmask |= (1 << 7); 
        USART2_send_string("Undervoltage detected. Setting fault flag.\n\r");
    } else {
        fault_bitmask &= ~(1 << 7);
    }
}

system_state_t determine_system_state() {
    // Critical faults - require both MOSFETs off
    if (fault_bitmask & ((1 << 1) | (1 << 2) | (1 << 4))) {
        return STATE_BOTH_OFF; // Overcurrent, over temperature (charge), or very low temperature
    }

    // Body diode protection should have the highest priority - both MOSFETs off
    if (body_diode_fault_flag) {
        return STATE_NORMAL_OPERATION;
    }

    // Discharge-only conditions
    if (fault_bitmask & ((1 << 3) | (1 << 5))) {
        return STATE_DISCHARGE_ONLY; // High temperature (discharge), low temperature (discharge), or overvoltage
    }

    // Charge-only conditions
    if (fault_bitmask & (1 << 7)) {
        return STATE_CHARGE_ONLY; // Undervoltage condition
    }

    if (fault_bitmask & (1 << 6)) {
        return STATE_DISCHARGE_ONLY;
    }

    // No faults detected, normal operation
    return STATE_NORMAL_OPERATION;
}

// Update MOSFET states based on the overall system state
void update_mosfet_states(system_state_t system_state, uint32_t current_time) {
    switch (system_state) {
        case STATE_NORMAL_OPERATION:
            set_charge_mosfet(MOSFET_ON);
            set_discharge_mosfet(MOSFET_ON);
            USART2_send_string("NORMAL_OPERATION: Charge ON, Discharge ON.\n\r");
            break;

        case STATE_CHARGE_ONLY:
            set_charge_mosfet(MOSFET_ON);
            set_discharge_mosfet(MOSFET_OFF);
            USART2_send_string("CHARGE_ONLY: Charge ON, Discharge OFF.\n\r");
            break;

        case STATE_DISCHARGE_ONLY:
            set_charge_mosfet(MOSFET_OFF);
            set_discharge_mosfet(MOSFET_ON);
            USART2_send_string("DISCHARGE_ONLY: Charge OFF, Discharge ON.\n\r");
            break;

        case STATE_BOTH_OFF:
            set_charge_mosfet(MOSFET_OFF);
            set_discharge_mosfet(MOSFET_OFF);
            USART2_send_string("BOTH_OFF: Charge OFF, Discharge OFF.\n\r");
            break;

        default:
            break;
    }

    previous_charge_mosfet_state = (system_state == STATE_NORMAL_OPERATION || system_state == STATE_CHARGE_ONLY) ? MOSFET_ON : MOSFET_OFF;
    previous_discharge_mosfet_state = (system_state == STATE_NORMAL_OPERATION || system_state == STATE_DISCHARGE_ONLY) ? MOSFET_ON : MOSFET_OFF;
    last_switch_time = current_time;
}

// Mock for GPIOB->BSRR
void set_charge_mosfet(mosfet_state_t state) {
    if (state == MOSFET_ON) {
        charge_mosfet_pin_state = 1;
        printf("Setting Charge MOSFET: ON (Internal state: %d)\n", charge_mosfet_pin_state);
    } else {
        charge_mosfet_pin_state = 0;
        printf("Setting Charge MOSFET: OFF (Internal state: %d)\n", charge_mosfet_pin_state);
    }
}

void set_discharge_mosfet(mosfet_state_t state) {
    if (state == MOSFET_ON) {
        discharge_mosfet_pin_state = 1;
        printf("Setting Discharge MOSFET: ON\n");
    } else {
        discharge_mosfet_pin_state = 0;
        printf("Setting Discharge MOSFET: OFF\n");
    }
}