#include "adc.h"

void adc_init(void) {
	 //set up pin PA0 and PA1 for analog input
	  GPIOA->MODER|=0x3;			//PA0 analog (A0)
	  GPIOA->MODER|=0xC;			//PA1 analog (A1)

	  //setup ADC1. p272
	  RCC->APB2ENR|=0x00000200;		//enable ADC1 clock
	  ADC1->CR2=0;					//bit 1=0: Single conversion mode
	  ADC1->SMPR3=7;				//384 cycles sampling time for channel 0 (longest)
	  ADC1->CR1&=~0x03000000;		//resolution 12-bit
}

uint16_t adc_read(const uint8_t adc_channel) {
	int result=0;
	// Set the ADC channel
	ADC1->SQR5 = channel;      // Conversion sequence starts at selected channel
	ADC1->CR2 |= 1;            // Bit 0, ADC on/off (1=on, 0=off)
	ADC1->CR2 |= 0x40000000;   // Start conversion
	while (!(ADC1->SR & 2)){}  // Wait for conversion complete
	result = ADC1->DR;     // Read conversion result

	ADC1->CR2 &= ~1;           // Bit 0, ADC on/off (1=on, 0=off)
	return result;
}
