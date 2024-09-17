#include<stdio.h>

extern int sensor_value[NUM_REGISTERS] = {0};


void modbus_extract_registers_info(unsigned char* received_frame, unsigned short int* reg_addr, unsigned short int* reg_quantity);
void modbus_send_response_frame(int *sensor_value);
void modbus_wrong_slave_address(void);

