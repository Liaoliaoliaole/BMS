#include "bms_configuration.h"
#include "stm32l1xx.h"
#include "adc.h"
#include "system.h"
void adc_init(void) {
    // Enable GPIOA and GPIOC
    RCC->AHBENR |= RCC_AHBENR_GPIOAEN | RCC_AHBENR_GPIOCEN;

    // Set GPIOA pins PA0, PA1, PA4, PA5, and PA6 to analog mode
    GPIOA->MODER |= (0x3 << (0 * 2))  // Set PA0 to analog mode (A0)
                 | (0x3 << (1 * 2))  // Set PA1 to analog mode (A1)
                 | (0x3 << (4 * 2))  // Set PA4 to analog mode (A2)
                 | (0x3 << (5 * 2))  // Set PA5 to analog mode (A3)
                 | (0x3 << (6 * 2)); // Set PA6 to analog mode (A5)

    // Set GPIOC pin PC1 to analog mode
    GPIOC->MODER |= (0x3 << (1 * 2));   // Set PC1 to analog mode (A4)

    // Enable ADC1 clock
    RCC->APB2ENR |= (1 << 9);  // Set bit 9 to enable ADC1 clock

    // Set ADC to single conversion mode
    ADC1->CR2 = 0;  // Single conversion mode

    // Set sampling time for channels 0, 1, 4, 5, 6
    ADC1->SMPR2 = (0x7 << (0 * 3))  // CH0 (PA0)
                | (0x7 << (1 * 3))  // CH1 (PA1)
                | (0x7 << (4 * 3))  // CH4 (PA4)
                | (0x7 << (5 * 3))  // CH5 (PA5)
                | (0x7 << (6 * 3)); // CH6 (PA6)

    // Set sampling time for channel 11 separately because it's outside the range of the 32-bit SMPR2 register
    // In STM32, SMPR1 is used for channels from 10 to 18
    ADC1->SMPR1 = (0x7 << ((11 - 10) * 3));  // CH11 (PC1)

    // Set resolution to 12-bit
    ADC1->CR1 &= ~(0x3 << 24);

    // Enable the ADC
    ADC1->CR2 |= (1 << 0);  // Set ADON bit to enable the ADC
}


uint16_t adc_read(const uint8_t adc_channel) {
	ADC1->CR2 &= ~(1 << 0);  // Clear ADON bit to disable the ADC

	// Set the channel to be read
	ADC1->SQR3 = adc_channel;  // Set the selected channel in the regular sequence register

	// Enable the ADC
	ADC1->CR2 |= (1 << 0);  // Set ADON bit to enable the ADC

	// Start the conversion
	ADC1->CR2 |= (1 << 30);  // Set SWSTART bit to start the conversion

	// Wait for the conversion to complete
	while (!(ADC1->SR & (1 << 1)));  // Wait until EOC (End of Conversion) bit is set

	// Read the conversion result
	uint16_t adc_value = ADC1->DR;

	return adc_value;
}

#if TEST
// Mock ADC read function to simulate ADC readings
uint16_t adc_read_mock(const uint8_t adc_channel) {
    // Simulated ADC values based on the selected channel.
    switch (adc_channel) {
        case 0: // CH0 - Simulate cell voltage measurement (PA0)
            return 3000;  // Example ADC value for testing cell voltage (2.42V)
        case 1: // CH1 - Simulate ADC input from temperature sensor 1 (PA1)
            return 1475;  // Example ADC value for mid-range temperature (1.197V for LM35)
        case 4: // CH4 - Simulate temperature sensor 2 measurement (PA4)
            return 1190;  // Example ADC value for mid-range temperature (0.965V for LM35)
        case 5: // CH5 - Simulate shunt resistor current measurement (PA5)
            return 2500;  // Example ADC value for testing current measurement
        case 6: // CH6 - Simulate flame sensor measurement (PA6)
            return 1800;  // Example ADC value for flame sensor
        case 11: // CH11 - Simulate future use measurement (PC1)
            return 2000;  // Example ADC value for testing future use
        default:
            return 0;     // Default case if no valid channel is provided
    }
}
#endif
