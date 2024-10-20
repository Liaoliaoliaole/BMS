#pragma once
#include "sensor_reader.h"

void intergration_test(sensor_values_t* sensor_data);
uint16_t get_mock_mux_output_value(uint32_t channel, uint32_t mux_select_pin_mask);
