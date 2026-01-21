#ifndef __APP_H__
#define __APP_H__

#include "gd32f4xx.h"
#include "tasks.h"

#include <stdio.h>
#include <string.h>
#include "utils.h"
#include "USART0.h"




// =============== Debug ==================
// 目标位置
extern uint16_t targetAngle;

// PID参数
extern float kp;
extern float ki;
extern float kd;

void App_debug_init();

void App_debug_recv_task();
// 100ms
void App_debug_sendCur_task();
// 2000ms
void App_debug_sendPID_task();

// ============== Servo ===================

// 当前位置
extern uint16_t currentAngle;
void App_servo_init();
void App_servo_task();

#endif