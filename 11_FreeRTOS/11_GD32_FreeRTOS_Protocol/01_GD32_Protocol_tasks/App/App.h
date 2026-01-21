#ifndef __APP_H__
#define __APP_H__

#include "gd32f4xx.h"
#include "tasks.h"

#include <stdio.h>
#include <string.h>
#include "utils.h"
#include "USART0.h"

void App_debug_init();

void App_debug_recv_task();

// 100ms
void App_debug_sendCur_task();

// 2000ms
void App_debug_sendPID_task();

#endif