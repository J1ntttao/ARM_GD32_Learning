#ifndef __SPI0_H__
#define __SPI0_H__

#include "gd32f4xx.h"

#define SPI0_SCL_RCU      RCU_GPIOA
#define SPI0_SCL_GPIO     GPIOA, GPIO_PIN_5

#define SPI0_MOSI_RCU     RCU_GPIOA
#define SPI0_MOSI_GPIO    GPIOA, GPIO_PIN_7

#define SPI0_MISO_RCU     RCU_GPIOA
#define SPI0_MISO_GPIO    GPIOA, GPIO_PIN_6


void SPI0_init();

void SPI0_write(uint8_t dat); // send

uint8_t SPI0_read(); // recv

#endif