#pragma once
#include <stdint.h>

/**
 * @brief Initializes the GPIO pins used for multiplexer control.
 *
 * This function sets up GPIO pins PB3 to PB14 as output pins for controlling
 * multiple multiplexers. It configures the pins for output, sets the speed,
 * and sets the appropriate pull resistors.
 */
void mux_init();

/**
 * @brief Selects a specific channel on the multiplexer.
 *
 * Clears all MUX control pins and sets the appropriate pins to select the given channel.
 *
 * @param channel The MUX channel to select (0-7).
 * @param mux_select_pin_mask The GPIO pin mask that represents the control pins for the MUX.
 */
void mux_select(const uint32_t channel, uint32_t mux_select_pin_mask);

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
uint16_t read_mux_channel(uint32_t channel, uint32_t mux_select_pin_mask, uint32_t adc_channel);

/**
 * @brief Resets the multiplexer control pins.
 *
 * This function sets all MUX control pins to low to reset the multiplexer.
 *
 * @param mux_select_pin_mask The GPIO pin mask that represents the control pins for the MUX.
 */
void mux_reset(uint32_t mux_select_pin_mask);

#ifdef TEST
/**
 * @brief Test function to validate multiplexer channel selection.
 *
 * Uses the on-board LED (PA5) to indicate the selected channel.
 * Blinks the LED for each selected channel to confirm MUX operation.
 */
void mux_test();
#endif
