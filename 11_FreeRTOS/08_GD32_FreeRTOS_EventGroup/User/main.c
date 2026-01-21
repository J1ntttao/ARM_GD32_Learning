#include "gd32f4xx.h"
#include "systick.h"
#include <stdio.h>
#include <string.h>
#include "main.h"
#include "USART0.h"
#include "bsp_keys.h"

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "event_groups.h"

/************************
模板工程:
3个独立的任务

1. taskKey: 监听扩展板上的四个独立按键
    Key0: 设置信号0    温度正常
    Key1: 设置信号1    气压正常
    Key2: 设置信号2    门关闭
    Key3: 清除所有信号

2. task1: 关心信号2     
3. task2: 关心信号0和1
4. task3: 关心信号0和2

*************************/


TaskHandle_t xStartTask_Handle;
TaskHandle_t xTaskKey_Handle;
TaskHandle_t xTask1_Handle;
TaskHandle_t xTask2_Handle;
TaskHandle_t xTask3_Handle;


EventGroupHandle_t eventgroup_handle;
 
void USART0_on_recv(uint8_t* data, uint32_t len) {

}

#define BIT_0 ( 1 << 0 )
#define BIT_1 ( 1 << 1 )
#define BIT_2 ( 1 << 2 )
#define BIT_3 ( 1 << 3 )
#define BIT_4 ( 1 << 4 )
#define BIT_5 ( 1 << 5 )
#define BIT_6 ( 1 << 6 )
#define BIT_7 ( 1 << 7 )
#define BIT_8 ( 1 << 8 )

void Keys_on_keydown(uint8_t key){

    switch(key){
        case 0:
            printf("SetBits->0\n");
            xEventGroupSetBits(eventgroup_handle, BIT_0);
            break;
        case 1:
            printf("SetBits->1\n");
            xEventGroupSetBits(eventgroup_handle, BIT_1);
            break;
        case 2:
            printf("SetBits->2\n");
            xEventGroupSetBits(eventgroup_handle, BIT_2);
            break;
        case 3:
            printf("ClearBits\n");
            xEventGroupClearBits(eventgroup_handle, BIT_0 | BIT_1 | BIT_2);
            break;
        default:
            break;
    }
    
};

void vTaskKey() {

    while(1) {
        
        bsp_keys_scan();
        
        vTaskDelay(pdMS_TO_TICKS(10));
    }

}

void vTask1() {

    EventBits_t uxBits;
    while(1) {
        printf("task1 waiting..\n");
#if 0
        EventBits_t xEventGroupWaitBits( EventGroupHandle_t xEventGroup,
                 const EventBits_t uxBitsToWaitFor,
                 const BaseType_t xClearOnExit,
                 const BaseType_t xWaitForAllBits,
                 TickType_t xTicksToWait ) PRIVILEGED_FUNCTION;
#endif
        // 0 0 0 0 - 0 0 0 0
        //             1
        uxBits = xEventGroupWaitBits(
            eventgroup_handle,  // 事件组句柄
            BIT_2,              // 关心的事件标志，多个用 |,+ 连在一起
            pdTRUE,             // 退出时是否清理标记(置0), 只会清理关心的标记
            pdTRUE,             // 所有关心的标志同时为1时，才解除阻塞
            portMAX_DELAY       // 等待超时时间： 一直等
        );
        
        // 0xFF00FF 打印的是清理之前的事件组bits情况
        printf("task1 -----------> uxBits: 0x%06X\n", uxBits);
        
        // 如果需要打印清理之后的值，可以用 uxBits = xEventGroupGetBits(eventgroup_handle);
        
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

void vTask2() {


    EventBits_t uxBits;
    while(1) {
        printf("task2 waiting..\n");
#if 0
        EventBits_t xEventGroupWaitBits( EventGroupHandle_t xEventGroup,
                 const EventBits_t uxBitsToWaitFor,
                 const BaseType_t xClearOnExit,
                 const BaseType_t xWaitForAllBits,
                 TickType_t xTicksToWait ) PRIVILEGED_FUNCTION;
#endif
        // 0 0 0 0 - 0 0 0 0
        //               1 1
        uxBits = xEventGroupWaitBits(
            eventgroup_handle,  // 事件组句柄
            BIT_0 | BIT_1,      // 关心的事件标志，多个用 |,+ 连在一起
            pdTRUE,             // 退出时是否清理标记(置0), 只会清理关心的标记
            pdTRUE,             // 所有关心的标志同时为1时，才解除阻塞
            portMAX_DELAY       // 等待超时时间： 一直等
        );
        
        // 0x00FF00 (3个字节 == 24bits)打印的是清理之前的事件组bits情况
        printf("task2 -----------> uxBits: 0x%06X\n", uxBits);
        
        // 如果需要打印清理之后的值，可以用 uxBits = xEventGroupGetBits(eventgroup_handle);
        
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}


void vTask3() {


    EventBits_t uxBits;
    while(1) {
        printf("task3 waiting..\n");
#if 0
        EventBits_t xEventGroupSync( EventGroupHandle_t xEventGroup,
                             const EventBits_t uxBitsToSet,
                             const EventBits_t uxBitsToWaitFor,
                             TickType_t xTicksToWait )
#endif
        
        // 0 0 0 0 - 0 0 0 0
        // 1           1 0 1
        uxBits = xEventGroupSync(eventgroup_handle,
                BIT_7,              // 提前设置的标志位
                BIT_0 | BIT_2,      // 关心的事件标志：等待的标志位
                portMAX_DELAY       // 等待时间
        );
               
        // 0x00FF00 (3个字节 == 24bits)打印的是清理之前的事件组bits情况
        printf("task3 -----------> uxBits: 0x%06X\n", uxBits);
        
        // 如果需要打印清理之后的值，可以用 uxBits = xEventGroupGetBits(eventgroup_handle);
        uxBits = xEventGroupGetBits(eventgroup_handle);
        printf("task3 -----------> after: 0x%06X\n", uxBits);
        
        
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}


void GPIO_init() {
    // 1. 时钟初始化
    rcu_periph_clock_enable(RCU_GPIOA);
    // 2. 配置GPIO 输入输出模式
    gpio_mode_set(GPIOA, GPIO_MODE_INPUT, GPIO_PUPD_NONE, GPIO_PIN_0);
}

void sys_init() {
    // 1. 初始化外设
    GPIO_init();
    USART0_init();
    
    bsp_keys_init();
}
void vTaskFunc(uint8_t *pvParameters) {
    // 1. 初始化外设
    sys_init();

    printf("Init Complete!\n");

    // 创建事件组
    eventgroup_handle = xEventGroupCreate();

    // 进入临界区, 所有任务会被禁止切换，中断被屏蔽 ---------------------------
    taskENTER_CRITICAL();

    // 2. 创建1个独立的任务
    xTaskCreate( vTaskKey,  "vTaskKey",128, NULL, 4, &xTaskKey_Handle );
    xTaskCreate( vTask1,    "vTask1",  128, NULL, 2, &xTask1_Handle   );
    xTaskCreate( vTask2,    "vTask2",  128, NULL, 3, &xTask2_Handle   );
    xTaskCreate( vTask3,    "vTask3",  128, NULL, 4, &xTask3_Handle   );

    // 退出临界区, 所有任务允许切换 -------------------------------------------
    taskEXIT_CRITICAL();

    // 3. 删除当前任务. 鲁棒性(健壮性，如果删除前忘了退出临界区，也不影响系统运行)
    vTaskDelete(NULL); // xStartTask_Handle
}


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
