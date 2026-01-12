#include "gd32f4xx.h"
#include "systick.h"
#include <stdio.h>
#include "main.h"
#include "USART0.h"

void GPIO_config(void) {

		/********************* PA0 按键引脚 GD32上的KEY *********/
		// 时钟初始化
		rcu_periph_clock_enable(RCU_GPIOA);
		// 配置GPIO模式
		gpio_mode_set(GPIOA, GPIO_MODE_INPUT, GPIO_PUPD_NONE, GPIO_PIN_0);
		
		/********************* PC0 按键引脚 *********************/
		// 时钟初始化
		rcu_periph_clock_enable(RCU_GPIOC);
		// 配置GPIO模式
		gpio_mode_set(GPIOC, GPIO_MODE_INPUT, GPIO_PUPD_PULLUP, GPIO_PIN_0);
		

		/********************* PB2 LED1引脚 *********************/
		// 1. 时钟初始化
		rcu_periph_clock_enable(RCU_GPIOB);
		// 2. 配置GPIO 输入输出模式
		gpio_mode_set(GPIOB, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO_PIN_2);
		// 3. 配置GPIO 模式的操作方式
		gpio_output_options_set(GPIOB, GPIO_OTYPE_PP, GPIO_OSPEED_2MHZ, GPIO_PIN_2);
		// 初始化为低电平
		gpio_bit_reset(GPIOB, GPIO_PIN_2);
		
			// PC6 LED_SW 
				// 1. 时钟初始化
				rcu_periph_clock_enable(RCU_GPIOC);
				// 2. 配置GPIO 输入输出模式
				gpio_mode_set(GPIOC, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO_PIN_6);
				// 3. 配置GPIO 模式的操作方式
				gpio_output_options_set(GPIOC, GPIO_OTYPE_PP, GPIO_OSPEED_2MHZ, GPIO_PIN_6);
				// 初始化为低电平
				gpio_bit_reset(GPIOC, GPIO_PIN_6);
			// PD8 LED1 
				// 1. 时钟初始化
				rcu_periph_clock_enable(RCU_GPIOD);
				// 2. 配置GPIO 输入输出模式
				gpio_mode_set(GPIOD, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO_PIN_8);
				// 3. 配置GPIO 模式的操作方式
				gpio_output_options_set(GPIOD, GPIO_OTYPE_PP, GPIO_OSPEED_2MHZ, GPIO_PIN_8);
				// 初始化为1电平
				gpio_bit_set(GPIOD, GPIO_PIN_8);
}

void callback(){
		gpio_bit_reset(GPIOD, GPIO_PIN_8);
		delay_1ms(100);
		gpio_bit_set(GPIOD, GPIO_PIN_8);
}	
	

int main(void) {
    // 滴答定时器初始化
    systick_config();
    // 初始化串口
    USART0_init();
		GPIO_config();
	
    // 发送字符串
    USART0_send_string("Init success\r\n");
    		
		FlagStatus pre_state = RESET;// 默认低电平抬起
		FlagStatus pre_state1 = SET;// 默认低电平抬起
		while(1) {
				FlagStatus state = gpio_input_bit_get(GPIOA, GPIO_PIN_0);
				if (state != pre_state){
						if(state == SET){  // 当前高电平, 上一次为低电平，按下开灯
								gpio_bit_set(GPIOB, GPIO_PIN_2);
								printf("PA0 high => PB2 high\n");
						}else {						 // 当前低电平, 上一次为高电平，抬起关灯
								gpio_bit_reset(GPIOB, GPIO_PIN_2);
								printf("PA0 low => PB2 low\n");
						}
						pre_state = state;	
				}
				
				FlagStatus state1 = gpio_input_bit_get(GPIOC, GPIO_PIN_0);
				if (state1 != pre_state1){
						if(state1 == SET){   // 当前高电平, 上一次为低电平，抬起关灯
								gpio_bit_reset(GPIOB, GPIO_PIN_2);
								printf("PC0 high => PB2 low\n");			
						}else {						 	// 当前低电平, 上一次为高电平，按下开灯
								gpio_bit_set(GPIOB, GPIO_PIN_2);
								printf("PC0 low => PB2 low\n");							
						}
						pre_state1 = state1;	
				}
		}
}
