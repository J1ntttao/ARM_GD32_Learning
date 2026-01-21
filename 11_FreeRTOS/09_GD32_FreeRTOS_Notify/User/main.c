#include "gd32f4xx.h"
#include "systick.h"
#include <stdio.h>
#include <string.h>
#include "main.h"
#include "USART0.h"
#include "bsp_keys.h"

#include "FreeRTOS.h"
#include "task.h"
//#include "queue.h"
//#include "event_groups.h"

/************************
模板工程:
3个独立的任务

1. taskKey: 监听扩展板上的四个独立按键
    Key0: 模拟信号量         （二值，计数型）
    Key1: 发送通知（覆写）   （消息邮箱，消息队列）
    Key2: 发送通知（不覆写） （消息邮箱，消息队列）
    Key3: 事件组

2. task1: 信号量
3. task2: 通知
4. task3: 事件组

*************************/


TaskHandle_t xStartTask_Handle;
TaskHandle_t xTaskKey_Handle;
TaskHandle_t xTask1_Handle;
TaskHandle_t xTask2_Handle;
TaskHandle_t xTask3_Handle;


 
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

    static uint32_t ulValue = 0x00; // 通知数值
    
    switch(key){
        case 0: // --------------------------- 模拟信号量
            printf("xTaskNotifyGive->信号量\n");
            xTaskNotifyGive(xTask1_Handle);        
            break;
        case 1: // --------------------------- 模拟通知（覆写）消息队列
            printf("xTaskNotify->发送通知（覆写）0x%X\n", ulValue);
            // 如果xTask2_Handle已经有数据了（还没来得及收），覆盖现有的数据
            xTaskNotify(xTask2_Handle, ulValue++, eSetValueWithOverwrite);
            break;
        case 2: // --------------------------- 模拟通知（不覆写）消息队列
            printf("xTaskNotify->发送通知（不覆写）0x%X\n", ulValue);
            // 如果xTask2_Handle已经有数据了（还没来得及收），不能写进去
            xTaskNotify(xTask2_Handle, ulValue++, eSetValueWithoutOverwrite);
            break;
        case 3:
            printf("xTaskNotify->事件组：%d\n", ulValue);
//            xTaskNotify(xTask3_Handle, BIT_3, eSetBits);// 将指定bit设置为1
//            xTaskNotify(xTask3_Handle, BIT_0 | BIT_2, eSetBits);// 将指定bit设置为1        
//            xTaskNotify(xTask3_Handle, 1 << ulValue, eSetBits);// 将指定bit设置为1

            uint32_t pulPreviousNotifyValue;
            // 接收方Exit不清零， 才能查询到上一次的值
            xTaskNotifyAndQuery(xTask3_Handle, 1 << ulValue, eSetBits, &pulPreviousNotifyValue);
            printf("pre value: 0x%X\n", pulPreviousNotifyValue);
        
        
            ulValue++;
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

    while(1) {
        printf("task1 waiting..\n");
        // xClearCountOnExit:
        // pdTRUE： 退出时清零通知值，二值信号量
        // pdFALSE：退出时不清零通知值，只-1，计数型信号量
        uint32_t cnt = ulTaskNotifyTake(pdFALSE, portMAX_DELAY);
        
        // cnt是清理之前的信号个数
        printf("task1-----------> 信号个数：%d\n", cnt);
        
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

void vTask2() {
    uint32_t notify_val = 0;

    while(1) {
//        printf("task2 waiting..\n");
#if 0
        ulBitsToClearOnEntry, 传入时，将指定bit位清为0
        ulBitsToClearOnExit,  退出时，将指定bit位清为0
        pulNotificationValue, 通过指针拿到通知值
        xTicksToWait
#endif
        // 0xAABBCCDD
        xTaskNotifyWait(
            0x00000000,
            0xFFFFFFFF, // 0xFFFFFFFF
            &notify_val,
            portMAX_DELAY
        );
        
        printf("收到消息: 0x%X\n", notify_val);

        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

#define IS_BIT_SET(val, n)    (((val) & (1 << n) ) > 0)
#define IS_BIT_RESET(val, n)  (((val) & (1 << n) ) == 0)

void vTask3() {
    uint32_t notify_val = 0;
    while(1) {
//        printf("task3 waiting..\n");
#if 0
        ulBitsToClearOnEntry, 传入时，将指定bit位清为0
        ulBitsToClearOnExit,  退出时，将指定bit位清为0
        pulNotificationValue, 通过指针拿到通知值
        xTicksToWait
   index  7  6  5  4   3  2  1  0
   bits   0  0  0  0   0  0  0  0
    hex  80 40 20 10  08 04 02 01
#endif
        // 0xAABBCCDD
        xTaskNotifyWait(
            0x00000000,
            0xFFFFFFFF, // 0xFFFFFFFF  0x00000000
            &notify_val,
            portMAX_DELAY
        );
        printf("task3收到事件组: 0x%X\n", notify_val);
        
        // 判定事件组 3
        if(IS_BIT_SET(notify_val, 3)){
            printf("单个标志触发成功!\n");
        }
        // 判定事件组 0 && 2
        if(IS_BIT_SET(notify_val, 0) && IS_BIT_SET(notify_val, 2)){
            printf("多个与标志触发成功!\n");
        }
        // 判定事件组 1 || 4
        if(IS_BIT_SET(notify_val, 1) || IS_BIT_SET(notify_val, 4)){
            printf("多个或标志触发成功!\n");
        }
        
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


    // 进入临界区, 所有任务会被禁止切换，中断被屏蔽 ---------------------------
    taskENTER_CRITICAL();

    // 2. 创建1个独立的任务
    xTaskCreate( vTaskKey,  "vTaskKey",128, NULL, 2, &xTaskKey_Handle );
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
