#include "sensor_reader.h"
#include "adc.h"
#include "adc_converter.h"

// Global variable to store sensor values
sensor_values_t sensor_data;

//TODO
void read_sensor_values(int16_t *sensors) {
    // Read ADC value for battery temperature
    uint16_t lm35_o = adc_read(1);
    uint16_t lm35_d = adc_read(4);
    sensor_data.battery_temperature = adc_convert_battery_temp(lm35_o, lm35_d);
    sensors[0] = sensor_data.battery_temperature;
    sensors[1] = sensor_data.battery_temperature - 12;

    // Read ADC value for cell voltages
    for (int i = 0; i < 4; i++) {
        sensor_data.cell_voltage[i] = adc_convert_cell_voltage(adc_read(0));
        sensors[2 + i] = sensor_data.cell_voltage[i];
    }

    // Read ADC value for battery current (charge)
    uint16_t adc_c_charge = adc_read(5);
    sensor_data.battery_current_charge = adc_convert_battery_current(adc_c_charge);
    sensors[6] = sensor_data.battery_current_charge;

    // Read ADC value for battery current (discharge)
    uint16_t adc_c_discharge = adc_read(5);
    sensor_data.battery_current_discharge = adc_convert_battery_current(adc_c_discharge);
    sensors[7] = sensor_data.battery_current_discharge;

    // Read ADC value for flame sensor
    sensor_data.flame_sensor = adc_read(6);
    sensors[8] = sensor_data.flame_sensor;
}


#if TEST
void read_sensor_values_mock(int16_t *sensors) {
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


