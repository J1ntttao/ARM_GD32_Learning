#include "gd32f4xx.h"
#include "systick.h"
#include <stdio.h>
#include "main.h"
#include "USART0.h"
#include "TIMER.h"
#include "ADC.h"
#include "task_timer.h"

#include "tasks.h"

#include "App.h"

/************************
任务目标: 
*************************/

void USART0_on_recv(uint8_t* data, uint32_t len){
    // 此代码是在中断里执行的, 不要做耗时操作 (delay_ms)
    printf("recv[%d]->%s\n", len, data);

}

static void GPIO_config(rcu_periph_enum rcu, uint32_t port, uint32_t pin) {
    // 1. 时钟初始化
    rcu_periph_clock_enable(rcu);
    // 2. 配置GPIO 输入输出模式
    gpio_mode_set(port, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, pin);
    // 3. 配置GPIO 输出选项
    gpio_output_options_set(port, GPIO_OTYPE_PP, GPIO_OSPEED_MAX, pin);
    // 4. 默认输出电平
    gpio_bit_write(port, pin, RESET);
}


int main(void) {
    // 配置全局中断优先级分组
    nvic_priority_group_set(NVIC_PRIGROUP_PRE2_SUB2);
    // 滴答定时器初始化
    systick_config();
    // 初始化串口
    USART0_init();
    // 初始化TIMER
    TIMER_init();
    // 初始化ADC
    ADC_init();
    
    // 初始化timer
    task_timer_init();
    
    printf("Init Complete!\n");
    
    Task_init();
    
    while(1){        
        Task_exec_handler();
    }
    
}
