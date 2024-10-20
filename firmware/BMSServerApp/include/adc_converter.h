#pragma once
#include <stdint.h>

/**
 * @brief Converts ADC value to cell voltage in millivolts.
 *
 * This function takes an ADC value and converts it to the corresponding cell voltage
 * in millivolts, based on the reference voltage and ADC resolution.
 *
 * @param adc_value The ADC value (12-bit).
 * @return The converted cell voltage in millivolts.
 */
uint16_t adc_convert_cell_voltage(const uint16_t adc_value);

/**
 * @brief Converts ADC value to battery temperature in hundredths of a degree Celsius.
 *
 * This function takes ADC values representing the LM35 temperature sensor output and the
 * reference diode voltage, then converts it to a temperature value in hundredths of a degree Celsius.
 *
 * @param output_adc The ADC value from the LM35 output.
 * @param diode_adc The ADC value from the diode reference.
 * @return The converted temperature in hundredths of a degree Celsius.
 */
int16_t adc_convert_battery_temp(const uint16_t output_adc, const uint16_t diode_adc);

/**
 * @brief Converts ADC value to battery current in milliamperes.
 *
 * This function takes an ADC value, representing the differential amplifier's output,
 * and converts it to the corresponding battery current in milliamperes, considering calibration factors.
 *
 * @param adc_value The ADC value representing the differential amplifier output.
 * @return The converted battery current in milliamperes.
 */
uint16_t adc_convert_battery_current(const uint16_t adc_value);

