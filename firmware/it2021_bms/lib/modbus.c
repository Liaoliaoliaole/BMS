#include "bms_configuration.h"
#include "crc.h"
#include "usart1.h"
#include "modbus.h"

void modbus_extract_registers_info(unsigned char* received_frame, unsigned short int* reg_addr, unsigned short int* reg_quantity) {
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

void modbus_send_response_frame(int *sensor_value)
{
	//PA5 led ON, transmiting mode activated
	//RE/DE control for MAX3485 from PA5(NEED TO CONNECT! NOT D2)
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

	for (int i = 0; i < response_length+2; ++i) {//2 stop bit if no parity according to protocol

		USART1_write(response_frame[i]);
	}



	//LED off, receiving mode activated.
	GPIOA->ODR&=~0x20;

}

void modbus_wrong_slave_address(void)
{
	   USART1->CR1 &= ~0x00000004;		//RE bit. p739-740. Disable receiver
	   delay_Ms(10); 					//time=1/9600 x 10 bits x 7 byte = 7,29 ms
	   USART1->CR1 |= 0x00000004;		//RE bit. p739-740. Enable receiver
	   USART1->CR1 |= 0x0020;			//enable RX interrupt
	   mFlag=0;
}

