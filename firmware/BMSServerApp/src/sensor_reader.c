#include "sensor_reader.h"
#include "adc.h"
#include "adc_converter.h"
#include "mux.h"
#include "system.h"
#include "utils.h"

#define FILTER_LENGTH 5

// Buffers to store raw ADC readings for filtering
static int16_t temp_buffer_1[FILTER_LENGTH] = {0};
static int16_t temp_buffer_2[FILTER_LENGTH] = {0};
static uint16_t current_buffer_1[FILTER_LENGTH] = {0};
static uint16_t current_buffer_2[FILTER_LENGTH] = {0};
static uint16_t voltage_buffer_1[4][FILTER_LENGTH] = {0};

void read_temperature_sensors(sensor_values_t *sensor_data) {
    // Read temperature from the first LM35 sensor
    uint16_t output_adc_1 = read_mux_channel(0, MUX_TEMP_SELECT_PIN_MASK, ADC_CHANNEL_TEMP_SENSOR); // Vout LM35 sensor 1
    uint16_t diode_adc_1 = read_mux_channel(2, MUX_TEMP_SELECT_PIN_MASK, ADC_CHANNEL_TEMP_SENSOR); // Diode voltage LM35 sensor 1
    for (int i = FILTER_LENGTH - 1; i > 0; i--) {
		temp_buffer_1[i] = temp_buffer_1[i - 1];
	}
	temp_buffer_1[0] = adc_convert_battery_temp(output_adc_1, diode_adc_1);
	sensor_data->battery_temperature = (int16_t)moving_average_filter((int32_t *)temp_buffer_1, FILTER_LENGTH);


    // Read temperature from the second LM35 sensor
    uint16_t output_adc_2 = read_mux_channel(1, MUX_TEMP_SELECT_PIN_MASK, ADC_CHANNEL_TEMP_SENSOR); // Vout LM35 sensor 2
    uint16_t diode_adc_2 = read_mux_channel(3, MUX_TEMP_SELECT_PIN_MASK, ADC_CHANNEL_TEMP_SENSOR); // Diode voltage LM35 sensor 2
    for (int i = FILTER_LENGTH - 1; i > 0; i--) {
		temp_buffer_2[i] = temp_buffer_2[i - 1];
	}
	temp_buffer_2[0] = adc_convert_battery_temp(output_adc_2, diode_adc_2);
	sensor_data->battery_temperature_alt = (int16_t)moving_average_filter((int32_t *)temp_buffer_2, FILTER_LENGTH);
}

void read_current_sensors(sensor_values_t *sensor_data)
{
    // Read charging current
    uint16_t current_adc_1 = read_mux_channel(0, MUX_CURR_SELECT_PIN_MASK, ADC_CHANNEL_SHUNT_CURRENT); // Charging current
    for (int i = FILTER_LENGTH - 1; i > 0; i--) {
		current_buffer_1[i] = current_buffer_1[i - 1];
	}
	current_buffer_1[0] = adc_convert_battery_current(current_adc_1);
	sensor_data->battery_current_charge = (uint16_t)moving_average_filter((int32_t *)current_buffer_1, FILTER_LENGTH);

    // Read discharging current
    uint16_t current_adc_2 = read_mux_channel(1, MUX_CURR_SELECT_PIN_MASK, ADC_CHANNEL_SHUNT_CURRENT); // Discharging current
    for (int i = FILTER_LENGTH - 1; i > 0; i--) {
        current_buffer_2[i] = current_buffer_2[i - 1];
    }
    current_buffer_2[0] = adc_convert_battery_current(current_adc_2);
    sensor_data->battery_current_discharge =  (uint16_t)moving_average_filter((int32_t *)current_buffer_2, FILTER_LENGTH);
}


void read_voltage_sensors(sensor_values_t *sensor_data)
{
    uint16_t cell_voltage_adc;

    // Read voltage from first multiplexer (Battery 1 and Battery 2)
    for (int channel = 0; channel < 2; ++channel) {
        cell_voltage_adc = read_mux_channel(channel, MUX_BATT1_SELECT_PIN_MASK, ADC_CHANNEL_VOLTAGE_MEASUREMENT_1);

        // Store and filter the readings
        for (int i = FILTER_LENGTH - 1; i > 0; i--) {
            voltage_buffer_1[channel][i] = voltage_buffer_1[channel][i - 1];
        }
        voltage_buffer_1[channel][0] = adc_convert_cell_voltage(cell_voltage_adc);
        sensor_data->cell_voltage[channel] =  (uint16_t)moving_average_filter((int32_t *)voltage_buffer_1[channel], FILTER_LENGTH);
    }

    // Read voltage from second multiplexer (Battery 3 and Battery 4)
    for (int channel = 0; channel < 2; ++channel) {
        cell_voltage_adc = read_mux_channel(channel, MUX_BATT2_SELECT_PIN_MASK, ADC_CHANNEL_VOLTAGE_MEASUREMENT_2);

        // Store and filter the readings
        for (int i = FILTER_LENGTH - 1; i > 0; i--) {
            voltage_buffer_1[channel + 2][i] = voltage_buffer_1[channel + 2][i - 1];
        }
        voltage_buffer_1[channel + 2][0] = adc_convert_cell_voltage(cell_voltage_adc);
        sensor_data->cell_voltage[channel + 2] =  (uint16_t)moving_average_filter((int32_t *)voltage_buffer_1[channel + 2], FILTER_LENGTH);
    }
}

void read_all_sensors(sensor_values_t *sensor_data)
{
    read_temperature_sensors(sensor_data);
    read_current_sensors(sensor_data);
    read_voltage_sensors(sensor_data);
}


