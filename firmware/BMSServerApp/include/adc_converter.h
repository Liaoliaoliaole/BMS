#pragma once
#include <stdint.h>

uint16_t adc_convert_cell_voltage(const uint16_t adc_value);
int16_t adc_convert_battery_temp(const uint16_t adc_value);
uint16_t adc_convert_battery_current(const uint16_t adc_value);
