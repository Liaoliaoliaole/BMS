#pragma once

#define MODBUS_WAITING 0
#define MODBUS_FRAME_RECEIVED 1
#define MODBUS_WRONG_ADDRESS 2

void modbus_poll(sensor_values_t *sensor_data);

