#include "stm32l1xx.h"

#define CHANNEL_BATTERY_TEMP0  0
#define CHANNEL_BATTERY_TEMP1  1
#define CHANNEL_SHUNT_CURRENT 2
#define CHANNEL_CELL_VOLT_1   3
#define CHANNEL_CELL_VOLT_2   4
#define CHANNEL_CELL_VOLT_3   5
#define CHANNEL_CELL_VOLT_4   6

uint16_t adc_read(const uint8_t adc_channel);
