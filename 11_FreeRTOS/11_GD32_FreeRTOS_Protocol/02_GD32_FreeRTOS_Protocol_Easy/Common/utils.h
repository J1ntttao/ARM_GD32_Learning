#ifndef __UTILS_H__
#define __UTILS_H__

#include "gd32f4xx.h"
#include <stdio.h>

void print_bytes(const char* label, uint8_t bytes[], uint32_t len);
uint8_t isLittleEndian();
uint8_t isFloatLittleEndian();
float bytesToFloat(uint8_t bytes[4]);

#endif