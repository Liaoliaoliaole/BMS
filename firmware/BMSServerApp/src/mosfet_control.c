#include "stm32l1xx.h"
#include "bms_configuration.h"
#include "system.h"
#include "utils.h"
#include "usart2.h"
#include "mosfet_control.h"

#include <time.h>
#include <math.h>

int charge_mosfet_pin_state = 0;
int discharge_mosfet_pin_state = 0;

static system_state_t previous_system_state = STATE_BOTH_OFF;
static mosfet_state_t previous_charge_mosfet_state = MOSFET_OFF;
static mosfet_state_t previous_discharge_mosfet_state = MOSFET_OFF;

static uint8_t fault_bitmask = 0x00;
static time_t last_switch_time = 0;
time_t current_mock_time = 0;

void reset_mosfet_control_logic() {
    previous_charge_mosfet_state = MOSFET_OFF;
    previous_discharge_mosfet_state = MOSFET_OFF;

    fault_bitmask = 0x00;

    last_switch_time = get_current_time_ms();

    set_charge_mosfet(MOSFET_OFF);
    set_discharge_mosfet(MOSFET_OFF);
}

// Initialize MOSFET GPIO pins
void mosfet_init(void) {
    // Enable GPIO Port Clock
    RCC->AHBENR |= RCC_AHBENR_GPIOBEN;

    // Set CHARGE_MOSFET_PIN and DISCHARGE_MOSFET_PIN as output
    GPIOB->MODER &= ~((0x3 << (CHARGE_MOSFET_PIN * 2)) | (0x3 << (DISCHARGE_MOSFET_PIN * 2))); // Clear mode bits
    GPIOB->MODER |= ((0x1 << (CHARGE_MOSFET_PIN * 2)) | (0x1 << (DISCHARGE_MOSFET_PIN * 2)));  // Set to output mode

    // Set output type as push-pull
    GPIOB->OTYPER &= ~((1U << CHARGE_MOSFET_PIN) | (1U << DISCHARGE_MOSFET_PIN));

    // Set speed to low
    GPIOB->OSPEEDR &= ~((0x3 << (CHARGE_MOSFET_PIN * 2)) | (0x3 << (DISCHARGE_MOSFET_PIN * 2)));

    // Set no pull-up, no pull-down
    GPIOB->PUPDR &= ~((0x3 << (CHARGE_MOSFET_PIN * 2)) | (0x3 << (DISCHARGE_MOSFET_PIN * 2)));
}

