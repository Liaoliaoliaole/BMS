#include "mux.h"
#include "bms_configuration.h"

void mux_init() {
	RCC->AHBENR |= (1<<1); // Enable GPIOB
	GPIOB->MODER &= ~MUX_MODER_MASK;  // Clear MODER bits for PB3, PB4, PB5
	GPIOB->MODER |= ((0x01 << 6)
					| (0x01 << 8)
					| (0x01 << 10));  // Set PB3, PB4, PB5 as output (01)


	// Set output type as push-pull (OTYPER)
	GPIOB->OTYPER &= ~MUX_OTYPER_MASK;  // Clear OTYPER bits (0 = push-pull)

	// Set speed to very high (OSPEEDR)
	GPIOB->OSPEEDR &= ~MUX_OSPEEDR_MASK;  // Clear OSPEEDR bits for PB3, PB4, PB5
	GPIOB->OSPEEDR |= ((0x03 << 6) | (0x03 << 8) | (0x03 << 10));  // Set speed to VH (11)

	// Set pull-down resistors (PUPDR)
	GPIOB->PUPDR &= ~MUX_PUPDR_MASK;  // Clear PUPDR bits for PB3, PB4, PB5
	GPIOB->PUPDR |= ((0x02 << 6) | (0x02 << 8) | (0x02 << 10));  // Set pull-down resistors (10)
	}

void mux_select(const uint32_t channel) {
	// Clear all MUX control pins (PB3, PB4, PB5)
	GPIOB->ODR &= ~MUX_IDR_ODR_MASK;

	// Wait until the pins are cleared
	while (GPIOB->IDR & MUX_IDR_ODR_MASK);

	// Set the appropriate MUX pins based on the channel
	GPIOB->ODR |= (channel & MUX_IDR_ODR_MASK);

	// Wait until the selected channel is correctly set
	while ((GPIOB->IDR & MUX_IDR_ODR_MASK) != (channel & MUX_IDR_ODR_MASK));
}

void mux_reset(void) {
    // Reset all MUX pins to low.
    GPIOB->ODR &= ~MUX_PIN_MASK;
    while ((GPIOB->IDR) & (MUX_IDR_ODR_MASK));
}

/*
 * TEST with PA5 as output,Toggle the state of PA5 based on the selected MUX channel.
 * In this example, the state of PA5 is toggled based on the selected channel.
 * For even channels (0, 2, 4, 6), the LED will turn ON, and for odd channels (1, 3, 5, 7), the LED will turn OFF.
 *
void mux_init(void) {
    // Enable GPIOA clock for PA5 (on-board LED)
	RCC->AHBENR |= RCC_AHBENR_GPIOAEN;  // Enable GPIOA clock

    // Configure PA5 as output (MODER5[1:0] = 01)
    GPIOA->MODER &= ~(0x3 << (5 * 2));  // Clear MODER5[1:0]
    GPIOA->MODER |= (0x1 << (5 * 2));   // Set MODER5 to output mode

    // Set output type as push-pull (OTYPER5 = 0)
    GPIOA->OTYPER &= ~(1 << 5);  // Clear OTYPER5 (0 = push-pull)

    // Set output speed to high speed (OSPEEDR5[1:0] = 11)
    GPIOA->OSPEEDR |= (0x3 << (5 * 2));  // Set OSPEEDR5 to very high speed

    // No pull-up, pull-down (PUPDR5[1:0] = 00)
    GPIOA->PUPDR &= ~(0x3 << (5 * 2));  // Clear PUPDR5
}

void mux_select(uint32_t channel) {
    // Simulate MUX channel selection by toggling PA5
    if (channel % 2 == 0) {
        GPIOA->ODR |= (1 << 5);  // Turn on PA5 (LED on)
    } else {
        GPIOA->ODR &= ~(1 << 5);  // Turn off PA5 (LED off)
    }
}
*/
