#pragma once
#include "stm32l1xx.h"
#include "usart2.h"
#include "bms_configuration.h"

void delay_s(uint32_t delay);      // Timer 5
void delay_ms(uint32_t delay);     // Timer 11
void delay_us(uint32_t delay);     // Timer 11
void delay_10us(uint32_t delay);   // Timer 6

// Median filter
void data_sort(int32_t arr[], uint32_t n); // Assuming the median filter works with int32_t

