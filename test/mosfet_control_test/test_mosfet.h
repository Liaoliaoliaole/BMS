#pragma once

#include <stdbool.h>

// Mock GPIO and USART functions for testing
void mock_set_charge_mosfet(bool state);
void mock_set_discharge_mosfet(bool state);
void mock_USART2_send_string(const char* str);

// Test case functions
void run_mosfet_control_tests(void);
void test_normal_operation(void);
void test_voltage_thresholds(void);
void test_current_thresholds(void);
void test_temperature_protection(void);
void test_state_transitions(void);
