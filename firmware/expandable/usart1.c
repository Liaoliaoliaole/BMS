#include "usart1.h"
#include "bms_configuration.h"

void USART1_Init(void)
{
	RCC->APB2ENR|=(1<<14);	 	//set bit 14 (USART1 EN) p.156
	RCC->AHBENR|=0x00000001; 	//enable GPIOA port clock bit 0 (GPIOA EN)
	GPIOA->AFR[1]=0x00000700;	//GPIOx_AFRL p.189,AF7 p.177
	GPIOA->AFR[1]|=0x00000070;	//GPIOx_AFRL p.189,AF7 p.177
	GPIOA->MODER|=0x00080000; 	//MODER2=PA9(TX)D8 to mode 10=alternate function mode. p184
	GPIOA->MODER|=0x00200000; 	//MODER2=PA10(RX)D2 to mode 10=alternate function mode. p184

	USART1->BRR = 0x00000D05;	//9600 BAUD and crystal 32MHz. p710, D05
	USART1->CR1 = 0x00000008;	//TE bit. p739-740. Enable transmit
	USART1->CR1 |= 0x00000004;	//RE bit. p739-740. Enable receiver
	USART1->CR1 |= 0x00002000;	//UE bit. p739-740. Uart enable
}

void USART1_write(char data)
{
	//wait while TX buffer is empty
	while(!(USART1->SR&0x0080)){} 	//TXE: Transmit data register empty. p736-737
		USART1->DR=(data);			//p739
}

char USART1_read()
{
	char data=0;
									//wait while RX buffer is data is ready to be read
	while(!(USART1->SR&0x0020)){} 	//Bit 5 RXNE: Read data register not empty
		data=USART1->DR;			//p739
		return data;
}

void USART1_IRQHandler(void)
{
	char received_slave_address=0;

	if(USART1->SR & 0x0020) 		//if data available in DR register. p737
	{
		received_slave_address=USART1->DR;
	}
	if(received_slave_address==SLAVE_ADDR) //if we have right address
	{
		mFlag=1;

	}
	else
	{
		mFlag=2;


	}
	USART1->CR1 &= ~0x0020;			//disable RX interrupt

}
