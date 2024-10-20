#include "sensor_reader.h"
#include "adc.h"
#include "adc_converter.h"
#include "mux.h"
#include "system.h"

// Global variable to store sensor values
sensor_values_t sensor_data;

void read_temperature_sensors(sensor_values_t *sensor_data) {
    // Read temperature from the first LM35 sensor
    uint16_t output_adc_1 = read_mux_channel(0, MUX_TEMP_SELECT_PIN_MASK, ADC_CHANNEL_TEMP_SENSOR); // Vout LM35 sensor 1
    uint16_t diode_adc_1 = read_mux_channel(2, MUX_TEMP_SELECT_PIN_MASK, ADC_CHANNEL_TEMP_SENSOR); // Diode voltage LM35 sensor 1
    sensor_data->battery_temperature = adc_convert_battery_temp(output_adc_1, diode_adc_1);

    // Read temperature from the second LM35 sensor
    uint16_t output_adc_2 = read_mux_channel(1, MUX_TEMP_SELECT_PIN_MASK, ADC_CHANNEL_TEMP_SENSOR); // Vout LM35 sensor 2
    uint16_t diode_adc_2 = read_mux_channel(3, MUX_TEMP_SELECT_PIN_MASK, ADC_CHANNEL_TEMP_SENSOR); // Diode voltage LM35 sensor 2
    sensor_data->battery_temperature_alt = adc_convert_battery_temp(output_adc_2, diode_adc_2);
}

void read_current_sensors(sensor_values_t *sensor_data)
{
    // Read charging current
    uint16_t current_adc_1 = read_mux_channel(0, MUX_CURR_SELECT_PIN_MASK, ADC_CHANNEL_SHUNT_CURRENT); // Charging current
    sensor_data->battery_current_charge = adc_convert_battery_current(current_adc_1);

    // Read discharging current
    uint16_t current_adc_2 = read_mux_channel(1, MUX_CURR_SELECT_PIN_MASK, ADC_CHANNEL_SHUNT_CURRENT); // Discharging current
    sensor_data->battery_current_discharge = adc_convert_battery_current(current_adc_2);
}


void read_voltage_sensors(sensor_values_t *sensor_data)
{
    uint16_t cell_voltage_adc;

    // Read voltage from first multiplexer (Battery 1 and Battery 2)
    for (uint32_t channel = 0; channel < 2; ++channel) {
        cell_voltage_adc = read_mux_channel(channel, MUX_BATT1_SELECT_PIN_MASK, ADC_CHANNEL_VOLTAGE_MEASUREMENT_1);
        sensor_data->cell_voltage[channel] = adc_convert_cell_voltage(cell_voltage_adc);
    }

    // Read voltage from second multiplexer (Battery 3 and Battery 4)
    for (uint32_t channel = 0; channel < 2; ++channel) {
        cell_voltage_adc = read_mux_channel(channel, MUX_BATT2_SELECT_PIN_MASK, ADC_CHANNEL_VOLTAGE_MEASUREMENT_2);
        sensor_data->cell_voltage[channel + 2] = adc_convert_cell_voltage(cell_voltage_adc);
    }
}

void read_all_sensors(sensor_values_t *sensor_data)
{
    read_temperature_sensors(sensor_data);
    read_current_sensors(sensor_data);
    read_voltage_sensors(sensor_data);
}

#if TEST
void read_sensor_values_mock(int16_t *sensors)
{
    // Mock read ADC value for battery temperature
    uint16_t lm35_o = adc_read_mock(1);
    uint16_t lm35_d = adc_read_mock(2);
    sensor_data.battery_temperature = adc_convert_battery_temp(lm35_o, lm35_d);
    sensors[0] = sensor_data.battery_temperature;
    sensors[1] = sensor_data.battery_temperature - 12;

    // Mock read ADC value for cell voltages
    for (int i = 0; i < 4; i++) {
        sensor_data.cell_voltage[i] = adc_convert_cell_voltage(adc_read_mock(0));
        sensors[2 + i] = sensor_data.cell_voltage[i];
    }

    // Mock read ADC value for battery current (charge)
    uint16_t adc_c_charge = adc_read_mock(4);
    sensor_data.battery_current_charge = adc_convert_battery_current(adc_c_charge);
    sensors[6] = sensor_data.battery_current_charge;

    // Mock read ADC value for battery current (discharge)
    uint16_t adc_c_discharge = adc_read_mock(4);
    sensor_data.battery_current_discharge = adc_convert_battery_current(adc_c_discharge);
    sensors[7] = sensor_data.battery_current_discharge;

    // Mock read ADC value for flame sensor
    sensor_data.flame_sensor = adc_read_mock(6);
    sensors[8] = sensor_data.flame_sensor;
}
#endif


