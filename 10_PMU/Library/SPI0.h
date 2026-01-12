#ifndef __SPI0_H__
#define __SPI0_H__

#include "gd32f4xx.h"

#define SPI0_SOFT           0     // 优先考虑软实现

#define SPI0_SCL_RCU        RCU_GPIOA
#define SPI0_SCL_GPIO       GPIOA, GPIO_PIN_5

#define SPI0_MOSI_RCU       RCU_GPIOA
#define SPI0_MOSI_GPIO      GPIOA, GPIO_PIN_7

#define SPI0_MISO_RCU       RCU_GPIOA
#define SPI0_MISO_GPIO      GPIOA, GPIO_PIN_6

// (n=2,4,8,16,32,64,128,256)
#define SPI0_PRESCALE       SPI_PSC_32 
#define SPI0_CLOCK_POL_PHA  SPI_CK_PL_HIGH_PH_2EDGE


void SPI0_init();

void SPI0_write(uint8_t dat); // send

uint8_t SPI0_read(); // recv

uint8_t SPI0_write_read(uint8_t dat);

#endif