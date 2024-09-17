#include <stdint.h>
#include "bms_configuration.h"

uint16_t adc_convert_cell_voltage(const uint16_t adc_value);
uint16_t adc_convert_battery_temp(const uint16_t adc_value);
uint16_t adc_convert_battery_current(const uint16_t adc_value);
