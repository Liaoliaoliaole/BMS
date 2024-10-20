#include "intergration_test.h"
#include "sensor_reader.h"
#include "modbus.h"
#include "mosfet_control.h"
#include "adc.h"
#include "mux.h"
#include "system.h"
#include "utils.h"

uint16_t get_mock_mux_output_value(uint32_t channel, uint32_t mux_select_pin_mask) {
    // Define mock ADC values for each multiplexer channel
    // These values simulate the ADC read from the sensor in the raw range of 0-4095 (12-bit ADC)
    if (mux_select_pin_mask == MUX_TEMP_SELECT_PIN_MASK) {
        switch (channel) {
            case 0:
                return 1850;  // ADC value for LM35 sensor output (around 1.5V equivalent)
            case 1:
                return 1480;  // ADC value for second LM35 sensor output (around 1.2V equivalent)
            case 2:
                return 370;   // ADC value for diode reference voltage (around 0.3V equivalent)
            case 3:
                return 395;   // ADC value for diode reference voltage for the second sensor (around 0.32V equivalent)
            default:
                return 0;     // Default value if no valid channel
        }
    } else if (mux_select_pin_mask == MUX_CURR_SELECT_PIN_MASK) {
        switch (channel) {
            case 0:
                return 2000;  // ADC value for charging current (mock raw ADC value)
            case 1:
                return 1500;  // ADC value for discharging current (mock raw ADC value)
            default:
                return 0;     // Default value if no valid channel
        }
    } else if (mux_select_pin_mask == MUX_BATT1_SELECT_PIN_MASK) {
        switch (channel) {
            case 0:
                return 3100;  // ADC value for Battery Cell 1 (mock raw ADC value)
            case 1:
                return 3250;  // ADC value for Battery Cell 2 (mock raw ADC value)
            default:
                return 0;     // Default value if no valid channel
        }
    } else if (mux_select_pin_mask == MUX_BATT2_SELECT_PIN_MASK) {
        switch (channel) {
            case 0:
                return 3400;  // ADC value for Battery Cell 3 (mock raw ADC value)
            case 1:
                return 3550;  // ADC value for Battery Cell 4 (mock raw ADC value)
            default:
                return 0;     // Default value if no valid channel
        }
    } else {
        return 0; // Default value if no valid pin mask
    }
}


void intergration_test(sensor_values_t* sensor_data) {
		read_all_sensors(sensor_data);
		//mosfet_control_logic(&sensor_data);
		modbus_poll(sensor_data);
		delay_ms(500);
}

