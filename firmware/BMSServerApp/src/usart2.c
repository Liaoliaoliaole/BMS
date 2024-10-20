#include <string.h>

#include "usart2.h"
#include "stm32l1xx.h"

/**
 * @brief Initialize USART2 for communication.
 *
 * This function configures GPIO pins, enables clocks, sets the baud rate,
 * and enables USART2 for transmission and reception.
 */
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

	// Configure DMA for USART2 TX
	RCC->AHBENR |= RCC_AHBENR_DMA1EN; // Enable DMA1 clock
	DMA1_Channel7->CPAR = (uint32_t)&USART2->DR; // Set peripheral address to USART2 data register
}

/**
 * @brief Read a byte of data from USART2.
 *
 * This function waits until data is available in the RX buffer,
 * then reads and returns the received byte.
 *
 * @return The byte of data received from USART2.
 */
uint8_t USART2_read()
{
	uint8_t data=0;
									//wait while RX buffer is data is ready to be read
	while(!(USART2->SR&0x0020)){} 	//Bit 5 RXNE: Read data register not empty
		data=USART2->DR;			//p739
		return data;
}

/**
 * @brief Write a byte of data to USART2.
 *
 * This function waits until the TX buffer is empty,
 * then writes the provided byte to the data register for transmission.
 *
 * @param data The byte of data to be sent.
 */
void USART2_write(char data)
{
	//wait while TX buffer is empty
	while(!(USART2->SR&0x0080)){} 	//TXE: Transmit data register empty. p736-737
		USART2->DR=(data);			//p739
}

/**
 * @brief Send a null-terminated string via USART2.
 *
 * This function sends each character in the string one by one using USART2.
 *
 * @param string Pointer to the null-terminated string to be sent.
 */
void USART2_send_string(const char *string) {
    while (*string) {
        USART2_write(*string++); // Send each character one by one
    }
}

/**
 * @brief Send a block of data via USART2.
 *
 * This function sends the provided data, byte by byte, using USART2.
 *
 * @param data Pointer to the data to be sent.
 * @param len Length of the data in bytes.
 */
void USART2_send_data(const void *data, size_t len) {
    const uint8_t *d = (const uint8_t *)data;
    for (size_t i = 0; i < len; i++) {
        USART2_write(d[i]); // Send each byte of data
    }
}

/**
 * @brief Send data over USART2 using DMA.
 *
 * This function sends the provided data using DMA for efficient transmission.
 *
 * @param data Pointer to the data to be sent.
 * @param len Length of the data in bytes.
 */
void USART2_send_data_dma(const void *data, size_t len) {
    // Configure DMA channel
    DMA1_Channel7->CCR = 0; // Disable DMA to configure
    DMA1_Channel7->CMAR = (uint32_t)data; // Set memory address
    DMA1_Channel7->CNDTR = len; // Set number of bytes to transfer
    DMA1_Channel7->CCR = DMA_CCR_MINC | DMA_CCR_DIR | DMA_CCR_TCIE; // Enable memory increment, direction (memory to peripheral), transfer complete interrupt
    DMA1_Channel7->CCR |= DMA_CCR_EN; // Enable DMA channel

    // Enable USART2 DMA transmitter
    USART2->CR3 |= USART_CR3_DMAT;

    // Wait until DMA transfer is complete (can also be handled via DMA interrupt)
    while (!(DMA1->ISR & DMA_ISR_TCIF7));
    DMA1->IFCR |= DMA_IFCR_CTCIF7; // Clear transfer complete flag
}
