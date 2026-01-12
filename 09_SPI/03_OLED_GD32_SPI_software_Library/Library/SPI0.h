#ifndef __SPI0_H__
#define __SPI0_H__

#include "gd32f4xx.h"


void SPI0_init();

void SPI0_write(uint8_t dat); // send

uint8_t SPI0_read(); // recv

#endif