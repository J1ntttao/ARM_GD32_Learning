#ifndef __ADC_CONFIG_H__
#define __ADC_CONFIG_H__

#include "gd32f4xx.h"
#include "systick.h"

#define USE_ADC_0		1
#define USE_ADC_1		0
#define USE_ADC_2		0


#define ADC_CHN_ROUTINE     0
#define ADC_CHN_INSERT      1

///////////////////// ADC 0 ///////////////////
#if USE_ADC_0

#define ADC0_RESOLUTION     ADC_RESOLUTION_12B
#define ADC0_DATAALIGN      ADC_DATAALIGN_RIGHT

#define ADC0_CHN_TYPE       ADC_CHN_INSERT
#define ADC0_SAMPLETIME     ADC_SAMPLETIME_144

#define ADC0_CHNS_LEN       2
#define ADC0_CHNS           {    \
            ADC_CHANNEL_13,      \
            ADC_CHANNEL_2,       \
            /*ADC_CHANNEL_4 ,*/  \
}

#endif

#endif