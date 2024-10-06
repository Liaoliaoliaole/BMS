#include "stm32l1xx.h"
#include "bms_configuration.h"
#include "system.h"
#include "utils.h"
#include "usart2.h"
#include "mosfet_control.h"
#include <stdbool.h>
#include <time.h>

#if TEST
#include <stdio.h>
#include <assert.h>
#endif

static time_t last_switch_time = 0;

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

// Control the MOSFETs based on battery voltage and current measurements using a state machine
// Control the MOSFETs based on battery voltage and current measurements
void mosfet_control_logic(float battery_voltage, float current) {
    static voltage_state_t previous_voltage_state = STATE_NORMAL_VOLTAGE;
    uint32_t current_time = get_current_time_ms();

    // Ensure that current-time-related conditions are always up to date.
    bool debounce_passed = (current_time - last_switch_time) >= DEBOUNCE_DELAY_MS;

    // Determine voltage state
    voltage_state_t current_voltage_state;
    if (battery_voltage < LOW_VOLTAGE_THRESHOLD) {
        current_voltage_state = STATE_LOW_VOLTAGE;
    } else if (battery_voltage <= HIGH_VOLTAGE_THRESHOLD) {
        current_voltage_state = STATE_NORMAL_VOLTAGE;
    } else {
        current_voltage_state = STATE_HIGH_VOLTAGE;
    }

    char buffer[100];

    // Handle over diode max forward current conditions independently of debounce
	if (current > HIGH_CURRENT_THRESHOLD) {
		// Current exceeds limit - turn off both MOSFETs for safety
		set_charge_mosfet(MOSFET_ON);
		set_discharge_mosfet(MOSFET_ON);
		USART2_send_string("Overcurrent detected. Both MOSFETs ON.\n\r");
		return;
	}

	// Only update based on voltage change if debounce delay has passed
	if (debounce_passed) {
		if (current_voltage_state != previous_voltage_state) {
			switch (current_voltage_state) {
				case STATE_LOW_VOLTAGE:
					// Voltage is too low: allow charging only
					set_charge_mosfet(MOSFET_ON);
					set_discharge_mosfet(MOSFET_OFF);
					USART2_send_string("LOW_VOLTAGE: Charge ON, Discharge OFF.\n\r");
					break;

				case STATE_NORMAL_VOLTAGE:
					// Normal operating range: allow both charging and discharging
					set_charge_mosfet(MOSFET_ON);
					set_discharge_mosfet(MOSFET_ON);
					USART2_send_string("NORMAL_VOLTAGE: Charge ON, Discharge ON.\n\r");
					break;

				case STATE_HIGH_VOLTAGE:
					// Voltage too high: allow discharging, stop charging
					set_charge_mosfet(MOSFET_OFF);
					set_discharge_mosfet(MOSFET_ON);
					USART2_send_string("HIGH_VOLTAGE: Charge OFF, Discharge ON.\n\r");
					break;

				default:
					break;
			}

			// Update the last switch time and previous state
			last_switch_time = current_time;
			previous_voltage_state = current_voltage_state;
		}
	}
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


#if TEST
// Test function for MOSFET control logic
void test_mosfet_control_logic(void) {
    char buffer[100];

    USART2_send_string("##########################\r\n\r\n");

    // Test case 1: Battery voltage below 2.8V, current below 0.9A
    mosfet_control_logic(2.7, 0.8);
    delay_ms(100);  // Allow debounce time for testing purposes
    sprintf(buffer, "Test 1: Voltage = 2.7V, Current = 0.8A\n\r");
    USART2_send_string(buffer);

    if ((GPIOB->ODR & (1U << CHARGE_MOSFET_PIN)) != 0) {
        sprintf(buffer, "Charge MOSFET State: ON\n\r");
    } else {
        sprintf(buffer, "Charge MOSFET State: OFF (ERROR!)\n\r");
    }
    USART2_send_string(buffer);

    if ((GPIOB->ODR & (1U << DISCHARGE_MOSFET_PIN)) == 0) {
        sprintf(buffer, "Discharge MOSFET State: OFF\n\r");
    } else {
        sprintf(buffer, "Discharge MOSFET State: ON (ERROR!)\n\r");
    }
    USART2_send_string(buffer);

    // Test case 2: Battery voltage above 3.2V, current above 1.1A
    mosfet_control_logic(3.3, 1.2);
    delay_ms(100);  // Allow debounce time for testing purposes
    sprintf(buffer, "Test 2: Voltage = 3.3V, Current = 1.2A\n\r");
    USART2_send_string(buffer);

    if ((GPIOB->ODR & (1U << CHARGE_MOSFET_PIN)) == 0) {
        sprintf(buffer, "Charge MOSFET State: OFF\n\r");
    } else {
        sprintf(buffer, "Charge MOSFET State: ON (ERROR!)\n\r");
    }
    USART2_send_string(buffer);

    if ((GPIOB->ODR & (1U << DISCHARGE_MOSFET_PIN)) != 0) {
        sprintf(buffer, "Discharge MOSFET State: ON\n\r");
    } else {
        sprintf(buffer, "Discharge MOSFET State: OFF (ERROR!)\n\r");
    }
    USART2_send_string(buffer);

    // Test case 3: Battery voltage between 2.8V and 3.2V, normal operation
    mosfet_control_logic(3.0, 0.5);
    delay_ms(100);  // Allow debounce time for testing purposes
    sprintf(buffer, "Test 3: Voltage = 3.0V, Current = 0.5A\n\r");
    USART2_send_string(buffer);

    if ((GPIOB->ODR & (1U << CHARGE_MOSFET_PIN)) != 0) {
        sprintf(buffer, "Charge MOSFET State: ON\n\r");
    } else {
        sprintf(buffer, "Charge MOSFET State: OFF (ERROR!)\n\r");
    }
    USART2_send_string(buffer);

    if ((GPIOB->ODR & (1U << DISCHARGE_MOSFET_PIN)) != 0) {
        sprintf(buffer, "Discharge MOSFET State: ON\n\r");
    } else {
        sprintf(buffer, "Discharge MOSFET State: OFF (ERROR!)\n\r");
    }
    USART2_send_string(buffer);

    USART2_send_string("##########################\r\n\r\n");
}

#endif
