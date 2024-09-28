#include <stdio.h>
#include <stdint.h>

#include "stm32l1xx.h"
#include "system_clock.h"
#include "bms_configuration.h"
#include "crc.h"
#include "mux.h"
#include "usart2.h"
#include "utils.h"
#include "adc.h"
#include "adc_converter.h"
#include "modbus.h"

// Main sensor values buffer
int16_t sensor_value[NUM_REGISTERS] = {0};

/**
**===========================================================================
**
**  Main program
**
**===========================================================================
*/
int main(void) {
	__disable_irq();
	USART2_Init();

	/* Configure the system clock to 32 MHz and update SystemCoreClock */
	SetSysClock();
	SystemCoreClockUpdate();

	USART2->CR1 |= 0x0020;			//enable RX interrupt
	NVIC_EnableIRQ(USART2_IRQn); 	//enable interrupt in NVIC
	__enable_irq();

	mux_init();
	adc_init();

    while(1)
    {
    	modbus_poll(sensor_value);
    }
    return 0;
}