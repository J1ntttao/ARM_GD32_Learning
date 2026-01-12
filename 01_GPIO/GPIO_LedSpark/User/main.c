#include "gd32f4xx.h"
#include "systick.h"
#include <stdio.h>
#include "bsp_leds.h"
/*********************
任务目标:
	按键点灯

	LED_SW  PC6 拉低导通总开关

	LED1  PD8
	LED2  PD9
	LED3  PD10
	LED4  PD11
	LED5  PD12
	LED6  PD13
	LED7  PD14
	LED8  PD15
模板工程

**********************/

int main(void) {

	// 系统滴答定时器初始化
	systick_config();
    bsp_leds_init();
	
	while(1) {

		bsp_leds_turn_on(1);
		bsp_leds_turn_on(2);
		bsp_leds_turn_on(3);
		bsp_leds_turn_on(4);
		bsp_leds_turn_on(5);
		bsp_leds_turn_on(6);
		bsp_leds_turn_on(7);
		bsp_leds_turn_on(8);
		delay_1ms(1000);
        
		bsp_leds_turn_off(1);
		bsp_leds_turn_off(2);
		bsp_leds_turn_off(3);
		bsp_leds_turn_off(4);
		bsp_leds_turn_off(5);
		bsp_leds_turn_off(6);
		bsp_leds_turn_off(7);
		bsp_leds_turn_off(8);
		delay_1ms(1000);
	}
	
}
