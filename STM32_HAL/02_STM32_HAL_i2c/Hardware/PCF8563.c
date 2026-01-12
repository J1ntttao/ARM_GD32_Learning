#include "PCF8563.h"
#include <stdio.h>


// 初始化函数
void PCF8563_init() {

//    // GPIO外设引脚初始化为开漏OD模式
//    GPIO_config();
//    // I2C硬件片上外设初始化
//    I2C_config();
}

// 输出函数
void PCF8563_set_clock(Clock_t c) {
    u8 Cent = 0;
    u8 p[NUMBER] = {0}; // 用于存储时间的数组
    // 准备: 秒,分,时,天,周, 世纪&月, 年

    // 将数字转成BCD格式数据(Binary-Coded Decimal)

    // 秒: VL 1 1 1 - 0 0 0 0  十进制转成BCD // 0x56  (十位 << 4) | 个位
    p[0] = ((c.second / 10) << 4) | (c.second % 10);

    // 分:  x 1 1 1 - 0 0 0 0  十进制转成BCD // 0x46
    p[1] = ((c.minute / 10) << 4) | (c.minute % 10);

    // 时:  x x 1 1 - 0 0 0 0  十进制转成BCD // 0x23
    p[2] = ((c.hour / 10) << 4) | (c.hour % 10);

    // 天:  x x 1 1 - 0 0 0 0  十进制转成BCD // 0x31
    p[3] = ((c.day / 10) << 4) | (c.day % 10);

    // 周:  x x x x - x 0 0 0  十进制转成BCD // 0x6
    p[4] = c.week;

    // 世纪
    Cent = (c.year >= 2100) ? 1 : 0;
    // 月:  C x x 1 - 0 0 0 0  十进制转成BCD // 12   (世纪 << 7 ) | (十位 << 4) | 个位
    p[5] = (Cent << 7 ) | ((c.month / 10) << 4) | (c.month % 10);

    // 年:  1 1 1 1 - 0 0 0 0 十进制转成BCD  // 2025 -> 0x25
    p[6] = ((c.year % 100 / 10) << 4) | (c.year % 10);

    // 将时间信息写进PCF8563的0x02开始的7个寄存器
    int8_t rst = I2C_WRITE(PCF8563_ADDR, PCF8563_REG, p, NUMBER);
    
//    printf("write rst->%d\n", rst);
}


// 输入函数
void PCF8563_get_clock(Clock_t* p_clock) {    
    u8 C;
    u8 p[NUMBER] = {0}; // 用于存储时间的数组

    // 从PCF8563的0x02开始, 读取7个寄存器数据
    I2C_READ(PCF8563_ADDR, PCF8563_REG, p, NUMBER);

    // 秒: VL 1 1 1 - 0 0 0 0  BCD转成十进制 // 0x56
    (*p_clock).second = ((p[0] >> 4) & 0x07) * 10 + (p[0] & 0x0F); // 十位 * 10 + 个位

    // 分:  x 1 1 1 - 0 0 0 0  BCD转成十进制 // 0x46
    p_clock->minute = ((p[1] >> 4) & 0x07) * 10 + (p[1] & 0x0F); // 十位 * 10 + 个位

    // 时:  x x 1 1 - 0 0 0 0  BCD转成十进制 // 0x23
    p_clock->hour   = ((p[2] >> 4) & 0x03) * 10 + (p[2] & 0x0F); // 十位 * 10 + 个位

    // 天:  x x 1 1 - 0 0 0 0  BCD转成十进制 // 0x31
    p_clock->day    = ((p[3] >> 4) & 0x03) * 10 + (p[3] & 0x0F); // 十位 * 10 + 个位

    // 周:  x x x x - x 0 0 0  BCD转成十进制 // 0x6
    p_clock->week   = p[4] & 0x07;

    // 月:  C x x 1 - 0 0 0 0  BCD转成十进制 // 0x31
    p_clock->month  = ((p[5] >> 4) & 0x01) * 10 + (p[5] & 0x0F); // 十位 * 10 + 个位
    C      = p[5] >> 7; //  0->20xx年, 1->21xx年

    // 年:  1 1 1 1 - 0 0 0 0 BCD转成十进制
    p_clock->year   = ((p[6] >> 4) & 0x0F) * 10 + (p[6] & 0x0F); // 十位 * 10 + 个位    2101
    p_clock->year   += (C == 0) ? 2000 : 2100;
}

// 设置闹铃时间 0x09开始的4个字节
void PCF8563_set_alarm(Alarm_t alarm){
    u8 a[4] = {0};
    // a. 设置闹铃时间: 09h分钟,0Ah小时,0Bh天,0Ch周 (最高位配0: 启用)
    // 分 M 1 1 1 - 0 0 0 0  Enable -> 0x00, Disable -> 1 << 7 (0x80)
    if(alarm.minute < 0){ // 禁用
        a[0] = 0x80;
    } else {    // 启用
        a[0] = ((alarm.minute / 10) << 4) + (alarm.minute % 10) + 0x00;
    }

    // 时 H x 1 1 - 0 0 0 0  Enable -> 0x00, Disable -> 1 << 7 (0x80) 23
    if(alarm.hour < 0){ // 禁用
        a[1] = 0x80;
    } else {    // 启用
        a[1] = ((alarm.hour / 10) << 4) + (alarm.hour % 10) + 0x00;
    }

    // 天 D x 1 1 - 0 0 0 0  Enable -> 0x00, Disable -> 1 << 7 (0x80) 31
    if(alarm.day < 0){ // 禁用
        a[2] = 0x80;
    } else {    // 启用
        a[2] = ((alarm.day / 10) << 4) + (alarm.day % 10) + 0x00;
    }

    // 周 W x x x - x 0 0 0  Enable -> 0x00, Disable -> 1 << 7 (0x80)
    if(alarm.week < 0){ // 禁用
        a[3] = 0x80;
    } else {    // 启用
        a[3] = alarm.week + 0x00;
    }

    // 将闹铃时间信息写进PCF8563的0x09开始的4个寄存器
    I2C_WRITE(PCF8563_ADDR, 0x09, a, 4);
}

