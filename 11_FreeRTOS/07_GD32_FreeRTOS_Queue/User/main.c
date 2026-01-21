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

/************************
模板工程:
3个独立的任务

1. taskKey: 监听 PA0 按下发送消息（基本，复杂）
2. task1: 阻塞式等待消息（基本类型）
3. task2: 阻塞式等待消息（复杂类型）

-------------------
创建两个消息队列
创建 Create
接收 Receive
发送 Send

*************************/


TaskHandle_t xStartTask_Handle;
TaskHandle_t xTaskKey_Handle;
TaskHandle_t xTask1_Handle;
TaskHandle_t xTask2_Handle;

typedef struct {
    char buffer[64];
    float height;    
    uint8_t age;
} MyData_t;

QueueHandle_t xQueue1;
QueueHandle_t xQueue2;

void USART0_on_recv(uint8_t* data, uint32_t len) {

}

//.\Objects\GD32F407.axf: Error: L6218E: Undefined symbol vApplicationStackOverflowHook (referred from tasks.o).
void vApplicationStackOverflowHook(TaskHandle_t xTask, char * pcTaskName ){
    printf("栈溢出了：%s\n", pcTaskName);
}

void vTaskKey() {

    FlagStatus pre_state = RESET;
    
    static uint32_t value = 0;
    BaseType_t xSuccess = pdFALSE;
    while(1) {
        FlagStatus cur_state = gpio_input_bit_get(GPIOA, GPIO_PIN_0);
//        printf("cur_state: %d\n", cur_state);
        if(cur_state != pre_state) {
            pre_state = cur_state;
            if(cur_state == SET) { // 按键按下
                printf("消息队列： 发送\n");
                
                // 基本数据类型 --------------------------
                xSuccess = xQueueSend(xQueue1, &value, 0); // portMAX_DELAY
                // 等待时长为0，则队列满时，不阻塞
                if(xSuccess){
                    value++;
                }else {
                    printf("Send1 Failure [%d]\n", value);
                }
                
                // 复杂数据类型 --------------------------
                MyData_t data;
                // 字符串strcpy, 字节数组memcpy
                sprintf(data.buffer, "name_%d", value);
//                strcpy(data.buffer, "zhangsan");
                data.height = 172.34;
                data.age = 23 + value;
                
                xSuccess = xQueueSend(xQueue2, &data, 0); // portMAX_DELAY
                // 等待时长为0，则队列满时，不阻塞
                if(xSuccess){
                    value++;
                }else {
                    printf("Send2 Failure [%s]\n", data.buffer);
                }

            }
        }

        vTaskDelay(pdMS_TO_TICKS(10));
    }

}

void vTask1() {

    uint32_t queue_num = 0;
    
    BaseType_t xSuccess;
    while(1) {
        printf("task1 waiting..\n");
        xSuccess = xQueueReceive(xQueue1, &queue_num, portMAX_DELAY);
        
        if(xSuccess == pdTRUE){
            printf("xQueue1 读成功 queue_num: %d\n", queue_num);
        }else {
            printf("xQueue1 读失败\n");
        }

        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

void vTask2() {

    MyData_t data;
    
    BaseType_t xSuccess;
    while(1) {
        printf("task2 waiting..\n");
        xSuccess = xQueueReceive(xQueue2, &data, portMAX_DELAY);
        
        if(xSuccess == pdTRUE){
            printf("xQueue2 读成功 name: %s, height: %.2f, age: %d\n", 
                data.buffer, data.height, data.age);
        }else {
            printf("xQueue2 读失败\n");
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
}
void vTaskFunc(uint8_t *pvParameters) {
    // 1. 初始化外设
    sys_init();

    printf("Init Complete!\n");

    // 创建消息队列
    //    UBaseType_t uxQueueLength, 队列最大长度
    //    UBaseType_t uxItemSize     每个数据的字节数
    // 基本数据类型的消息队列
    xQueue1 = xQueueCreate(3, sizeof(uint32_t));
    if(xQueue1 != NULL) {
        printf("xQueue1 create successful!\n");
    }
    
    // 复杂数据类型的消息队列
    xQueue2 = xQueueCreate(10, sizeof(MyData_t));
    if(xQueue2 != NULL) {
        printf("xQueue2 create successful!\n");
    }


    // 进入临界区, 所有任务会被禁止切换，中断被屏蔽 ---------------------------
    taskENTER_CRITICAL();

    // 2. 创建1个独立的任务
    xTaskCreate( vTaskKey,  "vTaskKey",128, NULL, 4, &xTaskKey_Handle );
    xTaskCreate( vTask1,    "vTask1",  128, NULL, 2, &xTask1_Handle   );
    xTaskCreate( vTask2,    "vTask2",  128, NULL, 3, &xTask2_Handle   );

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
