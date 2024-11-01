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
#include "test_handler.h"

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

	adc_init();
	mux_init();
	mosfet_init();

#ifdef TEST
	// This test blinks on board LED to check mutiplexer channel selection succedded.
    mux_test();
    mosfet_control_logic(sensor_data);
#endif

    while(1)
    {
    	modbus_poll();

        handle_serial_command();

    }
    return 0;
}
