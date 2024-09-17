#include "bms_configuration.h"
#include "adc_converter.h"

// Measurement of cell voltage in mv.
uint16_t adc_convert_cell_voltage(const uint16_t adc_value) {
	float voltage = (float)adc_value * CELL_VOLT_MAX / 4095.0;//TODO: add calibration
	return (uint16_t)(voltage * 1000);
}

/*
	LM35 Specifications:
	Output voltage = Temperature in °C × 10 mV
	At −55°C: VOUT = −550 mV
	At 25°C: VOUT = 250 mV
	At +150°C: VOUT = 1500 mV
 */

uint16_t adc_convert_battery_temp(const uint16_t adc_value) {
	float voltage_raw = (float)adc_value * REFERENCE_VOLT * 2 / 4095.0 - REFERENCE_VOLT;
    float temp_celcius = (voltage_raw) / 0.01;
    // Return temperature in hundredths of a degree Celsius, accuracy in 0.01
    return (uint16_t)(temp_celcius * 100);
}

uint16_t adc_convert_battery_current(const uint16_t adc_value){
	//float voltage = (adc_value / 4095.0) * REFERENCE_VOLT;
	//float current = (voltage - REFERENCE_VOLT) / SHUNT_RESISTANCE;
	//return current;  // Positive for charging, negative for discharging

}
