#pragma once
#include <stddef.h>
#include <stdint.h>

#include "stm32l1xx.h"

uint16_t CRC16(const uint8_t *nData, const uint16_t wLength);
