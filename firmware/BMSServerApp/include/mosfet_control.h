#pragma once

#include <stdint.h>

// MOSFET Control States
typedef enum {
    MOSFET_OFF = 0,
    MOSFET_ON
} mosfet_state_t;

typedef enum {
    STATE_LOW_VOLTAGE,
    STATE_NORMAL_VOLTAGE,
    STATE_HIGH_VOLTAGE
} voltage_state_t;

void mosfet_init(void) ;
void mosfet_control_logic(float battery_voltage, float current);
void set_charge_mosfet(mosfet_state_t state);
void set_discharge_mosfet(mosfet_state_t state);

#if TEST
void test_mosfet_control_logic(void) ;
#endif
