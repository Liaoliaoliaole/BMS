#pragma once
#include "stm32l1xx.h"
#include "usart2.h"
#include "bms_configuration.h"
#include <stdbool.h>
#include <time.h>

/**
 * @brief Delays execution for the specified number of seconds.
 *
 * This function uses Timer 5 to create a blocking delay.
 *
 * @param delay Number of seconds to delay.
 */
void delay_s(uint32_t delay);      // Timer 5

/**
 * @brief Delays execution for the specified number of milliseconds.
 *
 * This function uses Timer 11 to create a blocking delay.
 *
 * @param delay Number of milliseconds to delay.
 */
void delay_ms(uint32_t delay);     // Timer 11

/**
 * @brief Delays execution for the specified number of microseconds.
 *
 * This function uses Timer 11 to create a blocking delay.
 *
 * @param delay Number of microseconds to delay.
 */
void delay_us(uint32_t delay);     // Timer 11

/**
 * @brief Delays execution for the specified number of 10 microsecond intervals.
 *
 * This function uses Timer 6 to create a blocking delay.
 *
 * @param delay Number of 10 microsecond intervals to delay.
 */
void delay_10us(uint32_t delay);   // Timer 6

/**
 * @brief Sorts an array of integers using Bubble Sort.
 *
 * This function sorts an array of integers in ascending order.
 * Note: The time complexity is O(n^2). Consider using a more efficient algorithm for large arrays.
 *
 * @param arr Array of integers to be sorted.
 * @param n Number of elements in the array.
 */
void data_sort(int32_t arr[], uint32_t n);

/**
 * @brief Applies a moving average filter to an array of sensor readings.
 *
 * @param data Array of sensor readings to be filtered.
 * @param length Number of elements in the data array.
 * @return The filtered value as an average of the data array.
 */
int32_t moving_average_filter(int32_t *data, uint32_t length);

/**
 * @brief Gets the current time in milliseconds.
 *
 * This function uses Timer 5 to return the current time in milliseconds.
 * The timer must be initialized before calling this function.
 *
 * @return Current time in milliseconds.
 */
time_t get_current_time_ms(void);
