
#include "stm32l1xx.h"
#include "system_clock.h"
#include "bms_configuration.h"
#include "crc.h"
#include "usart2.h"
#include "utils.h"
#include <stdio.h>

void extract_registers_info(unsigned char* received_frame, unsigned short int* reg_addr, unsigned short int* reg_quantity);
void response_frame_constructor(int *sensor_value);
void wrong_slave_address(void);


void read_sensor_values(int *sensors);
int sensor_value[NUM_REGISTERS] = {0};
char mFlag_ = 0;
char received_slave_address=0;
char buffer[BUFFER_SIZE];

/**
**===========================================================================
**
**  Abstract: main program
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

	/*
	 * TEST for mux implementation.
	for (int channel = 0; channel < 8; channel++) {
	   mux_select(channel);  // Select a channel (PB3, PB4, PB5)
	   delay_ms(1000);  // Wait 1 second to observe LED behavior
	}
	*/

	unsigned char received_frame[8] = {0};

    while(1)
    {
    	switch (mFlag_) {
            case 0:
                for (int k = 0; k < 8;k++)
                {
                    received_frame[k] = '0';
                }
                //memset(received_frame, '0', 8);
                break;

    		case 1:
    			received_frame[0] = SLAVE_ADDR;
    			for (int i = 1; i < 8; i++)
				{
					received_frame[i] = USART2_read();
				}
				unsigned short int CRC_cal_received = CRC16 ((char *)received_frame,6);
				unsigned char crc_low = CRC_cal_received & 0xFF;
				unsigned char crc_high = CRC_cal_received >> 8;

				delay_ms(40);
				if((crc_high == received_frame[7]) && (crc_low == received_frame[6]))
				{
					unsigned short int reg_addr[MAX_REGISTERS] = {0};
					unsigned short int reg_quantity = 0;
					extract_registers_info(received_frame, reg_addr, &reg_quantity);

					if (reg_quantity != 0)
					{
                        int meas[NUM_REGISTERS] = {0};
                        read_sensor_values(meas);  // reading 6 sensor values

                        for (char i = START_ADDR; i < NUM_REGISTERS; i++)
                        {
                            sensor_value[i] = meas[i];
                        }

						response_frame_constructor(sensor_value);
					}
					else
					{
						//error handler
					}
    		  	}
				mFlag_=0;
				USART2->CR1 |= 0x0020;			//enable RX interrupt
				break;

    		  	case 2:
    		  		wrong_slave_address();

    		  		break;
    		  	}
    }
    return 0;
}

void extract_registers_info(unsigned char* received_frame, unsigned short int* reg_addr, unsigned short int* reg_quantity) {
    unsigned short int start_addr = (received_frame[2] << 8) | received_frame[3];
    *reg_quantity = (received_frame[4] << 8) | received_frame[5];

    if (*reg_quantity > MAX_REGISTERS) {
        // Handle error

    }

    for (int i = 0; i < *reg_quantity; i++)
    {
        reg_addr[i] = start_addr + i;
    }
}

void wrong_slave_address(void)
{
	   USART2->CR1 &= ~0x00000004;		//RE bit. p739-740. Disable receiver
	   delay_ms(10); 					//time=1/9600 x 10 bits x 7 byte = 7,29 ms
	   USART2->CR1 |= 0x00000004;		//RE bit. p739-740. Enable receiver
	   USART2->CR1 |= 0x0020;			//enable RX interrupt
	   mFlag_=0;
}


void response_frame_constructor(int *sensor_value)
{
	//PA5 led ON, transmiting mode activated
	GPIOA->ODR |= 0x20;

	unsigned char response_length = RESPONSE_BYTE_COUNT + 5;
	unsigned char response_frame[RESPONSE_BYTE_COUNT + 5] = {0};

    response_frame[0] = SLAVE_ADDR;
    response_frame[1] = FUNCTION_CODE;
    response_frame[2] = RESPONSE_BYTE_COUNT;

    // Each register has data first high-byte and then low byte
    for(char i = 0; i < NUM_REGISTERS ; i++)
    {
        // Update high byte for sensor
    	unsigned char data_hi = (sensor_value[i] >> 8) & 0xFF;
        // Update low byte for sensor
    	unsigned char data_lo = sensor_value[i] & 0xFF;

        response_frame[2*i+3] = data_hi;
        response_frame[2*i+4] = data_lo;
    }


	unsigned short int CRC_calc_response = 0;

	CRC_calc_response = CRC16((char*)response_frame, (response_length-2));
    response_frame[response_length - 2] = CRC_calc_response & 0xFF; // Low byte of CRC
    response_frame[response_length - 1] = (CRC_calc_response >> 8) & 0xFF; // High byte of CRC

	for (int i = 0; i < response_length; ++i) {
		USART2_write(response_frame[i]);
	}
	delay_ms(10);
}

//Check slave address is correct, give value to mFlag_
void USART2_IRQHandler(void)
{
	if(USART2->SR & 0x0020) 		//if data available in DR register. p737
	{
		received_slave_address=USART2->DR;
	}
	if(received_slave_address==SLAVE_ADDR) //if we have right address
	{
		mFlag_=1;
	}
	else
	{
		mFlag_=2;
	}
	USART2->CR1 &= ~0x0020;			//disable RX interrupt

}

void read_sensor_values(int *sensors) {
    sensors[0] = 2200;  // Mock temperature1: 22.00°C
    sensors[1] = 2300;  // Mock temperature2: 23.00°C
    sensors[2] = 3700;  // Mock cell voltage1: 3.7V (in millivolts: 3700 mV)
    sensors[3] = 3650;  // Mock cell voltage2: 3.65V (in millivolts: 3650 mV)
    sensors[4] = 3800;  // Mock cell voltage3: 3.8V (in millivolts: 3800 mV)
    sensors[5] = 3550;  // Mock cell voltage4: 3.55V (in millivolts: 3550 mV)
}

