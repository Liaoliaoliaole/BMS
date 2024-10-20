#pragma once
#include <stdint.h>

// Function declarations
void mux_init();
void mux_select(const uint32_t channel, uint32_t mux_select_pin_mask);
uint16_t read_mux_channel(uint32_t channel, uint32_t mux_select_pin_mask, uint32_t adc_channel);
void mux_reset();

#ifdef TEST
void mux_test();
#endif
