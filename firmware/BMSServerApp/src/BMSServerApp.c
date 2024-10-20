#include <stdio.h>
#include <stdint.h>

#include "stm32l1xx.h"
#include "system_clock.h"
#include "bms_configuration.h"
#include "crc.h"
#include "usart2.h"
#include "utils.h"
#include "adc.h"
#include "adc_converter.h"
#include "sensor_reader.h"
#include "mux.h"
#include "mosfet_control.h"
#include "modbus.h"

#ifdef TEST
#include "intergration_test.h"
#endif

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

	USART2->CR1 |= 0x0020;			// Enable RX interrupt
	NVIC_EnableIRQ(USART2_IRQn); 	// Enable interrupt in NVIC
	NVIC_EnableIRQ(EXTI0_IRQn); // Enable EXTI0 interrupt
	__enable_irq();

	adc_init();
	mux_init();
	mosfet_init();

	sensor_values_t sensor_data;

#ifdef TEST
	intergration_test(&sensor_data);
#endif

    while(1)
    {
    	//read_all_sensors(&sensor_data);
    	//mosfet_control_logic(&sensor_data);
    	//modbus_poll(&sensor_data);
    	intergration_test(&sensor_data);
    }
    return 0;
}
