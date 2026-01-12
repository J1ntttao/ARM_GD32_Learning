#ifndef __EXTI_H__
#define __EXTI_H__

#include "gd32f4xx.h"

// 用哪个就把哪个置为 1

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
// 硬实现需要检测引脚电平需要去 EXTI.c对应的位置改！！！！！
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
#define EXTI3_SOFT    1

    #if !EXTI3_SOFT
    // GPIO
    #define EXTI3_RCU     RCU_GPIOC
    #define EXTI3_GPIO    GPIOC
    #define EXTI3_PUPD    GPIO_PUPD_PULLUP
    // EXTI & NVIC
    #define EXTI3_SOURCE_PORT   EXTI_SOURCE_GPIOC
    #define EXTI3_TRIG_TYPE     EXTI_TRIG_FALLING
    #endif

#define EXTI3_IRQ_PRI   2U, 2U

#endif

// ------------------------------ EXTI4 ----------------------
#if USE_EXTI4
#define EXTI4_SOFT    1

    #if !EXTI4_SOFT
    // GPIO
    #define EXTI4_RCU     RCU_GPIOC
    #define EXTI4_GPIO    GPIOC
    #define EXTI4_PUPD    GPIO_PUPD_PULLUP
    // EXTI & NVIC
    #define EXTI4_SOURCE_PORT   EXTI_SOURCE_GPIOC
    #define EXTI4_TRIG_TYPE     EXTI_TRIG_FALLING
    #endif  

#define EXTI4_IRQ_PRI   2U, 2U

#endif

// ------------------------------ EXTI5 ----------------------
#if USE_EXTI5
#define EXTI5_SOFT    1

    #if !EXTI5_SOFT
    // GPIO
    #define EXTI5_RCU     RCU_GPIOC
    #define EXTI5_GPIO    GPIOC
    #define EXTI5_PUPD    GPIO_PUPD_PULLUP
    // EXTI & NVIC
    #define EXTI5_SOURCE_PORT   EXTI_SOURCE_GPIOC
    #define EXTI5_TRIG_TYPE     EXTI_TRIG_FALLING
    #endif  

#define EXTI5_IRQ_PRI   2U, 2U

#endif

// ------------------------------ EXTI6 ----------------------
#if USE_EXTI6
#define EXTI6_SOFT    1

    #if !EXTI6_SOFT
    // GPIO
    #define EXTI6_RCU     RCU_GPIOC
    #define EXTI6_GPIO    GPIOC
    #define EXTI6_PUPD    GPIO_PUPD_PULLUP
    // EXTI & NVIC
    #define EXTI6_SOURCE_PORT   EXTI_SOURCE_GPIOC
    #define EXTI6_TRIG_TYPE     EXTI_TRIG_FALLING
    #endif  

#define EXTI6_IRQ_PRI   2U, 2U

#endif

// ------------------------------ EXTI7 ----------------------
#if USE_EXTI7
#define EXTI7_SOFT    1

    #if !EXTI7_SOFT
    // GPIO
    #define EXTI7_RCU     RCU_GPIOC
    #define EXTI7_GPIO    GPIOC
    #define EXTI7_PUPD    GPIO_PUPD_PULLUP
    // EXTI & NVIC
    #define EXTI7_SOURCE_PORT   EXTI_SOURCE_GPIOC
    #define EXTI7_TRIG_TYPE     EXTI_TRIG_FALLING
    #endif  

#define EXTI7_IRQ_PRI   2U, 2U

#endif

// ------------------------------ EXTI8 ----------------------
#if USE_EXTI8
#define EXTI8_SOFT    1

    #if !EXTI8_SOFT
    // GPIO
    #define EXTI8_RCU     RCU_GPIOC
    #define EXTI8_GPIO    GPIOC
    #define EXTI8_PUPD    GPIO_PUPD_PULLUP
    // EXTI & NVIC
    #define EXTI8_SOURCE_PORT   EXTI_SOURCE_GPIOC
    #define EXTI8_TRIG_TYPE     EXTI_TRIG_FALLING
    #endif  

#define EXTI8_IRQ_PRI   2U, 2U

#endif

