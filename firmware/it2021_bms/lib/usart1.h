#include "stm32l1xx.h"

char mFlag = 0;

void USART1_write(char data);

void USART1_Init(void);

char USART1_read();

void USART1_IRQHandler(void);
