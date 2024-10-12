#include "mock_hardware.h"
#include <stdio.h>

// Simulated GPIO output states
static uint16_t gpio_state = 0;

// Function to simulate setting a GPIO pin (charge or discharge MOSFET)
void GPIO_SetPin(uint16_t pin) {
    gpio_state |= (1U << pin);
    printf("GPIO pin %d set ON\n", pin);
}

// Function to simulate resetting a GPIO pin
void GPIO_ResetPin(uint16_t pin) {
    gpio_state &= ~(1U << pin);
    printf("GPIO pin %d set OFF\n", pin);
}

// Function to simulate sending a string via USART2
void USART2_send_string(const char *string) {
    printf("USART2: %s", string);
}

// Function to simulate the current time in milliseconds
uint32_t get_current_time_ms(void) {
    static uint32_t simulated_time = 0;
    simulated_time += 100; // Increment the time by 100ms on each call
    return simulated_time;
}

// Functions to get GPIO pin states for test verification
bool is_gpio_pin_set(uint16_t pin) {
    return gpio_state & (1U << pin);
}
