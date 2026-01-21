#ifndef __ADC0_H__
#define __ADC0_H__

#include "ADC_config.h"

void ADC0_init();

uint16_t ADC0_get(uint8_t chn);

#endif