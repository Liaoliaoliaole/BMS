#include "bms_configuration.h"
#include "adc.h"

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

uint16_t adc_read(const uint8_t adc_channel) {
	ADC_DISABLE();
	ADC1->SQR5 = adc_channel;      // Conversion sequence starts at selected channel
	ADC_ENABLE();
	ADC_START_CONVERSION();  // Start conversion
	while (!(ADC1->SR & 2)){}  // Wait for conversion complete
	uint16_t adc_value = ADC1->DR;     // Read conversion result
	return adc_value;
}

//TODO: Read p270 ADC Power on-off
//     Read p272 Single conversion mode
