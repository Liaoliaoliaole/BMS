#pragma once

#include <stdint.h>

// Structure to hold sensor values
typedef struct {
    int16_t battery_temperature;           // Battery temperature value in hundredths of a degree Celsius
    int16_t battery_temperature_alt;       // Alternate temperature value for differential readings in hundredths of a degree Celsius
    uint16_t cell_voltage[4];              // Cell voltage readings for 4 cells in millivolts (always positive)
    uint16_t battery_current_charge;       // Battery current value (charge) in milliamperes (always positive)
    uint16_t battery_current_discharge;    // Battery current value (discharge) in milliamperes (always positive)
} sensor_values_t;


// Function prototypes
void read_temperature_sensors(sensor_values_t *sensor_data);
void read_current_sensors(sensor_values_t *sensor_data);
void read_voltage_sensors(sensor_values_t *sensor_data);
void read_all_sensors(sensor_values_t *sensor_data);
