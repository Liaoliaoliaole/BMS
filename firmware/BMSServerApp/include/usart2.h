#pragma once
#include <stddef.h>
#include <stdint.h>

void USART2_Init(void);

uint8_t USART2_read(void);

void USART2_write(char data);

void USART2_send_data(const void *data, size_t len);

void USART2_send_string(const char* string);

void USART2_receive_string(char *buffer, size_t buffer_size);