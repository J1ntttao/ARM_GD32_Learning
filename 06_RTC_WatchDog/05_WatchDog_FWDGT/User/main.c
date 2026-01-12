#include "gd32f4xx.h"
#include "systick.h"
#include <stdio.h>
#include "main.h"
#include "USART0.h"
#include <string.h>

/************************
任务目标: 配置独立看门狗

*************************/

void USART0_on_recv(uint8_t* data, uint32_t len){
    // 此代码是在中断里执行的, 不要做耗时操作 (delay_ms)
    printf("recv[%d]->%s\n", len, data);
    
    if(data[0] == 0x44){
        while(1); // 模拟耗时卡死操作
    }
}

// 初始化独立看门狗
void FWDGT_config(){
    // 不需要使能RCU, 但是要启用内部低速晶振IRC32K
    rcu_osci_on(RCU_IRC32K);
    // 等待晶振稳定
    while(SUCCESS != rcu_osci_stab_wait(RCU_IRC32K));
    
    /* 配置重载值 configure counter reload value, and prescaler divider value 
    reload_value: 重装载寄存器, prescaler_div: 预分频系数
    
    reload_value: 12位向下递减计数器[0x0000 - 0x0FFF] 初始值 Max: 4095
    prescaler_div: 预分频系数, 将32K进行降频(4,8,16,32,64,128,256)
    
    32000Hz / 16 = 2000Hz 每秒数2000次, 每次0.5ms
    32000Hz / 32 = 1000Hz 每秒数1000次, 每次1ms
    32000Hz / 64 =  500Hz  每秒数500次, 每次2ms
    
    
    目标重启时间: target_ms = 2000ms, 超过此时间不喂狗, 触发重启
    
    reload_value = target_ms / 一个周期的时长(ms)
                 = target_ms / (1000ms / Freq)
                 = target_ms / (1000ms / (32000Hz / PSC))
                 = 2000ms / (1000ms / (32000Hz / 64))
                 = 1000    
    
    */
    fwdgt_config(2000, FWDGT_PSC_DIV32);
//    fwdgt_config(1000, FWDGT_PSC_DIV64);

    /* start the free watchdog timer counter */
    fwdgt_enable();

}

int main(void) {
    // 配置全局中断优先级分组
    nvic_priority_group_set(NVIC_PRIGROUP_PRE2_SUB2);
    // 滴答定时器初始化
    systick_config();
    // 初始化串口
    USART0_init();
    
    printf("Init Complete111!\n");
    
    FWDGT_config();
    
    int b  = 0;
    uint32_t cnt = 0;
    uint32_t feed = 0;
    while(1) { 
        delay_1ms(1);
        
        if(++cnt > 900){ // 900ms
            cnt = 0;
            
            printf("嘿喂狗! %d\n", feed++);
            // 喂狗
            /* reload the counter of FWDGT */ 
            fwdgt_counter_reload(); // gd32f4xx_fwdgt.c中定义的
        }
        
//        if(feed == 5){
//            while(1);
//        }
    }
}
