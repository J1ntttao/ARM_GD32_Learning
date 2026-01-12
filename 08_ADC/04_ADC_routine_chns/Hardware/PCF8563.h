#ifndef __PCF8563_H__
#define __PCF8563_H__

// claude code, trae

#include "gd32f4xx.h"
#include "I2C0.h"

// 一定要注意这里用的是 7bit设备地址, 还是8bit设备写地址
#define PCF8563_ADDR       (0x51) 
#define PCF8563_REG        (0x02)

// 闹铃的开关     ENABLE  DISABLE
#define USE_ALARM   DISABLE

// 定时器的开关   ENABLE  DISABLE
#define USE_TIMER   DISABLE

#ifndef u8
#define u8 uint8_t
#endif

#ifndef u16
#define u16 uint16_t
#endif

#ifndef u32
#define u32 uint32_t
#endif

typedef struct Clock{
    u16 year; // 2025
    u8 month;
    u8 day;
    u8 week;
    u8 hour;
    u8 minute;
    u8 second; 
} Clock_t;


// alarm
typedef struct Alarm
{
    int8_t minute;    // [-128, 127]
    int8_t hour;      // [-128, 127]
    int8_t day;        
    int8_t week;
} Alarm_t;

typedef enum {
    HZ4096 = 0,
    HZ64   = 1,
    HZ1    = 2,
    HZ1_60 = 3,    
} TimerFreq;

#define NUMBER  7

#define I2C_WRITE(addr, reg, ptr, num) I2C0_write(addr, reg, ptr, num)
#define I2C_READ(addr, reg, ptr, num)  I2C0_read(addr, reg, ptr, num)

// 初始化函数
void PCF8563_init();

// 输出函数
void PCF8563_set_clock(Clock_t clock);

// 输入函数
void PCF8563_get_clock(Clock_t* p_clock);

// 设置闹铃时间
void PCF8563_set_alarm(Alarm_t alarm); 

// 启用闹铃
void PCF8563_enable_alarm(u8 enable);

// 清理闹铃标记
void PCF8563_clear_alarm();


// 设置Timer定时器
void PCF8563_set_timer(TimerFreq freq, u8 countdown); 

// 启用Timer定时器
void PCF8563_enable_timer(u8 enable);

// 清理Timer定时器标记
void PCF8563_clear_timer();

void PCF8563_ext_int_call();

// extern函数, 是要求使用者(程序员) 实现的函数

// 此函数在Alarm外部中断触发时被调用
extern void PCF8563_on_alarm();

// 此函数在Timer外部中断触发时被调用
extern void PCF8563_on_timer();

#endif