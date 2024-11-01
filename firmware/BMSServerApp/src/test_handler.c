#include "test_handler.h"
#include "usart2.h"
#include <string.h>

void handle_serial_command() {
    char buffer[20];
    USART2_receive_string(buffer, sizeof(buffer));

    if (strcmp(buffer, "STATUS") == 0) {
        send_device_status();
    } else if (strcmp(buffer, "MAKESURE") == 0) {
        are_you_sure_it_is_working();
    } else {
        USART2_send_string(buffer); 
        USART2_send_string("end\r\n"); 
    }

}

void send_device_status() {
    USART2_send_string("Device is working\r\n");
}

void are_you_sure_it_is_working() {
    USART2_send_string("Device is indeed working\r\n");
}
