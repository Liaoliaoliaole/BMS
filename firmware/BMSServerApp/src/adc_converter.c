#include "bms_configuration.h"
#include "adc_converter.h"

/**
 * @brief Converts ADC value to cell voltage in millivolts.
 *
 * Takes a 12-bit ADC value and calculates the cell voltage based on the reference voltage
 * and ADC resolution. Uses integer arithmetic to prevent overflow.
 *
 * @param adc_value The ADC value (12-bit resolution).
 * @return The corresponding cell voltage in millivolts.
 */
uint16_t adc_convert_cell_voltage(const uint16_t adc_value) {
	return (adc_value * REF_VOLT_ADC) / ADC_MAX_VALUE;
}


/**
 * @brief Converts ADC values to battery temperature in hundredths of a degree Celsius.
 *
 * This function calculates the temperature by subtracting the reference diode voltage
 * from the LM35 output voltage, converting the difference to Celsius with a sensitivity
 * of 10mV/degree Celsius. The returned value is in hundredths of a degree Celsius.
 *
 * @param output_adc The ADC value from the LM35 temperature sensor.
 * @param diode_adc The ADC value from the diode reference.
 * @return The calculated temperature in hundredths of a degree Celsius.
 */
int16_t adc_convert_battery_temp(const uint16_t output_adc, const uint16_t diode_adc) {
	int32_t out_voltage_mv = (output_adc * REF_VOLT_ADC) / ADC_MAX_VALUE;
	int32_t diode_voltage_mv = (diode_adc * REF_VOLT_ADC) / ADC_MAX_VALUE;

	int32_t diff_voltage_mv = out_voltage_mv - diode_voltage_mv;

	int16_t temp_celsius_hundredths = (diff_voltage_mv * 100) / 10; // 10mV per degree Celsius

	return temp_celsius_hundredths;
}

/**
 * @brief Converts ADC value to battery current in milliamperes.
 *
 * This function calculates the battery current based on the ADC value taken from the differential
 * amplifier output, the reference voltage, and calibration parameters. It considers the differential
 * amplifier gain and shunt resistance.
 *
 * The formula also uses linear calibration (y = mx + b), where m is the gain and b is the offset.
 *
 * @param adc_value The ADC value representing the current.
 * @return The calculated battery current in milliamperes.
 */
uint16_t adc_convert_battery_current(const uint16_t adc_value){
    int32_t voltage_mv = (adc_value * REF_VOLT_ADC) / ADC_MAX_VALUE;
    int32_t shunt_voltage_mv = (voltage_mv * 10) / GAIN; // Gain multiplied by 10 for precision

    int32_t current_ma = (shunt_voltage_mv * 1000) / R_SHUNT; // R_SHUNT is in milliohms

    current_ma = (current_ma * CURRENT_CALIBRATION_GAIN) / 1000 + CURRENT_CALIBRATION_OFFSET;

    if (current_ma < 0) {
        current_ma = 0;
    }

    return (uint16_t)current_ma;
}
