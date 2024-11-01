/*
 * system.c
 *
 *  Created on: 2024?10?31?
 *      Author: tende
 */

#include "system.h"
#include "stm32l1xx.h"
#include "bms_configuration.h"
#include "usart2.h"

uint8_t system_in_deep_sleep_mode = 0; // Initialize to 0 (awake)

//Configures the MCU to enter deep sleep mode.
void enter_deep_sleep(void) {
    SCB->SCR |= SCB_SCR_SLEEPDEEP_Msk;      // Set the SleepDeep bit to enable deep sleep
    PWR->CR |= PWR_CR_LPSDSR;               // Enable low-power regulator in stop mode
    PWR->CR &= ~PWR_CR_PDDS;                // Stop mode instead of standby

    USART2->CR1 &= ~USART_CR1_UE;           // Disable USART to save power

    __WFI(); // Wait for Interrupt (enters deep sleep until a wake-up event occurs)
}

// Configures the MCU to resume from deep sleep mode.
void exit_deep_sleep(void) {
	system_in_deep_sleep_mode = 1;
	system_in_deep_sleep_mode = 0;
    SetSysClock();
    SystemCoreClockUpdate();
    USART2_Init();
}

// Checks the temperature and enters deep sleep mode if below threshold.
void check_deep_sleep_condition(sensor_values_t* sensor_values) {
    int16_t avg_temp = (sensor_values->battery_temperature + sensor_values->battery_temperature_alt) / 2;

    if (avg_temp < DEEP_SLEEP_TEMP_THRESHOLD) {
        enter_deep_sleep();
    }
}
