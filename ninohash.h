#pragma once


#include <stdint.h>
#include <string.h>

void nino256sum(const uint8_t *const buffer, uint32_t length, uint16_t rounds, uint8_t * hash);

