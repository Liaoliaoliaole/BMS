#pragma once
#include <stdint.h>

uint16_t adc_convert_cell_voltage(const uint16_t adc_value);
int16_t adc_convert_battery_temp(const uint16_t output_adc, const uint16_t diode_adc);
uint16_t adc_convert_battery_current(const uint16_t adc_value);

