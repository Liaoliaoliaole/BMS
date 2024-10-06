#pragma once

void adc_init(void);
uint16_t adc_read(const uint8_t adc_channel);

#if TEST
uint16_t adc_read_mock(const uint8_t adc_channel);
#endif
