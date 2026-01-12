#ifndef __TIMER_CONFIG_H__
#define __TIMER_CONFIG_H__

#include "gd32f4xx.h"

#define USE_TIMER_0     1 
#define USE_TIMER_1     1   
#define USE_TIMER_2     0   
#define USE_TIMER_3     0  
#define USE_TIMER_4     0   
#define USE_TIMER_5     0   
#define USE_TIMER_6     0   
#define USE_TIMER_7     0   
#define USE_TIMER_8     0   
#define USE_TIMER_9     0   
#define USE_TIMER_10    0   
#define USE_TIMER_11    0   
#define USE_TIMER_12    0   
#define USE_TIMER_13    0   

// ===================== TIMER0 ===================== 高级 
#if USE_TIMER_0

#define TM0_PRESCALER     100U 
#define TM0_FREQ          50U
#define TM0_PERIOD        (SystemCoreClock / TM0_PRESCALER / TM0_FREQ)

#define TM0_CH0           RCU_GPIOE, GPIOE, GPIO_PIN_9  // PA8 PE9
#define TM0_CH0_ON        RCU_GPIOE, GPIOE, GPIO_PIN_8  // PA7 PB13 PE8

#define TM0_CH1           RCU_GPIOA, GPIOA, GPIO_PIN_9  // PA9 PE11
#define TM0_CH1_ON        RCU_GPIOB, GPIOB, GPIO_PIN_0  // PB0 PB14 PE1 PE10

#define TM0_CH2           RCU_GPIOE, GPIOE, GPIO_PIN_13 // PA10 PE13
#define TM0_CH2_ON        RCU_GPIOE, GPIOE, GPIO_PIN_12 // PB1 PB15 PE12

#define TM0_CH3           RCU_GPIOA, GPIOA, GPIO_PIN_11
#define TM0_CH3_ON        RCU_GPIOE, GPIOE, GPIO_PIN_14

#endif

// ===================== TIMER7 ===================== 高级 
#if USE_TIMER_7

#define TM7_PRESCALER     10U 
#define TM7_FREQ          1000U
#define TM7_PERIOD        (SystemCoreClock / TM7_PRESCALER / TM7_FREQ)

#define TM7_CH0           RCU_GPIOC, GPIOC, GPIO_PIN_6  // PC6 PI15
#define TM7_CH0_ON        RCU_GPIOA, GPIOA, GPIO_PIN_5  // PA5 PA7 PH13

#define TM7_CH1           RCU_GPIOC, GPIOC, GPIO_PIN_7  // PC7 PI6
#define TM7_CH1_ON        RCU_GPIOB, GPIOB, GPIO_PIN_0  // PB0 PB14 PH14

#define TM7_CH2           RCU_GPIOC, GPIOC, GPIO_PIN_8  // PC8 PI7
#define TM7_CH2_ON        RCU_GPIOB, GPIOB, GPIO_PIN_1  // PB1 PB15 PH15

#define TM7_CH3           RCU_GPIOC, GPIOC, GPIO_PIN_9
#define TM7_CH3_ON        RCU_GPIOI, GPIOI, GPIO_PIN_2

#endif

// ===================== TIMER1 ===================== 通用4通道
#if USE_TIMER_1

#define TM1_PRESCALER     10U 
#define TM1_FREQ          1000U
#define TM1_PERIOD        (SystemCoreClock / TM1_PRESCALER / TM1_FREQ)

#define TM1_CH0           RCU_GPIOA, GPIOA, GPIO_PIN_0  // PA0 PA5 PA15 PB8
#define TM1_CH1           RCU_GPIOB, GPIOB, GPIO_PIN_3  // PA1 PB3 PB9
#define TM1_CH2           RCU_GPIOA, GPIOA, GPIO_PIN_2  // PA2 PB10
#define TM1_CH3           RCU_GPIOB, GPIOB, GPIO_PIN_11 // PA3 PB2 PB11

#endif

// ===================== TIMER2 ===================== 通用4通道
#if USE_TIMER_2

#define TM2_PRESCALER     10U 
#define TM2_FREQ          1000U
#define TM2_PERIOD        (SystemCoreClock / TM2_PRESCALER / TM2_FREQ)

#define TM2_CH0           RCU_GPIOA, GPIOA, GPIO_PIN_6  // PA6 PB4 PC6
#define TM2_CH1           RCU_GPIOA, GPIOA, GPIO_PIN_7  // PA7 PB5 PC7
#define TM2_CH2           RCU_GPIOB, GPIOB, GPIO_PIN_0  // PB0 PC8
#define TM2_CH3           RCU_GPIOB, GPIOB, GPIO_PIN_1  // PB1 PC9

#endif

