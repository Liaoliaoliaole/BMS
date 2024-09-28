#include "bms_configuration.h"
#include "adc.h"
#include "stm32l1xx.h"

/*
Initialize ADC.
ADC channel:
PA0 (A0)--Multiplexer--CH0
PA1 (A1)--Temperature 1--CH1
PA4 (A2)--Temperature 2--CH4
PC1 (A4)--TODO:shunt resistor current?
*/
void adc_init(void) {
	// Enable GPIOA and GPIOC
	RCC->AHBENR |= RCC_AHBENR_GPIOAEN | RCC_AHBENR_GPIOCEN;

	GPIOA->MODER |= (GPIO_MODE_ANALOG << (PA0 * 2))  // Set PA0 analog (A0)
				 | (GPIO_MODE_ANALOG << (PA1 * 2))  // Set PA1 analog (A1)
	             | (GPIO_MODE_ANALOG << (PA4 * 2));  // Set PA4 to analog mode (A2)
	//GPIOC->MODER |= (GPIO_MODE_ANALOG << (PC1 * 2));   // Set PC1 to analog mode (A4)

	//setup ADC1. p272
	RCC->APB2ENR |= 1 << 9;		//enable ADC1 clock,setting bit 9
	ADC1->CR2 = 0;					//bit 1=0: Single conversion mode

	// Set sampling time for channels
	ADC1->SMPR2 = (ADC_SAMPLING_TIME_384CYC << 0) |  // CH 0
				  (ADC_SAMPLING_TIME_384CYC << 3) |  // CH 1
				  (ADC_SAMPLING_TIME_384CYC << 12);  // CH 4

	ADC1->CR1 &=  ~(0x3 << 24);		//resolution 12-bit
}

/* uint16_t adc_read(const uint8_t adc_channel) {
	ADC_DISABLE();
	ADC1->SQR5 = adc_channel;      // Conversion sequence starts at selected channel
	ADC_ENABLE();
	ADC_START_CONVERSION();  // Start conversion
	while (!(ADC1->SR & 2)){}  // Wait for conversion complete
	uint16_t adc_value = ADC1->DR;     // Read conversion result
	return adc_value;
} */


/*Mock ADC read*/
uint16_t adc_read(const uint8_t adc_channel) {
    // Simulated ADC values based on the selected channel.
    switch (adc_channel) {
        case 0: // CH0 - Simulate cell voltage measurement (PA0)
            return 3000;  // Example ADC value for testing cell voltage(2.42V)
        case 1: // CH1 - Simulate ADC input from LM35 (PA1) output
            return 1475;  // Example ADC value for mid-range temperature (1.197V for LM35)
        case 2: // CH2 - Simulate ADC input from LM35 (PA1) diode
            return 1190;  // Example ADC value for mid-range temperature (0.965V for LM35)
        case 4: // CH4 - Simulate shunt resistor current measurement (PA4)
            return 2500;  // Example ADC value for testing current measurement
        default:
            return 0;     // Default case if no valid channel is provided
    }
}
