#include "gd32f4xx.h"
#include "systick.h"
#include <stdio.h>
#include "main.h"
#include "USART0.h"
#include <string.h>

/************************
任务目标: 配置窗口看门狗

*************************/

void USART0_on_recv(uint8_t* data, uint32_t len){
    // 此代码是在中断里执行的, 不要做耗时操作 (delay_ms)
    printf("recv[%d]->%s\n", len, data);
    
    if(data[0] == 0x44){
        while(1); // 模拟耗时卡死操作
    }
}

#define COUNTER         0x7F // 127
#define WINDOW          0x50 // 80
#define WINDOW_END      0x3F // 63

// 初始化窗口看门狗
void WWDGT_config(){
    // 1. 启用RCU
    rcu_periph_clock_enable(RCU_WWDGT);
    
    // 2. 配置wwdgt
    /* 配置窗口看门狗参数 configure counter value, window value, and prescaler divider value 
    counter: 0x00 - 0x7F   计数器的初始值 Max: 0x7F -> 127 
    window: 0x00 - 0x7F    窗口值(最早)  (0x3F, 0x7F]  Max: 0x7F -> 127
    prescaler: wwdgt prescaler value 分频系数 1,2,4,8
    PCLK1  = 42M
    Freq   = PCLK1 / 4096 / 4 每秒计数个数 
           = 2563.48 Hz
    Period = 1000ms / 2563.48 Hz = 0.39ms (DIV4)
    
    窗口时间范围: ---------------------------------------------
    窗口开始时间: (COUNTER -     WINDOW) * Period = (127 - 80) * 0.39ms = 18.33ms
    窗口结束时间: (COUNTER - WINDOW_END) * Period = (127 - 63) * 0.39ms = 24.96ms
    
    只能在以下时间内喂狗, 否则会触发重启:
    18.33ms < time < 24.96ms
    */
    wwdgt_config(COUNTER, WINDOW, WWDGT_CFG_PSC_DIV4);

    /* start the window watchdog timer counter */
    wwdgt_enable();

}

int main(void) {
    // 配置全局中断优先级分组
    nvic_priority_group_set(NVIC_PRIGROUP_PRE2_SUB2);
    // 滴答定时器初始化
    systick_config();
    // 初始化串口
    USART0_init();
    
    // 初始化
    WWDGT_config();
    
    printf("Init Complete111!\n");
    
    
    int b  = 0;
    uint32_t cnt = 0;
    uint32_t feed = 0;
    while(1) { 
        //  18.33ms < time < 24.96ms
        
        // 模拟耗时阻塞
//        delay_1ms(17);
        delay_1ms(21);
//        delay_1ms(26);
        
        /* 喂狗 configure the window watchdog timer counter value */
        wwdgt_counter_update(COUNTER);
        
        printf("WWDGT喂狗! %d\n", feed++);

    }
}
