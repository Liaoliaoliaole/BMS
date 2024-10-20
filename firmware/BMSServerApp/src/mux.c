#include "stm32l1xx.h"
#include "mux.h"
#include "adc.h"
#include "system.h"

#ifdef TEST
#include "intergration_test.h"
#endif

/**
 * @brief Initializes the GPIO pins used for multiplexer control.
 *
 * This function sets up GPIO pins PB3 to PB14 as output pins for controlling
 * multiple multiplexers. It configures the pins for output, sets the speed,
 * and sets the appropriate pull resistors.
 */
void mux_init()
{
    // Enable GPIOB peripheral clock
    RCC->AHBENR |= (1 << 1); // Set bit 1 to enable GPIOB

    // Set PB3 to PB14 as output pins
	GPIOB->MODER &= ~MUX_MODER_MASK;  // Clear MODER bits for PB3 to PB14
	GPIOB->MODER |= ((0x01 << 6) | (0x01 << 8) | (0x01 << 10) | (0x01 << 12) | (0x01 << 14) | (0x01 << 16) | (0x01 << 18) | (0x01 << 20) | (0x01 << 22) | (0x01 << 24) | (0x01 << 26) | (0x01 << 28));  // Set PB3 to PB14 as output (01)

	// Set output type as push-pull (OTYPER)
	GPIOB->OTYPER &= ~MUX_OTYPER_MASK;  // Clear OTYPER bits (0 = push-pull)

	// Set speed to very high (OSPEEDR)
	GPIOB->OSPEEDR &= ~MUX_OSPEEDR_MASK;  // Clear OSPEEDR bits for PB3 to PB14
	GPIOB->OSPEEDR |= ((0x03 << 6) | (0x03 << 8) | (0x03 << 10) | (0x03 << 12) | (0x03 << 14) | (0x03 << 16) | (0x03 << 18) | (0x03 << 20) | (0x03 << 22) | (0x03 << 24) | (0x03 << 26) | (0x03 << 28));  // Set speed to VH (11)

	// Set pull-down resistors (PUPDR)
	GPIOB->PUPDR &= ~MUX_PUPDR_MASK;  // Clear PUPDR bits for PB3 to PB14
	GPIOB->PUPDR |= ((0x02 << 6) | (0x02 << 8) | (0x02 << 10) | (0x02 << 12) | (0x02 << 14) | (0x02 << 16) | (0x02 << 18) | (0x02 << 20) | (0x02 << 22) | (0x02 << 24) | (0x02 << 26) | (0x02 << 28));  // Set pull-down resistors (10)
}

/**
 * @brief Selects a specific channel on the multiplexer.
 *
 * Clears all MUX control pins and sets the appropriate pins to select the given channel.
 *
 * @param channel The MUX channel to select (0-7).
 * @param mux_select_pin_mask The GPIO pin mask that represents the control pins for the MUX.
 */
void mux_select(const uint32_t channel, uint32_t mux_select_pin_mask)
{
	// Clear all selected MUX control pins
    GPIOB->ODR &= ~mux_select_pin_mask;

    // Wait until the pins are cleared
    while (GPIOB->IDR & mux_select_pin_mask);

    // Set the appropriate MUX pins based on the channel
    GPIOB->ODR |= (channel & mux_select_pin_mask);

    // Wait until the selected channel is correctly set
    while ((GPIOB->IDR & mux_select_pin_mask) != (channel & mux_select_pin_mask));
}

/**
 * @brief Reads an ADC value from a specified channel using MUX selection.
 *
 * Selects the desired multiplexer channel, reads the corresponding ADC value, and then resets the MUX.
 *
 * @param channel The MUX channel to select (0-7).
 * @param mux_select_pin_mask The GPIO pin mask representing the MUX control pins.
 * @param adc_channel The ADC channel to read from.
 *
 * @return The ADC value read from the specified channel.
 */
uint16_t read_mux_channel(uint32_t channel, uint32_t mux_select_pin_mask, uint32_t adc_channel)
{
#ifdef TEST
	return get_mock_mux_output_value(channel, mux_select_pin_mask);
#endif

	mux_select(channel, mux_select_pin_mask);  // Select the appropriate MUX channel
	uint16_t adc_value = adc_read(adc_channel);  // Read the ADC value from the selected channel
	mux_reset(mux_select_pin_mask);  // Reset the MUX control pins
	return adc_value;  // Return the ADC value
}

/**
 * @brief Resets the multiplexer control pins.
 *
 * This function sets all MUX control pins to low to reset the multiplexer.
 *
 * @param mux_select_pin_mask The GPIO pin mask that represents the control pins for the MUX.
 */
void mux_reset(uint32_t mux_select_pin_mask)
{
    // Reset all selected MUX pins to low.
    GPIOB->ODR &= ~mux_select_pin_mask;
    while ((GPIOB->IDR) & (mux_select_pin_mask));
}

#ifdef TEST_HARDWARE
/**
 * @brief Test function to validate multiplexer channel selection.
 *
 * Uses the on-board LED (PA5) to indicate the selected channel.
 * Blinks the LED for each selected channel to confirm MUX operation.
 */
void mux_test(void)
{
    // Enable GPIOA peripheral clock
    RCC->AHBENR |= (1 << 0); // Set bit 0 to enable GPIOA

    // Set PA5 as output pin (On-board LED)
    GPIOA->MODER &= ~(0x3 << 10);  // Clear MODER bits for PA5
    GPIOA->MODER |= (0x01 << 10);  // Set PA5 as output (01)

    // Test each channel of Battery Voltage Measurement Multiplexer 1
    for (uint32_t channel = 0; channel <= 7; ++channel) {
        mux_select(channel, MUX_BATT1_SELECT_PIN_MASK);
        GPIOA->ODR |= (1 << 5);  // Turn on LED to indicate channel selection
        for (volatile int i = 0; i < 100000; i++);
        GPIOA->ODR &= ~(1 << 5); // Turn off LED
        for (volatile int i = 0; i < 100000; i++);
    }
}
#endif
