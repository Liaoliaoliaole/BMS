#pragma once
#include "stm32l1xx.h"

void USART2_Init(void);

void USART2_write(uint8_t data);

uint8_t USART2_read(void);