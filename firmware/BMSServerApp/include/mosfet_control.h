#pragma once

#include <stdint.h>
#include "sensor_reader.h"
#include "utils.h"

// MOSFET states enumeration
typedef enum {
    MOSFET_OFF = 0,  ///< MOSFET is turned off
    MOSFET_ON        ///< MOSFET is turned on
} mosfet_state_t;

// System states enumeration
typedef enum {
    STATE_BOTH_OFF = 0,    ///< Both charging and discharging MOSFETs are off
    STATE_CHARGE_ONLY,     ///< Only charging MOSFET is on
    STATE_DISCHARGE_ONLY,  ///< Only discharging MOSFET is on
    STATE_NORMAL_OPERATION ///< Both charging and discharging MOSFETs are on
} system_state_t;

/**
 * @brief Resets the MOSFET control logic to initial states.
 *
 * This function resets the MOSFET states and clears any existing fault flags.
 */
void reset_mosfet_control_logic(void);

/**
 * @brief Initializes MOSFET GPIO pins for control.
 *
 * Sets the appropriate pins for charge and discharge MOSFETs as output pins.
 */
void mosfet_init(void);

/**
 * @brief Main MOSFET control logic function.
 *
 * Determines the system state and manages the MOSFETs based on sensor readings.
 * Uses moving average filters for current and voltage readings to reduce noise.
 *
 * @param sensor_values Pointer to sensor data structure containing all sensor readings.
 */
void mosfet_control_logic(sensor_values_t* sensor_values);

/**
 * @brief Interrupt service routine for short-circuit protection.
 *
 * Sets the fault flag for short-circuit and turns off both MOSFETs immediately.
 */
void short_circuit_isr(void);

void EXTI0_IRQHandler(void);

/**
 * @brief Sets the state of the charge MOSFET.
 *
 * @param state Desired state of the charge MOSFET (ON or OFF).
 */
void set_charge_mosfet(mosfet_state_t state);

/**
 * @brief Sets the state of the discharge MOSFET.
 *
 * @param state Desired state of the discharge MOSFET (ON or OFF).
 */
void set_discharge_mosfet(mosfet_state_t state);

/**
 * @brief Checks for overcurrent condition and updates fault bitmask.
 *
 * @param current_max Maximum current reading from the sensor.
 */
void check_overcurrent(uint16_t current_max);

/**
 * @brief Checks for body diode protection condition and updates fault bitmask.
 *
 * @param current Current reading from the sensor.
 */
void check_body_diode_protection(uint16_t current);

/**
 * @brief Checks for over-temperature condition and updates fault bitmask.
 *
 * @param temperature Temperature reading from the sensor.
 */
void check_over_temperature_protection(int16_t temperature);

/**
 * @brief Checks for low-temperature condition and updates fault bitmask.
 *
 * @param temperature Temperature reading from the sensor.
 */
void check_low_temperature_protection(int16_t temperature);

/**
 * @brief Checks for voltage protection conditions and updates fault bitmask.
 *
 * @param cell_voltage Array of voltage readings from the cells.
 */
void check_voltage_protection(uint16_t cell_voltage[4]);

/**
 * @brief Determines the overall system state based on fault conditions.
 *
 * @return The current system state based on detected faults.
 */
system_state_t determine_system_state(void);

/**
 * @brief Updates the states of the MOSFETs based on the system state.
 *
 * @param system_state The current state of the system.
 * @param current_time Current timestamp.
 */
void update_mosfet_states(system_state_t system_state, uint32_t current_time);
