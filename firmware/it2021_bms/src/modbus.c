#include "bms_configuration.h"
#include "crc.h"
#include "usart.h"
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

void modbus_routine(register_data *reg_data) {
    unsigned char received_frame[8] = {0};

    switch (mFlag) {
        case 0:
            // Clear received frame
            for (int k = 0; k < 8; k++) {
                received_frame[k] = '0';
            }
            break;

        case 1:
            // Fill received frame
            received_frame[0] = SLAVE_ADDR;
            for (int i = 1; i < 8; i++) {
                received_frame[i] = USART1_read();
            }

            // Calculate and check CRC
            unsigned short int CRC_cal_received = CRC16((char *)received_frame, 6);
            unsigned char crc_low = CRC_cal_received & 0xFF;
            unsigned char crc_high = CRC_cal_received >> 8;

            delay_Ms(40);
            if ((crc_high == received_frame[7]) && (crc_low == received_frame[6])) {
                // Extract register info
                unsigned short int reg_addr[MAX_REGISTERS] = {0};
                unsigned short int reg_quantity = 0;
                extract_registers_info(received_frame, reg_addr, &reg_quantity);

                if (reg_quantity != 0) {
                    // Populate register_data struct with sensor readings
                    int meas[NUM_REGISTERS] = {0};
                    //TODO:read_dht22_humidity_and_temperature(&meas[1], &meas[0]);

                    // Store readings in the register_data struct
                    reg_data->LM35_temp = meas[0];    // LM35 temperature
                    reg_data->shunt = meas[1];        // Shunt voltage
                    for (int i = 0; i < 4; i++) {
                        reg_data->cell_vol[i] = meas[2 + i];  // Cell voltages
                    }

                    // Construct response frame with sensor values
                    response_frame_constructor((unsigned short int*)reg_data);
                } else {
                    // Handle error
                }
            }
            mFlag = 0;
            USART1->CR1 |= 0x0020;  // Enable RX interrupt
            break;

        case 2:
            wrong_slave_address();
            break;
    }
}
