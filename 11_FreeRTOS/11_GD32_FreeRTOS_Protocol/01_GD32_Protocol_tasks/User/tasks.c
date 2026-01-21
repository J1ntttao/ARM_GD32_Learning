#include "tasks.h"
#include "App.h"


void Task_init(){     

    App_debug_init(); 
}

Task_t task_list[] = {
//      state       task_cnt   task_period     callback           callback_suspend
  {TASK_STATE_STOP,      0,           10,    App_debug_recv_task},       // 0
  {TASK_STATE_STOP,      0,          100,    App_debug_sendCur_task},    // 1
  {TASK_STATE_STOP,      0,         2000,    App_debug_sendPID_task},    // 2
};

// 任务数量
uint8_t task_len = sizeof(task_list) / sizeof(task_list[0]);


// 修改全局计数值(在中断函数里1ms调用一次, 用于任务切换, 不耗时)
void Task_switch_handler(void){
    for(uint8_t i = 0; i < task_len; i++) {
        Task_t* task = &task_list[i];        
        // 如果任务是挂起状态了, 直接跳过当前循环
        if(task->state == TASK_STATE_SUSPEND){
            continue;
        }        
        // 如果任务计数值task_cnt不是0, 减1
        if(task->task_cnt) task->task_cnt--;
        
        // 计数值为0, 切换任务为可运行状态
        if(task->task_cnt == 0){
            // 切换任务状态, 不在这里(终端)运行
            task->state = TASK_STATE_RUN;
            // 重置count计数值
            task->task_cnt = task->task_period;
        }
    }
}

// 任务执行的函数(在main的while里一直执行)
void Task_exec_handler(void){
 
    for(uint8_t i = 0; i < task_len; i++) { 
//        Task_t *task = &task_list[i];   // 可以使用task->代替task_list[i].        
        // 判断任务是否是RUN可运行状态
        if(task_list[i].state == TASK_STATE_RUN){
            // 真正执行函数任务的地方 !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
            task_list[i].callback();

            // 如果函数在执行时被修改了, 则无需切换成STOP
            if(task_list[i].state != TASK_STATE_RUN){
                continue;
            }
            
            // 重置任务为停止状态
            task_list[i].state = TASK_STATE_STOP;
        }
    }

}

// 挂起(暂停)指定任务
void Task_suspend(uint8_t index){
    if(index >= task_len) return;
    
    Task_t *task = &task_list[index];
    // 重置计数值
    task->task_cnt = task->task_period;  
    // 修改为挂起状态, 让计数值不再变化
    task->state = TASK_STATE_SUSPEND;
    // 执行暂停函数, 让任务及时清理资源
    if(task->callback_suspend != NULL){
        // 避免空指针异常, 不是NULL才执行
        task->callback_suspend();    
    }    
}

// 恢复(继续)指定任务
void Task_resume(uint8_t index){
    if(index >= task_len) return; 
    // 修改为STOP状态
    task_list[index].state = TASK_STATE_STOP;    
}

// 获取任务状态
uint8_t Task_get_task_state(uint8_t index){
    if(index >= task_len) return TASK_STATE_STOP;    
    return task_list[index].state;    
}


// 根据任务函数名字, 获取索引位置
int8_t Task_get_task_id(Task_cb func_name){
    if(func_name == NULL) return -1;
    
    for(uint8_t i = 0; i < task_len; i++) {
        if(task_list[i].callback == func_name){        
            return i;
        }
    }
    return -1;
}




