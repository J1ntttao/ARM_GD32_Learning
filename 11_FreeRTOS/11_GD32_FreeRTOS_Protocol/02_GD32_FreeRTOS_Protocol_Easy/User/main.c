#include "gd32f4xx.h"
#include "systick.h"
#include <stdio.h>
#include "main.h"
#include "USART0.h"
#include <string.h>
#include "App.h"
#include "../Common/utils.h"

#include "FreeRTOS.h"
#include "task.h"
/************************
模板工程:

1. 在串口中断里接收数据
 - 把数据放到消息队列

2. taskRecv 在独立的任务里, 阻塞等待消息
 - 收到消息进行解析

3. taskSend 在独立的任务里, 发送当前位置
 - [0 - 300]°循环发送当前角度 100ms
 - 循环发送当前PID, 2000ms

*************************/

TaskHandle_t            StartTask_Handler;
TaskHandle_t            Task1_Handler;
TaskHandle_t            Task2_Handler;



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

void GPIO_init() {

    GPIO_config(RCU_GPIOC, GPIOC, GPIO_PIN_6);

    GPIO_config(RCU_GPIOD, GPIOD, GPIO_PIN_8);
    GPIO_config(RCU_GPIOD, GPIOD, GPIO_PIN_9);
}

void start_task(void *pvParameters) {

    GPIO_init();
    // 初始化串口
    USART0_init();
    
    uint8_t f1 = isLittleEndian();
    uint8_t f2 = isFloatLittleEndian();
    printf("Init Complete: %d, %d!\n", f1 , f2);

    taskENTER_CRITICAL();
    // 创建任务1
    xTaskCreate((TaskFunction_t)taskRecv,
                (const char*   )"taskRecv",
                2048,
                NULL,
                4,
                (TaskHandle_t*  )&Task1_Handler);
    // 创建任务2
    xTaskCreate((TaskFunction_t)taskSend,
                (const char*   )"taskSend",
                512,
                NULL,
                2,
                (TaskHandle_t*  )&Task2_Handler);
    // 销毁自己
    vTaskDelete(StartTask_Handler);

    taskEXIT_CRITICAL();
}

int main(void) {
    // 全局优先级分配规则：4抢占[0,15], 0响应
    nvic_priority_group_set(NVIC_PRIGROUP_PRE4_SUB0);

    // 创建第一个任务
    xTaskCreate((TaskFunction_t)start_task,
                (const char*   )"start_task",
                128,
                NULL,
                1,
                (TaskHandle_t*  )&StartTask_Handler);
    // 开启调度器
    vTaskStartScheduler();

    while(1);
}
