#include "gd32f4xx.h"
#include "systick.h"
#include <stdio.h>
#include "main.h"
#include "USART0.h"
#include "EXTI.h"

/************************
任务目标: 通过软件代码触发外部中断函数

串口接收消息, 根据不同的消息, 启动不同的中断

EXTI_0
EXTI_1
EXTI_2

*************************/

void USART0_on_recv(uint8_t* data, uint32_t len){
    // 此代码是在中断里执行的, 不要做耗时操作 (delay_ms)
    printf("recv[%d]->%s\n", len, data);
    
    // 通过软件代码触发外部中断函数
    switch(data[0]){
        case 0x00: exti_software_interrupt_enable(EXTI_0);break;
        case 0x01: exti_software_interrupt_enable(EXTI_1);break;
        case 0x02: exti_software_interrupt_enable(EXTI_2);break;
        default: break;
    }
}

void EXTI0_on_trig(){
    for(uint8_t i = 1; i < 11; i++){             
        printf("EXTI_0 校长打饭: %d\n", i);    
        delay_1ms(200); // 模拟正常计算耗时
    }
}

void EXTI1_on_trig(){
    for(uint8_t i = 1; i < 11; i++){             
        printf("EXTI_1 老师打饭: %d\n", i);    
        delay_1ms(350); // 模拟正常计算耗时
    }
}

void EXTI2_on_trig(){
    for(uint8_t i = 1; i < 11; i++){             
        printf("EXTI_2 同学打饭: %d\n", i);    
        delay_1ms(500); // 模拟正常计算耗时
    }
}

int main(void) {
    // 配置全局中断优先级分组
    nvic_priority_group_set(NVIC_PRIGROUP_PRE2_SUB2);
    // 滴答定时器初始化
    systick_config();
    // 初始化串口
    USART0_init();
    // 初始化外部中断
    EXTI_init();
    
    uint8_t cnt = 0;
    while(1) {
        delay_1ms(1000);        
			printf("MAIN 食堂大妈玩手机：%d\n", cnt++);
    }
}
