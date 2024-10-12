#include "utils.h"

void delay_s(uint32_t delay)
{
	uint32_t i=0;
	RCC->APB1ENR|=(1<<3); 	//TIM5EN: Timer 5 clock enable.P160
	TIM5->PSC = 32000 - 1;  // 32 MHz / 32000 = 1000 Hz.
	TIM5->ARR = 1000 - 1;   // Auto-reload value: 1000 cycles = 1 second.
	TIM5->CNT = 0;	//counter start value = 0
	TIM5->CR1 = 1; 			//TIM5 Counter enabled. p421

	  while(i<delay)
	  {
		  while(!((TIM5->SR)&1)){} //Update interrupt flag. p427
		  i++;
		  TIM5->SR &= ~1; 	//flag cleared. p427
		  TIM5->CNT=0;	  	//counter start value = 0
	  }
	  TIM5->CR1=0; 		//TIM11 Counter disabled. p421
}

void delay_ms(uint32_t delay)
{
	uint32_t i=0;
	RCC->APB2ENR|=(1<<4); 	//TIM11EN: Timer 11 clock enable. p156
	TIM11->PSC=32-1; 		//32 000 000 MHz / 32 = 1 000 000 Hz. p435,The counter clock frequency CK_CNT is equal to fCK_PSC / (PSC[15:0] + 1).
	TIM11->ARR=1000-1; 		//TIM11 counter. 1 000 000 Hz / 1000 = 1000 Hz ~ 1ms. p435,Auto-reload value'	ARR is the value to be loaded in the actual auto-reload register.
	TIM11->CNT=0;			//counter start value = 0
	TIM11->CR1=1; 			//TIM11 Counter enabled. p421

	  while(i<delay)
	  {
		  while(!((TIM11->SR)&1)){} //Update interrupt flag. p427
		  i++;
		  TIM11->SR &= ~1; 	//flag cleared. p427
		  TIM11->CNT=0;	  	//counter start value = 0
	  }
	  TIM11->CR1=0; 		//TIM11 Counter disabled. p421
}

void delay_us(uint32_t delay)
{
	uint32_t i=0;
	RCC->APB2ENR|=(1<<4); 	//TIM11EN: Timer 11 clock enable. p160
	TIM11->PSC=1; 		//32 000 000 MHz / 32 = 1 000 000 Hz. p435
	TIM11->ARR=1;	 		//TIM11 counter. 1 000 000 Hz / 1000 = 1000 Hz ~ 1ms. p435
	TIM11->CNT=0;			//counter start value = 0
	TIM11->CR1=1; 			//TIM11 Counter enabled. p421

	  while(i<delay)
	  {
		  while(!((TIM11->SR)&1)){} //Update interrupt flag. p427
		  i++;
		  TIM11->SR &= ~1; 	//flag cleared. p427
		  TIM11->CNT=0;	  	//counter start value = 0
	  }
	  TIM11->CR1=0; 		//TIM11 Counter disabled. p421
}

void delay_10us(uint32_t delay)
{
	uint32_t i=0;
	RCC->APB1ENR|=(1<<4); 	//TIM5EN: Timer 5 clock enable.P160
	TIM6->PSC = 32 - 1;
	TIM6->ARR = 10 - 1;
	TIM6->CNT = 0;
	TIM6->CR1 = 1;

	  while(i<delay)
	  {
		  while(!((TIM6->SR)&1)){} //Update interrupt flag. p427
		  i++;
		  TIM6->SR &= ~1; 	//flag cleared. p427
		  TIM6->CNT=0;	  	//counter start value = 0
	  }
	  TIM6->CR1=0; 		//TIM11 Counter disabled. p421
}

void data_sort(int32_t arr[], uint32_t n)
{
	uint32_t i, j;
	for (i = 0; i < n - 1; i++)
	{
		for (j = 0; j < n - i - 1; j++)
		{
			if (arr[j] > arr[j + 1])
			{
				int temp = arr[j];
				arr[j] = arr[j + 1];
				arr[j + 1] = temp;
			}
			else
			{

			}
		}
	}
}

time_t get_current_time_ms(void) {
    // Use TIM5 as a simple millisecond timer
    RCC->APB1ENR |= RCC_APB1ENR_TIM5EN;  // Enable Timer 5 clock
    TIM5->PSC = 32000 - 1;  // Prescaler: 32 MHz / 32000 = 1 kHz (1 ms per tick)
    TIM5->ARR = 0xFFFF;  // Max auto-reload value
    TIM5->CNT = 0;  // Reset counter
    TIM5->CR1 |= TIM_CR1_CEN;  // Enable Timer 5

    return TIM5->CNT;  // Return current count as time in ms
}
