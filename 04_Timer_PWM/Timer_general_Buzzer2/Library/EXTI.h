#ifndef __EXTI_H__
#define __EXTI_H__

#include "gd32f4xx.h"

#define USE_EXTI0    1
#define USE_EXTI1    1
#define USE_EXTI2    1
#define USE_EXTI3    0
#define USE_EXTI4    0
#define USE_EXTI5    0
#define USE_EXTI6    0
#define USE_EXTI7    0
#define USE_EXTI8    0
#define USE_EXTI9    0
#define USE_EXTI10   0
#define USE_EXTI11   0
#define USE_EXTI12   0
#define USE_EXTI13   0
#define USE_EXTI14   0
#define USE_EXTI15   0

// ------------------------------ EXTI0 ----------------------
#if USE_EXTI0
#define EXTI0_SOFT    1

#if !EXTI0_SOFT
// GPIO
#define EXTI0_RCU     RCU_GPIOC
#define EXTI0_GPIO    GPIOC
#define EXTI0_PUPD    GPIO_PUPD_PULLUP
// EXTI & NVIC
#define EXTI0_SOURCE_PORT   EXTI_SOURCE_GPIOC
#define EXTI0_TRIG_TYPE     EXTI_TRIG_FALLING
#endif

#define EXTI0_IRQ_PRI   1U, 0U

#endif

// ------------------------------ EXTI1 ----------------------
#if USE_EXTI1
#define EXTI1_SOFT    1

#if !EXTI1_SOFT
// GPIO
#define EXTI1_RCU     RCU_GPIOC
#define EXTI1_GPIO    GPIOC
#define EXTI1_PUPD    GPIO_PUPD_PULLUP
// EXTI & NVIC
#define EXTI1_SOURCE_PORT   EXTI_SOURCE_GPIOC
#define EXTI1_TRIG_TYPE     EXTI_TRIG_FALLING
#endif

#define EXTI1_IRQ_PRI   2U, 0U

#endif

// ------------------------------ EXTI2 ----------------------
#if USE_EXTI2
#define EXTI2_SOFT    1

#if !EXTI2_SOFT
// GPIO
#define EXTI2_RCU     RCU_GPIOC
#define EXTI2_GPIO    GPIOC
#define EXTI2_PUPD    GPIO_PUPD_PULLUP
// EXTI & NVIC
#define EXTI2_SOURCE_PORT   EXTI_SOURCE_GPIOC
#define EXTI2_TRIG_TYPE     EXTI_TRIG_FALLING
#endif

#define EXTI2_IRQ_PRI   3U, 0U

#endif

// ------------------------------ EXTI3 ----------------------
#if USE_EXTI3

// GPIO
#define EXTI3_RCU     RCU_GPIOC
#define EXTI3_GPIO    GPIOC
#define EXTI3_PUPD    GPIO_PUPD_PULLUP
// EXTI & NVIC
#define EXTI3_SOURCE_PORT   EXTI_SOURCE_GPIOC
#define EXTI3_TRIG_TYPE     EXTI_TRIG_FALLING

#define EXTI3_IRQ_PRI   2U, 2U

#endif

// ------------------------------ EXTI4 ----------------------
#if USE_EXTI4

// GPIO
#define EXTI4_RCU     RCU_GPIOC
#define EXTI4_GPIO    GPIOC
#define EXTI4_PUPD    GPIO_PUPD_PULLUP
// EXTI & NVIC
#define EXTI4_SOURCE_PORT   EXTI_SOURCE_GPIOC
#define EXTI4_TRIG_TYPE     EXTI_TRIG_FALLING

#define EXTI4_IRQ_PRI   2U, 2U

#endif

// ------------------------------ EXTI5 ----------------------
#if USE_EXTI5

// GPIO
#define EXTI5_RCU     RCU_GPIOC
#define EXTI5_GPIO    GPIOC
#define EXTI5_PUPD    GPIO_PUPD_PULLUP
// EXTI & NVIC
#define EXTI5_SOURCE_PORT   EXTI_SOURCE_GPIOC
#define EXTI5_TRIG_TYPE     EXTI_TRIG_FALLING

#define EXTI5_IRQ_PRI   2U, 2U

#endif

// ------------------------------ EXTI6 ----------------------
#if USE_EXTI6

// GPIO
#define EXTI6_RCU     RCU_GPIOC
#define EXTI6_GPIO    GPIOC
#define EXTI6_PUPD    GPIO_PUPD_PULLUP
// EXTI & NVIC
#define EXTI6_SOURCE_PORT   EXTI_SOURCE_GPIOC
#define EXTI6_TRIG_TYPE     EXTI_TRIG_FALLING

#define EXTI6_IRQ_PRI   2U, 2U

#endif


void EXTI_init(void);

#endif

