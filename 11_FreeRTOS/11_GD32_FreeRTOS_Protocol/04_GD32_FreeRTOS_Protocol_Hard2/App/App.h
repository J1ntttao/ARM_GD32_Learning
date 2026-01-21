#ifndef __APP_H__
#define __APP_H__

#include "gd32f4xx.h"

#include <stdio.h>
#include <string.h>

#include "../Common/utils.h"
#include "FreeRTOS.h"
#include "task.h"

// Ç¯Î»ºê
#define CLIP_VALUE(value, min, max) do {\
	if (value < min) value = min;		\
	else if (value > max) value = max;	\
} while(0)

void taskSend(void *pvParameters);
void taskRecv(void *pvParameters);

#endif