// ===================== TIMER3 ===================== 通用4通道
#if USE_TIMER_3

#define TM3_PRESCALER     10U 
#define TM3_FREQ          1000U
#define TM3_PERIOD        (SystemCoreClock / TM3_PRESCALER / TM3_FREQ)

#define TM3_CH0           RCU_GPIOD, GPIOD, GPIO_PIN_12 // PB6 PD12
#define TM3_CH1           RCU_GPIOD, GPIOD, GPIO_PIN_13 // PB7 PD13
#define TM3_CH2           RCU_GPIOD, GPIOD, GPIO_PIN_14 // PB8 PD14
#define TM3_CH3           RCU_GPIOD, GPIOD, GPIO_PIN_15 // PB9 PD15

#endif
// ===================== TIMER4 ===================== 通用4通道
#if USE_TIMER_4

#define TM4_PRESCALER     10U 
#define TM4_FREQ          1000U
#define TM4_PERIOD        (SystemCoreClock / TM4_PRESCALER / TM4_FREQ)

#define TM4_CH0           RCU_GPIOA, GPIOA, GPIO_PIN_0  // PA0 PH10
#define TM4_CH1           RCU_GPIOA, GPIOA, GPIO_PIN_1  // PA1 PH11
#define TM4_CH2           RCU_GPIOA, GPIOA, GPIO_PIN_2  // PA2 PH12
#define TM4_CH3           RCU_GPIOA, GPIOA, GPIO_PIN_3  // PA3 PI0

#endif

// 定时器5、6 是基本定时器 0 通道
// ===================== TIMER5 ===================== 基本0通道
#if USE_TIMER_5

#define TM5_PRESCALER     10U 
#define TM5_FREQ          1000U
#define TM5_PERIOD        (SystemCoreClock / TM5_PRESCALER / TM5_FREQ)

#endif

// ===================== TIMER6 ===================== 基本0通道
#if USE_TIMER_6

#define TM6_PRESCALER     10U 
#define TM6_FREQ          1000U
#define TM6_PERIOD        (SystemCoreClock / TM6_PRESCALER / TM6_FREQ)

#endif

// ===================== TIMER8 ===================== 通用2通道
#if USE_TIMER_8

#define TM8_PRESCALER     10U 
#define TM8_FREQ          1000U
#define TM8_PERIOD        (SystemCoreClock / TM8_PRESCALER / TM8_FREQ)

#define TM8_CH0           RCU_GPIOA, GPIOA, GPIO_PIN_2  // PA2 PE5
#define TM8_CH1           RCU_GPIOA, GPIOA, GPIO_PIN_3  // PA3 PE6

#endif

// ===================== TIMER11 ===================== 通用2通道
#if USE_TIMER_11

#define TM11_PRESCALER     10U 
#define TM11_FREQ          1000U
#define TM11_PERIOD        (SystemCoreClock / TM11_PRESCALER / TM11_FREQ)

#define TM11_CH0           RCU_GPIOA, GPIOA, GPIO_PIN_8  // PB14 PH6
#define TM11_CH1           RCU_GPIOA, GPIOA, GPIO_PIN_9  // PB15 PH7

#endif

// ===================== TIMER9 ===================== 通用1通道
#if USE_TIMER_9

#define TM9_PRESCALER     10U 
#define TM9_FREQ          1000U
#define TM9_PERIOD        (SystemCoreClock / TM9_PRESCALER / TM9_FREQ)

#define TM9_CH0           RCU_GPIOB, GPIOB, GPIO_PIN_8  // PB8 PF6

#endif 

// ===================== TIMER10 ===================== 通用1通道
#if USE_TIMER_10

#define TM10_PRESCALER     10U 
#define TM10_FREQ          1000U
#define TM10_PERIOD        (SystemCoreClock / TM10_PRESCALER / TM10_FREQ)

#define TM10_CH0           RCU_GPIOB, GPIOB, GPIO_PIN_9  // PB9 PF7

#endif

// ===================== TIMER12 ===================== 通用1通道
#if USE_TIMER_12

#define TM12_PRESCALER     10U 
#define TM12_FREQ          1000U
#define TM12_PERIOD        (SystemCoreClock / TM12_PRESCALER / TM12_FREQ)

#define TM12_CH0           RCU_GPIOA, GPIOA, GPIO_PIN_6 // PA6 PF8

#endif

// ===================== TIMER13 ===================== 通用1通道
#if USE_TIMER_13

#define TM13_PRESCALER     10U 
#define TM13_FREQ          1000U
#define TM13_PERIOD        (SystemCoreClock / TM13_PRESCALER / TM13_FREQ)

#define TM13_CH0           RCU_GPIOA, GPIOA, GPIO_PIN_7 // PA7 PF9

#endif

#endif