// Main MOSFET control logic function
void mosfet_control_logic(sensor_values_t sensor_values) {
    time_t current_time = get_current_time_ms();
    bool debounce_passed = (current_time - last_switch_time) >= DEBOUNCE_DELAY_MS;

    // Use the higher of charge or discharge current for protections
    float current_max = fmaxf(sensor_values.battery_current_charge, sensor_values.battery_current_discharge);
    float temperature_max = fmaxf(sensor_values.battery_temperature, sensor_values.battery_temperature_alt);
    float temperature_min = fminf(sensor_values.battery_temperature, sensor_values.battery_temperature_alt);

    // Update fault flags based on sensor values
    check_overcurrent(current_max, current_time);
    check_body_diode_protection(current_max, current_time);
    check_over_temperature_protection(temperature_max, current_time);
    check_low_temperature_protection(temperature_min, current_time);
    check_voltage_protection(sensor_values.cell_voltage);

    // Determine system state based on all available sensor data
    system_state_t current_system_state = determine_system_state();

    // Update MOSFET states if debounce has passed and there are no active faults
    if (debounce_passed) {
        if (current_system_state != previous_system_state) {
            update_mosfet_states(current_system_state, current_time);
            previous_system_state = current_system_state;
        }
    } else {
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
    fault_bitmask |= (1 << 0); // Set bit 0 for short-circuit
    USART2_send_string("Short-circuit detected. Setting fault flag via interrupt.\n\r");

    // Immediately turn off both MOSFETs for safety
    set_charge_mosfet(MOSFET_OFF);
    set_discharge_mosfet(MOSFET_OFF);
    previous_charge_mosfet_state = MOSFET_OFF;
    previous_discharge_mosfet_state = MOSFET_OFF;
}

// Check for overcurrent condition
void check_overcurrent(float current_max, time_t current_time) {
    if (current_max >= OVERCURRENT_THRESHOLD) {
        fault_bitmask |= (1 << 1); // Set bit 1 for overcurrent
        USART2_send_string("Overcurrent detected. Setting fault flag.\n\r");
    } else {
        fault_bitmask &= ~(1 << 1); // Clear bit 1
    }
}

// Check for body diode protection
void check_body_diode_protection(float current, time_t current_time) {
    if (current >= HIGH_CURRENT_THRESHOLD) {
        fault_bitmask |= (1 << 2); // Set bit 2 for high threshold current
        USART2_send_string("High current detected. Setting body diode protection flag.\n\r");
    } else {
        fault_bitmask &= ~(1 << 2); // Clear bit 2
    }
}

// Check for temperature protection
void check_over_temperature_protection(float temperature, time_t current_time) {
    if (temperature > OVER_TEMPERATURE_THRESHOLD) {
    fault_bitmask |= (1 << 3); // Set bit 3 for over-temperature
    USART2_send_string("Temperature out of range. Setting fault flag.\n\r");
    } else {
    fault_bitmask &= ~(1 << 3); // Clear bit 3
    }
}

void check_low_temperature_protection(float temperature, time_t current_time) {
    if (temperature < UNDER_TEMPERATURE_THRESHOLD) {
    fault_bitmask |= (1 << 3); // Set bit 3 for over-temperature
    USART2_send_string("Temperature out of range. Setting fault flag.\n\r");
    } else {
    fault_bitmask &= ~(1 << 3); // Clear bit 3
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
        fault_bitmask |= (1 << 4); // Set bit 4 for overvoltage
        USART2_send_string("Overvoltage detected. Setting fault flag.\n\r");
    } else {
        fault_bitmask &= ~(1 << 4); // Clear bit 4
    }

    if (undervoltage) {
        fault_bitmask |= (1 << 5); // Set bit 5 for undervoltage
        USART2_send_string("Undervoltage detected. Setting fault flag.\n\r");
    } else {
        fault_bitmask &= ~(1 << 5); // Clear bit 5
    }
}

// Determine the overall system state based on the fault bitmask
system_state_t determine_system_state() {
    if (fault_bitmask & ((1 << 1) | (1 << 3))) {
        return STATE_BOTH_OFF; // Overcurrent and Overtemperature critical fault detected
    }

    if (fault_bitmask & (1 << 2)) {
        return STATE_NORMAL_OPERATION; // High current (body diode protection)
    }

    if (fault_bitmask & (1 << 5)) {
        return STATE_CHARGE_ONLY; // Undervoltage detected, only allow charging
    }

    if (fault_bitmask & (1 << 4)) {
        return STATE_DISCHARGE_ONLY; // Overvoltage detected, only allow discharging
    }

    return STATE_NORMAL_OPERATION; // No faults detected
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

// Set the state of the charge MOSFET
void set_charge_mosfet(mosfet_state_t state) {
    if (state == MOSFET_ON) {
        GPIOB->BSRR = (1U << CHARGE_MOSFET_PIN);  // Set PB1
        USART2_send_string("Setting Charge MOSFET: ON\n\r");
    } else {
        GPIOB->BSRR = (1U << (CHARGE_MOSFET_PIN + 16)); // Reset PB1
        USART2_send_string("Setting Charge MOSFET: OFF\n\r");
    }
}

void set_discharge_mosfet(mosfet_state_t state) {
    if (state == MOSFET_ON) {
        GPIOB->BSRR = (1U << DISCHARGE_MOSFET_PIN);  // Set PB2
        USART2_send_string("Setting Discharge MOSFET: ON\n\r");
    } else {
        GPIOB->BSRR = (1U << (DISCHARGE_MOSFET_PIN + 16)); // Reset PB2
        USART2_send_string("Setting Discharge MOSFET: OFF\n\r");
    }
}
