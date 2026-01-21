#include "App.h"
#include "TIMER.h"
#include "ADC.h"
/*****************

让电机在[0, 300]旋转

逆时针: ADC值变大, 角度变大
顺时针: ADC值变小, 角度变小

******************/

#define clip_value(value, min, max)         \
        do {                                \
            if(value > max){                \
                value = max;                \
            }else if(value < min){          \
                value = min;                \
            }                               \
        }while(0)

/**********************************************************
 * @brief 驱动电机正反转
 * @param speed 运动速度 [-100, 100]
 *        正数 逆时针转
 *        负数 顺时针转
 **********************************************************/
void bsp_servo_update(float speed){ // -30
    
    clip_value(speed, -100, 100);
    
    if(speed >= 0){
        // 逆时针
        
        // 设置IN-的占空比
        TIMER_channel_update(TIMER2, TIMER_CH_0, speed);
        // 设置IN+的占空比
        TIMER_channel_update(TIMER2, TIMER_CH_1, 0);        
    }else {
        // 顺时针
        
        // 设置IN-的占空比
        TIMER_channel_update(TIMER2, TIMER_CH_0, 0);
        // 设置IN+的占空比
        TIMER_channel_update(TIMER2, TIMER_CH_1, -speed);  
    }
}

void App_servo_init(){
    
    // 初始化运动速度
//    bsp_servo_update(-80);
    bsp_servo_update(0);
}

uint16_t currentAngle = 0;

void App_servo_task(){ // 10ms

    // 上一个误差值
    static int last_cte = 0;
    static int cte_sum = 0;
    
    uint16_t adc_value = ADC0_get(ADC_INSERTED_CHANNEL_0);
    
//    printf("adc_value: %4d\n", adc_value);
    
    // [0, 4095] / 4095 => [0.0, 1.0] 
    currentAngle = adc_value * 360 / 4095; 
    
    // Cross Track Error 误差值
    int cte = targetAngle - currentAngle;
    
    
    // 计算P参数: 比例系数 * 目标值和当前值的差值  (Proportional)
    float p = kp * cte; 
    
    // 计算I参数: 积分系数 * 目标值和当前值的差值的累加  (Integral)
    cte_sum += cte;
    // 限制累计的误差范围, 避免大幅度调整导致难以平衡
    clip_value(cte_sum, -2000, 2000);
    float i = ki * cte_sum;
    
    // 计算D参数: 微分系数 * 目标值和当前值的差值的变化  (Derivative)
    float d = kd * (cte - last_cte);    
    last_cte = cte;
    
    float pid = p + i + d;
    
    // pid作为带方向的速度值
    bsp_servo_update(pid);
    
//    printf("current: %d target: %d cte: %d\n", currentAngle, targetAngle, cte);
}







