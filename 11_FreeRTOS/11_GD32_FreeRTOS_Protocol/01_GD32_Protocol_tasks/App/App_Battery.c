#include "App.h"
#include "bsp_leds.h"


// 高内聚, 低耦合

typedef enum {
    STATE_SLEEP = 0,    // 休眠
    STATE_CHARGE = 1,   // 充电中
    STATE_STOPPING = 2, // 充电停止中(当前电量闪烁3次)
    STATE_SHOW, // 显示电量(当前电量闪烁3次)
} BATTERY_STATE;

BATTERY_STATE state = STATE_SLEEP;

void App_Battery_init(){
    // 初始化leds
    bsp_leds_init();   
}

// 0: [0,25)
// 1: [25, 50)
// 2: [50, 75)
// 3: [75, 98)
// 4: [98, 100)

// 真正的电量
uint8_t power = 2;  // 2,3,4, 2,3,4, 2,3,4

// 临时显示用的电量
uint8_t power_show = 0;

void App_Battery_start(){
    printf("开始充电: %d\n", power);
    
    // 临时显示用的电量
    power_show = 0;
    
    state = STATE_CHARGE; // 充电中
    
    // 恢复任务状态
    Task_resume(0);
}

void App_Battery_stop(){
    printf("停止充电: %d\n", power);
    state = STATE_STOPPING;
}

void App_Battery_update(){
    power++;
    printf("电量变化: %d\n", power);
}

void App_Battery_show(){    
    printf("显示电量: %d\n", power);
    if(state == STATE_CHARGE){
        return;
    }
    
    state = STATE_SHOW;
    // 恢复任务状态
    Task_resume(0);
}

#define LED1    1
#define LED2    3
#define LED3    5
#define LED4    7

/********************************
 * 状态机案例, 该函数200ms执行一次
 ********************************/
void App_Battery_task(){ // loop, 200ms
    switch(state){
        case STATE_SLEEP:
            
            bsp_leds_turn(LED1, 0);
            bsp_leds_turn(LED2, 0);
            bsp_leds_turn(LED3, 0);
            bsp_leds_turn(LED4, 0);
            printf("休眠\n");
        
            Task_suspend(0);
        
            break;
        case STATE_CHARGE:
            // 奥卡姆剃刀原理: 保持最简单的解决方案
            bsp_leds_turn(LED1, power_show >= 1);
            bsp_leds_turn(LED2, power_show >= 2);
            bsp_leds_turn(LED3, power_show >= 3);
            bsp_leds_turn(LED4, power_show >= 4);
            
            // 2,3,4,2,3,4,....
            if(++power_show > 4) power_show = power;
            printf("充电中power_show: %d\n", power_show);
            break;
        case STATE_SHOW:
        case STATE_STOPPING: {
            static uint8_t stopping_cnt = 0;        
            // 0, 1, 2, 3, 4, 5 偶数灭, 奇数亮(根据当前电量power显示)
            if(stopping_cnt % 2 == 0){
                bsp_leds_turn(LED1, 0);
                bsp_leds_turn(LED2, 0);
                bsp_leds_turn(LED3, 0);
                bsp_leds_turn(LED4, 0);
            }else{
                bsp_leds_turn(LED1, power >= 1);
                bsp_leds_turn(LED2, power >= 2);
                bsp_leds_turn(LED3, power >= 3);
                bsp_leds_turn(LED4, power >= 4);
            }
            
            if(STATE_SHOW == state){
                printf("显示电量\n"); 
            }else {            
                printf("充电停止中\n");
            }

            stopping_cnt++;
            if(stopping_cnt >= 6){
                stopping_cnt = 0;
                // 进入真正休眠
                state = STATE_SLEEP;
            }
            break;
        }
        default:
            break;
    }   
}

// 清理资源, 清空屏幕
void App_Battery_suspend(void){
    printf("充电LED任务被暂停\n");
}


