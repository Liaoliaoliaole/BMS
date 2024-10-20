#pragma once

#include <stdint.h>

// Structure to hold sensor values
typedef struct {
    int16_t battery_temperature;     // Battery temperature value
    int16_t battery_temperature_alt; // Alternate temperature value for differential readings
    int16_t cell_voltage[4];         // Cell voltage readings for 4 cells
    int16_t battery_current_charge;  // Battery current value (charge)
    int16_t battery_current_discharge;        // Battery current value (discharge)
    int16_t flame_sensor;            // Flame sensor reading
} sensor_values_t;

// Function prototypes
void read_temperature_sensors(sensor_values_t *sensor_data);
void read_current_sensors(sensor_values_t *sensor_data);
void read_voltage_sensors(sensor_values_t *sensor_data);
void read_all_sensors(sensor_values_t *sensor_data);


#if TEST
void read_sensor_values_mock(int16_t *sensors);
#endif
