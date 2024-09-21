#pragma once
#include <stdint.h> 
#include "stm32l1xx.h"
#include "bms_configuration.h"

void mux_init();
void mux_select(const uint32_t channel);
void mux_reset(void);
