#ifndef __TASKS_H__
#define __TASKS_H__

#include "gd32f4xx.h"

#ifndef u8
#define u8 uint8_t
#endif

#ifndef u16
#define u16 uint16_t
#endif

#ifndef u32
#define u32 uint32_t
#endif

// 多任务管理器

#define  TASK_STATE_STOP        0 // 任务停止
#define  TASK_STATE_RUN         1 // 任务运行
#define  TASK_STATE_SUSPEND     2 // 任务挂起(暂停Pause) 

// 定义函数指针类型
typedef void (*Task_cb)(void);

// 定义结构体
typedef struct {
    uint8_t state;           // 任务状态

    uint16_t task_cnt;       // 任务的计数值(动态修改) 65535
    
    uint16_t task_period;    // 即多少ms执行一次(固定)
    
//    void (*callback)(void);   // 任务函数的指针变量
    Task_cb callback;           // 任务函数的指针变量
    Task_cb callback_suspend;   // 任务函数的指针变量(任务挂起回调)
} Task_t;

void Task_init();

// 任务切换函数(中断里调用)
void Task_switch_handler(void);

// 任务执行函数(main函数while里调用)
void Task_exec_handler(void);

// 挂起(暂停)指定任务
void Task_suspend(uint8_t index);

// 恢复(继续)指定任务
void Task_resume(uint8_t index);

#endif