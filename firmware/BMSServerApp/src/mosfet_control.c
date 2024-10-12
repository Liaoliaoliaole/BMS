#include "stm32l1xx.h"
#include "bms_configuration.h"
#include "system.h"
#include "utils.h"
#include "usart2.h"
#include "mosfet_control.h"

#include <time.h>
#include <math.h>

// File-scope static variables for the control logic state
static voltage_state_t previous_voltage_state = STATE_NORMAL_VOLTAGE;
static mosfet_state_t previous_charge_mosfet_state = MOSFET_OFF;
static mosfet_state_t previous_discharge_mosfet_state = MOSFET_OFF;
static bool overcurrent_active = false;
static bool short_circuit_active = false;
static bool overtemperature_active = false;
static bool undertemperature_active = false;
static time_t last_switch_time = 0;
static float soc = 50.0f; // Initialize to 50% for simplicity
static float previous_soc = 50.0f;
static time_t last_soc_update_time = 0;

void reset_mosfet_control_logic() {
    previous_voltage_state = STATE_NORMAL_VOLTAGE;
    previous_charge_mosfet_state = MOSFET_OFF;
    previous_discharge_mosfet_state = MOSFET_OFF;
    overcurrent_active = false;
    short_circuit_active = false;
    overtemperature_active = false;
    undertemperature_active = false;
    last_switch_time = get_current_time_ms();
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

    // Calculate total battery voltage TODO maybe use later?
    //float total_battery_voltage = calculate_total_voltage(sensor_values.cell_voltage, 4);

    // Calculate currents (convert mA to A)
    float charge_current = sensor_values.battery_current_charge / 1000.0f;
    float discharge_current = sensor_values.battery_current_discharge / 1000.0f;

    // Use the higher of charge or discharge current for SoC calculation and protections
    float current = fmaxf(charge_current, discharge_current);

    update_soc(charge_current , current_time);

    // Determine if the battery is charging or discharging
    bool charging = is_charging();
    bool discharging = is_discharging();


    // Get temperature (assume battery_temperature is in degrees Celsius)
    float temperature = sensor_values.battery_temperature;

    // Perform protection checks
    if (check_short_circuit(current, current_time)) return;
    if (check_overcurrent(current, charging, discharging, current_time)) return;
    if (check_body_diode_protection(current, current_time)) return;
    if (check_temperature_protection(temperature, current_time)) return;

    // Voltage-based logic
    if (debounce_passed && !overcurrent_active && !short_circuit_active && !overtemperature_active) {
        voltage_state_t current_voltage_state = check_voltage_state(sensor_values);
        if (current_voltage_state != previous_voltage_state) {
            update_mosfet_states(current_voltage_state, current_time);
            previous_voltage_state = current_voltage_state;
        }
    } else {
        // Debounce not passed or fault active; maintain previous states
        set_charge_mosfet(previous_charge_mosfet_state);
        set_discharge_mosfet(previous_discharge_mosfet_state);
    }
}

bool is_charging() {
    // If current SoC is greater than previous SoC, then it's charging
    bool charging = (soc > previous_soc);
    previous_soc = soc; // Update the previous SoC
    return charging;
}

bool is_discharging() {
    // If current SoC is less than previous SoC, then it's discharging
    bool discharging = (soc < previous_soc);
    previous_soc = soc; // Update the previous SoC
    return discharging;
}

// Check for short-circuit condition
bool check_short_circuit(float current, uint32_t current_time) {
    if (current >= SHORT_CIRCUIT_THRESHOLD) {
        if (!short_circuit_active) {
            short_circuit_active = true;
            USART2_send_string("Short-circuit detected. Turning off both MOSFETs.\n\r");
            set_charge_mosfet(MOSFET_OFF);
            set_discharge_mosfet(MOSFET_OFF);
            previous_charge_mosfet_state = MOSFET_OFF;
            previous_discharge_mosfet_state = MOSFET_OFF;
            last_switch_time = current_time;
        }
        return true; // Fault active
    } else {
        short_circuit_active = false;
        return false;
    }
}

// Check for overcurrent condition
bool check_overcurrent(float current, bool charging, bool discharging, uint32_t current_time) {
    if (charging && current >= OVERCURRENT_THRESHOLD) {
        if (!overcurrent_active) {
            overcurrent_active = true;
            USART2_send_string("Charging overcurrent detected. Disabling charging.\n\r");
            set_charge_mosfet(MOSFET_OFF);
            previous_charge_mosfet_state = MOSFET_OFF;
            last_switch_time = current_time;
        }
        return true; // Fault active
    } else if (discharging && current >= OVERCURRENT_THRESHOLD) {
        if (!overcurrent_active) {
            overcurrent_active = true;
            USART2_send_string("Discharging overcurrent detected. Disabling discharging.\n\r");
            set_discharge_mosfet(MOSFET_OFF);
            previous_discharge_mosfet_state = MOSFET_OFF;
            last_switch_time = current_time;
        }
        return true; // Fault active
    } else {
        overcurrent_active = false;
        return false;
    }
}

