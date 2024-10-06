#pragma once
#include <stdint.h>

// Function declarations
void mux_init();
void mux_select(const uint32_t channel, uint32_t mux_select_pin_mask);

#ifdef TEST
void mux_reset();
#endif
