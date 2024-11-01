#include <string.h>

#include "usart2.h"
#include "stm32l1xx.h"

void USART2_Init(void)
{
	RCC->APB1ENR|=0x00020000; 	//set bit 17 (USART2 EN)
	RCC->AHBENR|=0x00000001; 	//enable GPIOA port clock bit 0 (GPIOA EN)
	GPIOA->AFR[0]=0x00000700;	//GPIOx_AFRL p.188,AF7 p.177
	GPIOA->AFR[0]|=0x00007000;	//GPIOx_AFRL p.188,AF7 p.177
	GPIOA->MODER|=0x00000020; 	//MODER2=PA2(TX) to mode 10=alternate function mode. p184
	GPIOA->MODER|=0x00000080; 	//MODER2=PA3(RX) to mode 10=alternate function mode. p184

	USART2->BRR = 0x00000D05;	//9600 BAUD and crystal 32MHz. p710, D05
	USART2->CR1 = 0x00000008;	//TE bit. p739-740. Enable transmit
	USART2->CR1 |= 0x00000004;	//RE bit. p739-740. Enable receiver
	USART2->CR1 |= 0x00002000;	//UE bit. p739-740. Uart enable
}

uint8_t USART2_read()
{
	uint8_t data=0;
									//wait while RX buffer is data is ready to be read
	while(!(USART2->SR&0x0020)){} 	//Bit 5 RXNE: Read data register not empty
		data=USART2->DR;			//p739
		return data;
}

void USART2_write(char data)
{
	//wait while TX buffer is empty
	while(!(USART2->SR&0x0080)){} 	//TXE: Transmit data register empty. p736-737
		USART2->DR=(data);			//p739
}

void USART2_send_string(const char *string) {
    while (*string) {
        USART2_write(*string++); // Send each character one by one
    }
}

void USART2_receive_string(char *buffer, size_t buffer_size) {
    size_t i = 0;
    char received;

    // Wait for the start of the command with three `~` symbols
    do {
        received = USART2_read();
    } while (received == '~');

    // Add the first character of the command that is not `~`
    buffer[i++] = received;

    // Read the remaining characters of the command until `\n`
    while (i < (buffer_size - 1)) {
        received = USART2_read();
        if (received == '\n') { 
            break;
        }
        buffer[i++] = received;
    }
    buffer[i] = '\0';  // Terminate the string with a null character

    // Remove all leading '~' from the string
    char *command_start = buffer;
    while (*command_start == '~') {
        command_start++;
    }
    memmove(buffer, command_start, strlen(command_start) + 1);  // Shift the command to the start of the buffer
}


void USART2_send_data(const void *data, size_t len) {
    const uint8_t *d = (const uint8_t *)data;
    for (size_t i = 0; i < len; i++) {
        USART2_write(d[i]); // Send each byte of data
    }
}
