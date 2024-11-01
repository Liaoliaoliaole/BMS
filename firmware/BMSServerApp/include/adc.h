#pragma once

/**
 * @brief Initializes the ADC module.
 *
 * Configures GPIO pins for ADC input, enables the ADC clock, sets the ADC
 * to single conversion mode, and configures sampling times for each channel.
 */
void adc_init(void);

/**
 * @brief Reads the ADC value from the specified channel.
 *
 * Disables the ADC before configuring the channel, then reads the ADC value
 * after enabling it and starting the conversion.
 *
 * @param adc_channel The channel to read from.
 * @return The ADC value read from the specified channel.
 */
uint16_t adc_read(const uint8_t adc_channel);

#if TEST
/**
 * @brief Reads the ADC value from the specified channel.
 *
 * Disables the ADC before configuring the channel, then reads the ADC value
 * after enabling it and starting the conversion.
 *
 * @param adc_channel The channel to read from.
 * @return The ADC value read from the specified channel.
 */
uint16_t adc_read_mock(const uint8_t adc_channel);
#endif
