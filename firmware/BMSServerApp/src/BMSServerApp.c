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

void extract_registers_info(uint8_t* received_frame, uint16_t* reg_addr, uint16_t* reg_quantity);
void response_frame_constructor(int16_t *sensor_value);
void wrong_slave_address(void);
void read_sensor_values(int16_t *sensors);


int16_t sensor_value[NUM_REGISTERS] = {0};
uint8_t mFlag_ = 0;
uint8_t received_slave_address = 0;
uint8_t buffer[BUFFER_SIZE];

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
	adc_init();

	/*
	 * TEST for mux implementation.
	for (int channel = 0; channel < 8; channel++) {
	   mux_select(channel);  // Select a channel (PB3, PB4, PB5)
	   delay_ms(1000);  // Wait 1 second to observe LED behavior
	}
	*/

	uint8_t received_frame[8] = {0};

    while(1)
    {
		    uint16_t adc_value;
	char buf[100];

    adc_value = adc_read(0);
    uint16_t cell_voltage = adc_convert_cell_voltage(adc_value);
    sprintf(buf, "Simulated cell voltage %u mV\r\n", cell_voltage);
    USART2_send_string(buf);

    adc_value = adc_read(1);
    int16_t battery_temp = adc_convert_battery_temp(adc_value);
    sprintf(buf, "Simulated Battery Temperature: %d degrees Celsius\r\n", battery_temp);
    USART2_send_string(buf); 

    adc_value = adc_read(4);
    uint16_t battery_current = adc_convert_battery_current(adc_value);
    sprintf(buf, "Simulated Battery Current: %u mA\r\n", battery_current);
    USART2_send_string(buf); 
    	switch (mFlag_) {
            case 0:
                for (uint8_t k = 0; k < 8;k++)
                {
                    received_frame[k] = '0';
                }
                //memset(received_frame, '0', 8);
                break;

    		case 1:
    			received_frame[0] = SLAVE_ADDR;
    			for (uint8_t i = 1; i < 8; i++)
				{
					received_frame[i] = USART2_read();
				}
				uint16_t CRC_cal_received = CRC16 ((const uint8_t *)received_frame,6);
				uint8_t crc_low = CRC_cal_received & 0xFF;
				uint8_t crc_high = CRC_cal_received >> 8;

				delay_ms(40);
				if((crc_high == received_frame[7]) && (crc_low == received_frame[6]))
				{
					uint16_t reg_addr[MAX_REGISTERS] = {0};
					uint16_t reg_quantity = 0;
					extract_registers_info(received_frame, reg_addr, &reg_quantity);

					if (reg_quantity != 0)
					{
                        int16_t meas[NUM_REGISTERS] = {0};
                        read_sensor_values(meas);  // reading 6 sensor values

                        for (uint8_t i = START_ADDR; i < NUM_REGISTERS; i++)
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

void extract_registers_info(uint8_t* received_frame, uint16_t* reg_addr, uint16_t* reg_quantity) {
    uint16_t start_addr = (received_frame[2] << 8) | received_frame[3];
    *reg_quantity = (received_frame[4] << 8) | received_frame[5];

    if (*reg_quantity > MAX_REGISTERS) {
        // Handle error

    }

    for (uint8_t i = 0; i < *reg_quantity; i++)
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


void response_frame_constructor(int16_t *sensor_value)
{
	//PA5 led ON, transmiting mode activated
	GPIOA->ODR |= 0x20;

	uint8_t response_length = RESPONSE_BYTE_COUNT + 5;
	uint8_t response_frame[RESPONSE_BYTE_COUNT + 5] = {0};

    response_frame[0] = SLAVE_ADDR;
    response_frame[1] = FUNCTION_CODE;
    response_frame[2] = RESPONSE_BYTE_COUNT;

    // Each register has data first high-byte and then low byte
    for(uint8_t i = 0; i < NUM_REGISTERS ; i++)
    {
        // Update high byte for sensor
    	uint8_t data_hi = (sensor_value[i] >> 8) & 0xFF;
        // Update low byte for sensor
    	uint8_t data_lo = sensor_value[i] & 0xFF;

        response_frame[2*i+3] = data_hi;
        response_frame[2*i+4] = data_lo;
    }


	uint16_t CRC_calc_response = 0;

	CRC_calc_response = CRC16((uint8_t*)response_frame, (response_length-2));
    response_frame[response_length - 2] = CRC_calc_response & 0xFF; // Low byte of CRC
    response_frame[response_length - 1] = (CRC_calc_response >> 8) & 0xFF; // High byte of CRC

	for (uint8_t i = 0; i < response_length; ++i) {
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

/* Mock adc values to test sdc converter and modbus*/
void read_sensor_values(int16_t *sensors) {
    uint16_t adc_value;
    char buf[100];

    // Read ADC value for cell voltage 1
    adc_value = adc_read(0);
    uint16_t cell_voltage = adc_convert_cell_voltage(adc_value);
    //sprintf(buf, "Simulated cell voltage: %u mV, corresponding ADC value: %u\r\n", cell_voltage, adc_value);
    //USART2_send_string(buf);

    // Read ADC value for battery temperature
    adc_value = adc_read(1);
    int16_t battery_temp = adc_convert_battery_temp(adc_value);
    //sprintf(buf, "Simulated Battery Temperature: %d degrees Celsius\r\n", battery_temp);
    //USART2_send_string(buf); // Send battery temperature information

    // Read ADC value for battery current
    adc_value = adc_read(4);
    uint16_t battery_current = adc_convert_battery_current(adc_value);
    //sprintf(buf, "Simulated Battery Current: %u mA\r\n", battery_current);
    //USART2_send_string(buf); // Send battery current information

    // Send the sensor values to the sensors array
    sensors[0] = battery_temp;    // Store battery temperature in sensors array
    sensors[1] = adc_value;        // Store ADC value for battery temperature
    sensors[2] = cell_voltage;     // Store cell voltage in sensors array
    sensors[3] = battery_current;   // Store battery current in sensors array

    sensors[4] = 3800;  // Mock cell voltage3: 3.8V (in millivolts: 3800 mV)
    sensors[5] = 3550;  // Mock cell voltage4: 3.55V (in millivolts: 3550 mV)
}

