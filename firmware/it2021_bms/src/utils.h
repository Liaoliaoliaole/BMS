#include "stm32l1xx.h"

void delay_s(unsigned long delay);//Timer 5
void delay_ms(unsigned long delay);//Timer 11
void delay_us(unsigned long delay);//Timer 11
void delay_10us(unsigned long delay); //Timer 6

//median filter
void data_sort(int arr[], int n);
