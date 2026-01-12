#include "gd32f4xx.h"
#include "systick.h"
#include <stdio.h>
#include "main.h"
#include "USART0.h"
#include "FreeRTOS.h"
#include "task.h"

/************************
模板工程: 学习如何创建任务

*************************/
void USART0_on_recv(uint8_t* data, uint32_t len){

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

void GPIO_init(){
    GPIO_config(RCU_GPIOC, GPIOC, GPIO_PIN_6);
    GPIO_config(RCU_GPIOD, GPIOD, GPIO_PIN_12);
    GPIO_config(RCU_GPIOD, GPIOD, GPIO_PIN_14);
}

TaskHandle_t xStartTask_Handle;
TaskHandle_t xLed1Task_Handle;
TaskHandle_t xLed2Task_Handle;

void vTaskLed1(){

    while(1){
        gpio_bit_toggle(GPIOD, GPIO_PIN_12);        
        vTaskDelay(1000);        
    }
        
//    gpio_bit_toggle(GPIOD, GPIO_PIN_12);        
//    vTaskDelay(1000);  

//    gpio_bit_toggle(GPIOD, GPIO_PIN_12);        
//    vTaskDelay(1000);  

//    gpio_bit_toggle(GPIOD, GPIO_PIN_12);        
//    vTaskDelay(1000);  

//    gpio_bit_toggle(GPIOD, GPIO_PIN_12);        
//    vTaskDelay(1000); 
    
    // 如果任务有可能结束, 一定要销毁自己
    vTaskDelete(NULL);
}

void vTaskLed2(){

    while(1){
        gpio_bit_toggle(GPIOD, GPIO_PIN_14);        
        vTaskDelay(500);
    }
    
    // 如果任务有可能结束, 一定要销毁自己
    vTaskDelete(NULL);
}

void sys_init(){
    GPIO_init();
    USART0_init();
}


void vTaskFunc(uint8_t *pvParameters){
    // 1. 初始化外设
    sys_init();
    
    printf("Init Complete!\n");
    
    uint32_t cnt = 0;
    // 2. 创建2个独立的点灯任务
    xTaskCreate(
        (TaskFunction_t)   vTaskLed1,   // 任务函数的指针
        (const char *)   "vTaskLed1",   // 任务名称, 最大长度取决于 configMAX_TASK_NAME_LEN
        (uint16_t)                64,   // 任务栈大小, 单位: Word字(32位 4字节)  128 x 4字节, 任务越复杂所需空间越大
        (uint8_t *)              NULL,  // 任务函数参数, 通常使用NULL
        (UBaseType_t)              1,   // 任务的优先级, 数值越大, 优先级越高, 最大值为configMAX_PRIORITIES-1
        (TaskHandle_t * ) &xLed1Task_Handle    // 任务句柄, 用于在需要时操作该任务
    );       
    xTaskCreate(
        (TaskFunction_t)   vTaskLed2,   // 任务函数的指针
        (const char *)   "vTaskLed2",   // 任务名称, 最大长度取决于 configMAX_TASK_NAME_LEN
        (uint16_t)                64,   // 任务栈大小, 单位: Word字(32位 4字节)  128 x 4字节, 任务越复杂所需空间越大
        (uint8_t *)              NULL,  // 任务函数参数, 通常使用NULL
        (UBaseType_t)              1,   // 任务的优先级, 数值越大, 优先级越高, 最大值为configMAX_PRIORITIES-1
        (TaskHandle_t * ) &xLed2Task_Handle    // 任务句柄, 用于在需要时操作该任务
    );         
    
    // 3. 删除当前任务
    vTaskDelete(NULL); // xStartTask_Handle
}

uint8_t params[] = {33, 44};

int main(void) {
    // 全局优先级分配规则：4抢占[0,15], 0响应
    nvic_priority_group_set(NVIC_PRIGROUP_PRE4_SUB0);

    // 创建第一个任务(初始化外设, 创建其他任务)
    BaseType_t rst = xTaskCreate(
        (TaskFunction_t)   vTaskFunc,   // 任务函数的指针
        (const char *)   "task_func",   // 任务名称, 最大长度取决于 configMAX_TASK_NAME_LEN
        (uint16_t)               128,   // 任务栈大小, 单位: Word字(32位 4字节)  128 x 4字节, 任务越复杂所需空间越大
        (uint8_t *)             NULL,   // 任务函数参数, 通常使用NULL
        (UBaseType_t)              1,   // 任务的优先级, 数值越大, 优先级越高, 最大值为configMAX_PRIORITIES-1
        (TaskHandle_t * ) &xStartTask_Handle    // 任务句柄, 用于在需要时操作该任务
    ); 
    //  rst:  pdPASS创建成功，pdFAIL失败 (通常栈空间不足时,才会失败)
    
    // 开启任务调度
    vTaskStartScheduler();
  
    while(1);  
}
