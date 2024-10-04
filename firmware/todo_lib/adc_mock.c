#include "bms_configuration.h"
#include "adc.h"

uint16_t adc_read(const uint8_t adc_channel) {
    switch (adc_channel) {
        case CHANNEL_BATTERY_TEMP0:
            return 1234;  // Mock ADC value for battery temperature
        case CHANNEL_BATTERY_TEMP1:
            return 2134;  // Mock ADC value for internal STM32 temperature
        case CHANNEL_SHUNT_CURRENT:
            return 3124;  // Mock ADC value for shunt current measurement
        case CHANNEL_CELL_VOLT_1:
            return 1233;  // Mock ADC value for cell voltage 1
        case CHANNEL_CELL_VOLT_2:
            return 1234;  // Mock ADC value for cell voltage 2
        case CHANNEL_CELL_VOLT_3:
            return 1235;  // Mock ADC value for cell voltage 3
        case CHANNEL_CELL_VOLT_4:
            return 1236;  // Mock ADC value for cell voltage 4
        default:
            return 0;     // Return 0 for undefined channels
    }
}