// 启用闹铃 0x01 cs2
void PCF8563_enable_alarm(u8 enable){
    u8 cs2;
    // 设置01h寄存器CS2, AF=0, AIE=1 启用闹铃
    I2C_READ(PCF8563_ADDR, 0x01, &cs2, 1);
    // 清除Alarm标记, AF设置为0, 下次闹钟到点了才能继续触发INT
    cs2 &= ~(1 << 3); // 1 << 3 == 0x08
    if(enable){
        // 开启Alarm中断, AIE设置为1
        cs2 |= (1 << 1);  // 1 << 1 == 0x02        
    }else {        
        // 关闭Alarm中断, AIE设置为0
        cs2 &=~(1 << 1);  // 1 << 1 == 0x02   
    }
    // 将配置信息写进PCF8563的0x01开始的1个寄存器
    I2C_WRITE(PCF8563_ADDR, 0x01, &cs2, 1);
}

// 清理闹铃标记 0x01 cs2
void PCF8563_clear_alarm(){
    u8 cs2;
    // 先将cs2内容读出来
    I2C_READ(PCF8563_ADDR, 0x01, &cs2, 1);
    // 清除Alarm标记, AF设置为0, 下次闹钟到点了才能继续触发INT
    cs2 &= ~(1 << 3); // 1 << 3 == 0x08
    // 将配置信息写进PCF8563的0x01开始的1个寄存器
    I2C_WRITE(PCF8563_ADDR, 0x01, &cs2, 1);
}


// 设置Timer定时器
void PCF8563_set_timer(TimerFreq freq, u8 countdown){ // 255
    u8 p;
    // 设置Timer运行频率 & 启用Timer
    p = (1 << 7) + freq; // 4096Hz, 64Hz, 1Hz, 1/60Hz
    I2C_WRITE(PCF8563_ADDR, 0x0E, &p, 1);
    // 设置Timer计数值
    p = countdown;
    I2C_WRITE(PCF8563_ADDR, 0x0F, &p, 1);
}

// 启用Timer定时器
void PCF8563_enable_timer(u8 enable){
    u8 cs2;
        // 设置01h寄存器CS2, TF=0, TIE=1 启用Timer
    I2C_READ(PCF8563_ADDR, 0x01, &cs2, 1);
    // 清除Timer标记, TF设置为0, 下次定时器到点了才能继续触发INT
    cs2 &= ~(1 << 2); // 1 << 2 == 0x04
    if(enable){
        // 开启Timer中断, TIE设置为1
        cs2 |= (1 << 0);  // 1 << 0 == 0x01        
    }else {
        // 关闭Timer中断, TIE设置为0
        cs2 &=~(1 << 0);  // 1 << 0 == 0x01        
    }
    // 将配置信息写进PCF8563的0x01开始的1个寄存器
    I2C_WRITE(PCF8563_ADDR, 0x01, &cs2, 1);
}

// 清理Timer定时器标记
void PCF8563_clear_timer(){
     u8 cs2;
    // 先将cs2内容读出来
    I2C_READ(PCF8563_ADDR, 0x01, &cs2, 1);
    // 清除Timer标记, TF设置为0, 下次Timer到点了才能继续触发INT
    cs2 &= ~(1 << 2); // 1 << 2 == 0x04
    // 将配置信息写进PCF8563的0x01开始的1个寄存器
    I2C_WRITE(PCF8563_ADDR, 0x01, &cs2, 1);
}

// 封装思想: 高内聚, 低耦合
void PCF8563_ext_int_call(){
#if USE_ALARM || USE_TIMER
    u8 cs2;    
    // 先将cs2内容读出来
    I2C_READ(PCF8563_ADDR, 0x01, &cs2, 1);
    
#if USE_ALARM
    // 判断Alarm Flag(AF) && AIE    
    if( (cs2 & (1 << 3)) && (cs2 & (1 << 1))) {
        // 清理cs2的AF标记---------------------------------------
        PCF8563_clear_alarm();
        // 执行Alarm任务
        PCF8563_on_alarm();
    }
#endif
    
#if USE_TIMER
    // 判断Timer Flag(TF) && TIE    
    if( (cs2 & (1 << 2)) && (cs2 & (1 << 0))) {
        // 清理cs2的TF标记---------------------------------------
        PCF8563_clear_timer();   
        // 执行Timer任务     
        PCF8563_on_timer();
    }
#endif
    
#endif
}