// Check for body diode protection
bool check_body_diode_protection(float current, uint32_t current_time) {
    if (current >= HIGH_CURRENT_THRESHOLD) {
        if (!overcurrent_active) {
            overcurrent_active = true;
            USART2_send_string("High current detected. Turning on both MOSFETs to prevent body diode conduction.\n\r");
            set_charge_mosfet(MOSFET_ON);
            set_discharge_mosfet(MOSFET_ON);
            previous_charge_mosfet_state = MOSFET_ON;
            previous_discharge_mosfet_state = MOSFET_ON;
            last_switch_time = current_time;
        }
        return true; // Fault active
    } else if (current >= LOW_CURRENT_THRESHOLD && current < HIGH_CURRENT_THRESHOLD) {
        // Maintain previous states to prevent frequent switching
        set_charge_mosfet(previous_charge_mosfet_state);
        set_discharge_mosfet(previous_discharge_mosfet_state);
        USART2_send_string("Maintaining previous MOSFET states due to intermediate current level.\n\r");
        return true; // Fault active
    } else {
        overcurrent_active = false;
        return false;
    }
}

// Check for temperature protection
bool check_temperature_protection(float temperature, uint32_t current_time) {
	static uint32_t recovery_start_time = 0;

    if (temperature >= OVER_TEMPERATURE_THRESHOLD) {
        if (!overtemperature_active) {
            overtemperature_active = true;
            recovery_start_time = current_time;
            USART2_send_string("Overtemperature detected. Disabling charging and discharging.\n\r");
            set_charge_mosfet(MOSFET_OFF);
            set_discharge_mosfet(MOSFET_OFF);
            previous_charge_mosfet_state = MOSFET_OFF;
            previous_discharge_mosfet_state = MOSFET_OFF;
            last_switch_time = current_time;
        }
        return true; // Fault active
    } else if (temperature <= UNDER_TEMPERATURE_THRESHOLD) {
        if (!undertemperature_active) {
            undertemperature_active = true;
            USART2_send_string("Undertemperature detected. Disabling charging.\n\r");
            set_charge_mosfet(MOSFET_OFF);
            previous_charge_mosfet_state = MOSFET_OFF;
            last_switch_time = current_time;
        }
        return false; // Discharging may continue
    } else if (overtemperature_active || undertemperature_active) {
        // Fault recovery logic: After a delay, allow normal operation again
        if ((current_time - recovery_start_time) > DEBOUNCE_DELAY_MS) {
            overtemperature_active = false;
            undertemperature_active = false;
            USART2_send_string("Temperature back to normal. Enabling normal operations.\n\r");
        }
    }
    return false;
}

// Check voltage state based on cell voltages
voltage_state_t check_voltage_state(sensor_values_t sensor_values) {
    bool cell_overvoltage = false;
    bool cell_undervoltage = false;
    for (int i = 0; i < 4; i++) {
        float cell_voltage = sensor_values.cell_voltage[i] / 1000.0f; // Convert mV to V
        if (cell_voltage >= HIGH_VOLTAGE_THRESHOLD) {
            cell_overvoltage = true;
            break;
        }
        if (cell_voltage <= LOW_VOLTAGE_THRESHOLD) {
            cell_undervoltage = true;
            break;
        }
    }

    if (cell_undervoltage) {
        return STATE_LOW_VOLTAGE;
    } else if (cell_overvoltage) {
        return STATE_HIGH_VOLTAGE;
    } else {
        return STATE_NORMAL_VOLTAGE;
    }
}

// Update MOSFET states based on voltage state
void update_mosfet_states(voltage_state_t current_voltage_state, uint32_t current_time) {
    switch (current_voltage_state) {
        case STATE_LOW_VOLTAGE:
            // Voltage too low: allow charging only
            set_charge_mosfet(MOSFET_ON);
            set_discharge_mosfet(MOSFET_OFF);
            USART2_send_string("LOW_VOLTAGE: Charge ON, Discharge OFF.\n\r");
            previous_charge_mosfet_state = MOSFET_ON;
            previous_discharge_mosfet_state = MOSFET_OFF;
            break;

        case STATE_NORMAL_VOLTAGE:
            // Normal voltage: allow charging and discharging
            set_charge_mosfet(MOSFET_ON);
            set_discharge_mosfet(MOSFET_ON);
            USART2_send_string("NORMAL_VOLTAGE: Charge ON, Discharge ON.\n\r");
            previous_charge_mosfet_state = MOSFET_ON;
            previous_discharge_mosfet_state = MOSFET_ON;
            break;

        case STATE_HIGH_VOLTAGE:
            // Voltage too high: disable charging
            set_charge_mosfet(MOSFET_OFF);
            set_discharge_mosfet(MOSFET_ON);
            USART2_send_string("HIGH_VOLTAGE: Charge OFF, Discharge ON.\n\r");
            previous_charge_mosfet_state = MOSFET_OFF;
            previous_discharge_mosfet_state = MOSFET_ON;
            break;

        default:
            break;
    }
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

// Update State of Charge (SoC)
void update_soc(float current, uint32_t current_time) {
    uint32_t time_delta = current_time - last_soc_update_time;

    // Update the SOC at a regular interval
    if (time_delta >= SOC_UPDATE_INTERVAL_MS) {
        // Coulomb counting: delta_SOC = (current * time) / battery_capacity
        // Assume battery capacity is 10 Ah for simplicity
        const float battery_capacity = 10.0f; // Ampere-hours

        // Calculate delta SoC
        float delta_soc = (current * (time_delta / 3600000.0f)) / battery_capacity; // Convert ms to hours

        // Update the SoC
        soc += delta_soc;

        // Bound SoC between 0% and 100%
        if (soc > 100.0f) {
            soc = 100.0f;
        } else if (soc < 0.0f) {
            soc = 0.0f;
        }

        // Update the last update time
        last_soc_update_time = current_time;

        // Debug message
        char buffer[100];
        sprintf(buffer, "SoC updated: %.2f%%\n\r", soc);
        USART2_send_string(buffer);
    }
}


