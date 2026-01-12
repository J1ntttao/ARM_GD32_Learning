#include "gd32f4xx.h"
#include "systick.h"
#include <stdio.h>
#include <string.h>
#include "main.h"

#include "USART0.h"
#include "I2C.h"
#include "PCF8563.h"

/************************
任务目标: 使用CPU拉高拉低GPIO软实现I2C

*************************/

void USART0_on_recv(uint8_t* data, uint32_t len){
    // 此代码是在中断里执行的, 不要做耗时操作 (delay_ms)
    printf("recv[%d]->%s\n", len, data);
}

int main(void) {
    Clock_t c;
    
    // 配置全局中断优先级分组
    nvic_priority_group_set(NVIC_PRIGROUP_PRE2_SUB2);
    // 滴答定时器初始化
    systick_config();
    // 初始化串口
    USART0_init();
    // 初始化I2C
    I2C_init();
 //   I2C_soft_init();
        // 提前准备要写的时间: 只需要写一次
    c.year = 2025, c.month = 9, c.day = 28, c.week = 2;
    c.hour = 23, c.minute = 59, c.second = 55;

    // 调用设置时钟函数, 传入结构体参数
    PCF8563_set_clock(c);
    
    while(1) { 
                // 循环 读取: 秒, 分, 时, 日, 周, 月, 年, 世纪
        PCF8563_get_clock(&c);
        // 打印数据
        printf("%02d-%02d-%02d ", (int)c.year, (int)c.month, (int)c.day);
        printf("%02d:%02d:%02d ", (int)c.hour, (int)c.minute, (int)c.second);
        printf("week-> %d\n", (int)c.week);
        
        delay_1ms(1000);
    }
}
