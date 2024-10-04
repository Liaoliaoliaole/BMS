#pragma once
#include <stdint.h>

void mux_init();
void mux_select(const uint32_t channel);
void mux_reset(void);