// ------------------------------ EXTI9 ----------------------
#if USE_EXTI9
#define EXTI9_SOFT    1

    #if !EXTI9_SOFT
    // GPIO
    #define EXTI9_RCU     RCU_GPIOC
    #define EXTI9_GPIO    GPIOC
    #define EXTI9_PUPD    GPIO_PUPD_PULLUP
    // EXTI & NVIC
    #define EXTI9_SOURCE_PORT   EXTI_SOURCE_GPIOC
    #define EXTI9_TRIG_TYPE     EXTI_TRIG_FALLING
    #endif  

#define EXTI9_IRQ_PRI   2U, 2U

#endif

// ------------------------------ EXTI10 ----------------------
#if USE_EXTI10
#define EXTI10_SOFT    1

    #if !EXTI10_SOFT
    // GPIO
    #define EXTI10_RCU     RCU_GPIOC
    #define EXTI10_GPIO    GPIOC
    #define EXTI10_PUPD    GPIO_PUPD_PULLUP
    // EXTI & NVIC
    #define EXTI10_SOURCE_PORT   EXTI_SOURCE_GPIOC
    #define EXTI10_TRIG_TYPE     EXTI_TRIG_FALLING
    #endif  

#define EXTI10_IRQ_PRI   2U, 2U

#endif  

// ------------------------------ EXTI11 ----------------------
#if USE_EXTI11
#define EXTI11_SOFT    1

    #if !EXTI11_SOFT
    // GPIO
    #define EXTI11_RCU     RCU_GPIOC
    #define EXTI11_GPIO    GPIOC
    #define EXTI11_PUPD    GPIO_PUPD_PULLUP
    // EXTI & NVIC
    #define EXTI11_SOURCE_PORT   EXTI_SOURCE_GPIOC
    #define EXTI11_TRIG_TYPE     EXTI_TRIG_FALLING
    #endif  

#define EXTI11_IRQ_PRI   2U, 2U

#endif  

// ------------------------------ EXTI12 ----------------------
#if USE_EXTI12
#define EXTI12_SOFT    1

    #if !EXTI12_SOFT
    // GPIO
    #define EXTI12_RCU     RCU_GPIOC
    #define EXTI12_GPIO    GPIOC
    #define EXTI12_PUPD    GPIO_PUPD_PULLUP
    // EXTI & NVIC
    #define EXTI12_SOURCE_PORT   EXTI_SOURCE_GPIOC
    #define EXTI12_TRIG_TYPE     EXTI_TRIG_FALLING
    #endif  

#define EXTI12_IRQ_PRI   2U, 2U

#endif      

// ------------------------------ EXTI13 ----------------------
#if USE_EXTI13
#define EXTI13_SOFT    1

    #if !EXTI13_SOFT
    // GPIO
    #define EXTI13_RCU     RCU_GPIOC
    #define EXTI13_GPIO    GPIOC
    #define EXTI13_PUPD    GPIO_PUPD_PULLUP
    // EXTI & NVIC
    #define EXTI13_SOURCE_PORT   EXTI_SOURCE_GPIOC
    #define EXTI13_TRIG_TYPE     EXTI_TRIG_FALLING
    #endif  

#define EXTI13_IRQ_PRI   2U, 2U

#endif  

// ------------------------------ EXTI14 ----------------------
#if USE_EXTI14
#define EXTI14_SOFT    1

    #if !EXTI14_SOFT
    // GPIO
    #define EXTI14_RCU     RCU_GPIOC
    #define EXTI14_GPIO    GPIOC
    #define EXTI14_PUPD    GPIO_PUPD_PULLUP
    // EXTI & NVIC
    #define EXTI14_SOURCE_PORT   EXTI_SOURCE_GPIOC
    #define EXTI14_TRIG_TYPE     EXTI_TRIG_FALLING
    #endif  

#define EXTI14_IRQ_PRI   2U, 2U

#endif  

// ------------------------------ EXTI15 ----------------------
#if USE_EXTI15
#define EXTI15_SOFT    1

    #if !EXTI15_SOFT
    // GPIO
    #define EXTI15_RCU     RCU_GPIOC
    #define EXTI15_GPIO    GPIOC
    #define EXTI15_PUPD    GPIO_PUPD_PULLUP
    // EXTI & NVIC
    #define EXTI15_SOURCE_PORT   EXTI_SOURCE_GPIOC
    #define EXTI15_TRIG_TYPE     EXTI_TRIG_FALLING
    #endif  

#define EXTI15_IRQ_PRI   2U, 2U

#endif  




void EXTI_init();

#endif