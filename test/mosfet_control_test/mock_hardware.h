#pragma once

#include <stdint.h>
#include <stdbool.h>

void GPIO_SetPin(uint16_t pin);
void GPIO_ResetPin(uint16_t pin);
void USART2_send_string(const char *string);
uint32_t get_current_time_ms(void);
bool is_gpio_pin_set(uint16_t pin);
