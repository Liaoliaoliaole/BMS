#include "utils.h"

/**
 * @brief Delays execution for the specified number of seconds.
 *
 * Uses Timer 5 for a blocking delay.
 *
 * @param delay Number of seconds to delay.
 */
void delay_s(uint32_t delay)
{
    uint32_t i = 0;
    RCC->APB1ENR |= (1 << 3);   // Enable Timer 5 clock (TIM5EN: Timer 5 clock enable, p160)
    TIM5->PSC = 32000 - 1;      // 32 MHz / 32000 = 1000 Hz
    TIM5->ARR = 1000 - 1;       // Auto-reload value: 1000 cycles = 1 second
    TIM5->CNT = 0;              // Counter start value = 0
    TIM5->CR1 = 1;              // TIM5 Counter enabled (p421)

    while (i < delay)
    {
        while (!((TIM5->SR) & 1)) {}  // Wait for update interrupt flag (p427)
        i++;
        TIM5->SR &= ~1;   // Clear flag (p427)
        TIM5->CNT = 0;    // Reset counter start value
    }
    TIM5->CR1 = 0;        // Disable TIM5 Counter (p421)
}

/**
 * @brief Delays execution for the specified number of milliseconds.
 *
 * Uses Timer 11 for a blocking delay.
 *
 * @param delay Number of milliseconds to delay.
 */
void delay_ms(uint32_t delay)
{
    uint32_t i = 0;
    RCC->APB2ENR |= (1 << 4);   // Enable Timer 11 clock (TIM11EN: Timer 11 clock enable, p156)
    TIM11->PSC = 32 - 1;        // Prescaler: 32 MHz / 32 = 1 MHz
    TIM11->ARR = 1000 - 1;      // Auto-reload value: 1 MHz / 1000 = 1 kHz (1 ms per tick)
    TIM11->CNT = 0;             // Counter start value = 0
    TIM11->CR1 = 1;             // TIM11 Counter enabled (p421)

    while (i < delay)
    {
        while (!((TIM11->SR) & 1)) {}  // Wait for update interrupt flag (p427)
        i++;
        TIM11->SR &= ~1;  // Clear flag (p427)
        TIM11->CNT = 0;   // Reset counter start value
    }
    TIM11->CR1 = 0;       // Disable TIM11 Counter (p421)
}

/**
 * @brief Delays execution for the specified number of microseconds.
 *
 * Uses Timer 11 for a blocking delay.
 *
 * @param delay Number of microseconds to delay.
 */
void delay_us(uint32_t delay)
{
    uint32_t i = 0;
    RCC->APB2ENR |= (1 << 4);   // Enable Timer 11 clock (TIM11EN: Timer 11 clock enable, p160)
    TIM11->PSC = 1;             // Prescaler: 32 MHz / 2 = 16 MHz
    TIM11->ARR = 1;             // Auto-reload value: 16 MHz / 1 = 16 MHz (1 us per tick)
    TIM11->CNT = 0;             // Counter start value = 0
    TIM11->CR1 = 1;             // TIM11 Counter enabled (p421)

    while (i < delay)
    {
        while (!((TIM11->SR) & 1)) {}  // Wait for update interrupt flag (p427)
        i++;
        TIM11->SR &= ~1;  // Clear flag (p427)
        TIM11->CNT = 0;   // Reset counter start value
    }
    TIM11->CR1 = 0;       // Disable TIM11 Counter (p421)
}

/**
 * @brief Delays execution for the specified number of 10 microsecond intervals.
 *
 * Uses Timer 6 for a blocking delay.
 *
 * @param delay Number of 10 microsecond intervals to delay.
 */
void delay_10us(uint32_t delay)
{
    uint32_t i = 0;
    RCC->APB1ENR |= (1 << 4);   // Enable Timer 6 clock (TIM6EN: Timer 6 clock enable, p160)
    TIM6->PSC = 32 - 1;         // Prescaler: 32 MHz / 32 = 1 MHz
    TIM6->ARR = 10 - 1;         // Auto-reload value: 1 MHz / 10 = 100 kHz (10 us per tick)
    TIM6->CNT = 0;              // Counter start value = 0
    TIM6->CR1 = 1;              // TIM6 Counter enabled

    while (i < delay)
    {
        while (!((TIM6->SR) & 1)) {}  // Wait for update interrupt flag (p427)
        i++;
        TIM6->SR &= ~1;  // Clear flag (p427)
        TIM6->CNT = 0;   // Reset counter start value
    }
    TIM6->CR1 = 0;       // Disable TIM6 Counter
}

/**
 * @brief Sorts an array of integers using Bubble Sort.
 *
 * @param arr Array of integers to be sorted.
 * @param n Number of elements in the array.
 */
void data_sort(int32_t arr[], uint32_t n)
{
    uint32_t i, j;
    for (i = 0; i < n - 1; i++)
    {
        for (j = 0; j < n - i - 1; j++)
        {
            if (arr[j] > arr[j + 1])
            {
                int temp = arr[j];
                arr[j] = arr[j + 1];
                arr[j + 1] = temp;
            }
        }
    }
}

/**
 * @brief Applies a moving average filter to an array of sensor readings.
 *
 * @param data Array of sensor readings to be filtered.
 * @param length Number of elements in the data array.
 * @return The filtered value as an average of the data array.
 */
int32_t moving_average_filter(int32_t *data, uint32_t length) {
    int32_t sum = 0;
    for (uint32_t i = 0; i < length; i++) {
        sum += data[i];
    }
    return sum / length;
}

/**
 * @brief Gets the current time in milliseconds using Timer 5.
 *
 * @return Current time in milliseconds.
 */
time_t get_current_time_ms(void) {
    // Use TIM5 as a simple millisecond timer
    RCC->APB1ENR |= RCC_APB1ENR_TIM5EN;  // Enable Timer 5 clock
    TIM5->PSC = 32000 - 1;  // Prescaler: 32 MHz / 32000 = 1 kHz (1 ms per tick)
    TIM5->ARR = 0xFFFF;  // Max auto-reload value
    TIM5->CNT = 0;  // Reset counter
    TIM5->CR1 |= TIM_CR1_CEN;  // Enable Timer 5

    return TIM5->CNT;  // Return current count as time in ms
}
