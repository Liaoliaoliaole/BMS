#include "bms_configuration.h"
#include "adc_converter.h"

// Measurement of cell voltage in mv.
uint16_t adc_convert_cell_voltage(const uint16_t adc_value) {
	float voltage = (float)adc_value * CELL_VOLT_MAX / ADC_MAX_VALUE;//TODO: add calibration?
	return (uint16_t)(voltage * 1000);
}

/*
	LM35 Specifications:
	Output voltage = Temperature in deg × 10 mV
	At −55 deg: VOUT = −550 mV
	At 25 deg: VOUT = 250 mV
	At +150 deg: VOUT = 1500 mV
	Using asymmetric bipolar voltage range of -1.4V to +5V.
 */
int16_t adc_convert_battery_temp(const uint16_t adc_value) {
    // Convert the ADC value to a voltage
    // The total span of the reference voltage is POSITIVE_REF_VOLT - NEGATIVE_REF_VOLT
    float voltage = ((float)adc_value / ADC_MAX_VALUE) * (POSITIVE_REF_VOLT - NEGATIVE_REF_VOLT) + NEGATIVE_REF_VOLT;
    
    // Convert the voltage to temperature in Celsius (LM35 outputs 10 mV/°C or 0.01V/°C)
    float temp_celsius = voltage / 0.01;
    
    // Return temperature in hundredths of a degree Celsius (for accuracy)
    return (int16_t)(temp_celsius * 100);
}

/*
	Measure current in both directions (charge and discharge) using a differential amplifier and shunt.
	The maximum current through the MOSFET will determine the voltage drop across the shunt.
	To measure up to +-10A with a shunt resistor value of shunt = 0.01
	The voltage drop across the shunt would be: shunt=Imax*shunt=10A×0.01=0.1V
	The reference voltage of 3.3V (STM32).
	GAIN can be calculated as Rf/Rin=3.3/0.1=33
*/
uint16_t adc_convert_battery_current(const uint16_t adc_value){
    float voltage = (float)adc_value * (5.0 / ADC_MAX_VALUE);  // Convert ADC to voltage
    float shunt_voltage = voltage / GAIN;  // Undo the amplifier gain
    float current = shunt_voltage / R_SHUNT; 
    return current;
}
