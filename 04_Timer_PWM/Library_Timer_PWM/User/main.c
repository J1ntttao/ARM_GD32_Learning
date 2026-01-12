#include "gd32f4xx.h"
#include "systick.h"
#include <stdio.h>
#include "main.h"
#include "USART0.h"
#include "TIMER.h"
#include "Buzzer.h"

/************************
任务目标: 使用通用定时器

TIMER3CH0   PD12: LED5
TIMER3CH1   PD13: LED6
TIMER3CH2   PD14: LED7
TIMER3CH3   PD15: LED8

*************************/


#define	L1	1
#define	L2	2
#define	L3	3
#define	L4	4
#define	L5	5
#define	L6	6
#define	L7	7

#define N0 0

#define	N1	L1 + 7
#define	N2	L2 + 7
#define	N3	L3 + 7
#define	N4	L4 + 7
#define	N5	L5 + 7
#define	N6	L6 + 7
#define	N7	L7 + 7

#define	H1	N1 + 7
#define	H2	N2 + 7
#define	H3	N3 + 7
#define	H4	N4 + 7
#define	H5	N5 + 7
#define	H6	N6 + 7
#define	H7	N7 + 7


u8 notes[] = {
	L5,N1,N1,N3, N6,N3,N5, N5,N6,N5,N3, N4,N3,N2, 
	L6,N2,N2,N4, N7,N7,N6,N5, N4,N0,N4,N3, L6,L7,N1,
	N2, N2,N0, L5,N1,N1,N3, N6,N3,N5,
	N5,N6,N5,N3, N4,N3,N2, L6,N2,N2,N4, N7,N7,N6,N5,
	N4,N4,N3, L7,N2, N1, N1,N0,L5,
	
};

u8 durations[] = {
	3,1,3,1, 3,1,3, 3,1,3,1, 3,1,3,
	3,1,3,1, 3,1,3,1, 2,2,3,1, 2,4,2,
	8, 4,4, 3,1,3,1, 3,1,3,
	3,1,3,1, 3,1,3, 3,1,3,1, 3,1,3,1,
	4,3,1, 4,4, 8, 4,3,1,
};
void USART0_on_recv(uint8_t* data, uint32_t len){
    // 此代码是在中断里执行的, 不要做耗时操作 (delay_ms)
    printf("recv[%d]->%s\n", len, data);

}

static void GPIO_config(rcu_periph_enum rcu, uint32_t port, uint32_t pin) {
    // 1. 时钟初始化
    rcu_periph_clock_enable(rcu);
    // 2. 配置GPIO 输入输出模式
    gpio_mode_set(port, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, pin);
    // 3. 配置GPIO 输出选项
    gpio_output_options_set(port, GPIO_OTYPE_PP, GPIO_OSPEED_MAX, pin);
    // 4. 默认输出电平
    gpio_bit_write(port, pin, RESET);
}

#define LED1_2   TIMER0, TIMER_CH_0

void test_timer_adv(){
    // 0 -> 100 -> 0 -> 100
    float duty = 0;
    int8_t dir = 1;
    while(1) {
        if(duty >= 100) dir = -1;
        else if(duty <= 0) dir = 1;        
        duty += dir;
        
        TIMER_channel_update(LED1_2, duty);
     
        delay_1ms(10);        
    }
}

void test_buzzer(){
    Buzzer_init();
    
    u16 len = sizeof(notes) / sizeof(notes[0]);
    
    for(u16 i = 0; i < len; i++){
        // 按照指定音调输出
        Buzzer_beep(notes[i]);
        
        // 根据每个音调进行休眠
        delay_1ms(durations[i] * 100);
        
        // 短暂的间隔
        Buzzer_stop();
        delay_1ms(20);
    }
    
    // 停止
    Buzzer_stop();
    while(1) {
     
        delay_1ms(10);        
    }
}

#define LED5     TIMER3, TIMER_CH_0
#define LED6     TIMER3, TIMER_CH_1
#define LED7     TIMER3, TIMER_CH_2
#define LED8     TIMER3, TIMER_CH_3
void test_leds(){
    // 0 -> 100 -> 0 -> 100
    float duty = 0;
    int8_t dir = 1;
    while(1) {
        if(duty >= 100) dir = -1;
        else if(duty <= 0) dir = 1;        
        duty += dir;
        
        // 更新指定TIMER定时器通道的占空比
        TIMER_channel_update(LED5, duty);
        TIMER_channel_update(LED6, duty);
        TIMER_channel_update(LED7, duty);
        TIMER_channel_update(LED8, duty);
//        printf("duty->%.2f\n", duty);
     
        delay_1ms(10);        
    }
}

// 舵机接收的PWM信号频率为50HZ，即周期为20ms。当高电平的脉宽在0.5ms-2.5ms之间时舵机就可以对应旋转到不同的角度。
//	0.5 ms 	  0 			0.5/20			 5/200			0.025
//	  1 ms		45				1/20			10/200      0.050
//	1.5 ms 	  90			1.5/20			15/200      0.075
//	  2	ms		135				2/20			20/200      0.100
//	2.5 ms		180			2.5/20			25/200      0.125
//T = 20ms

double mapToRange(double angle) {
    // 确保输入在有效范围内
    if (angle < 0) angle = 0;
    if (angle > 359) angle = 359;
    
    // 线性映射计算
    return 0.025 + (angle / 360.0) * (0.225 - 0.025);
}

void Angle2Per(double angle){	// 5 - 25	
		// [0, 180] -> [0.025, 0.125]
		double duty = mapToRange(angle);
		duty = duty * 100.0f;
		printf("duty:%.2f\n", duty);
	
		TIMER_channel_update(TIMER0, TIMER_CH_0, duty);
}

int main(void) {
    // 配置全局中断优先级分组
    nvic_priority_group_set(NVIC_PRIGROUP_PRE2_SUB2);
    // 滴答定时器初始化
    systick_config();
    // 初始化串口
    USART0_init();    
    // 初始化所有TIMER
    TIMER_init();
    
    // 初始化并拉低总开关
    GPIO_config(RCU_GPIOC, GPIOC, GPIO_PIN_6);    
    
//    test_timer_adv();
//    test_buzzer();
//    test_leds();
	
	  double angle = 0.0;

		while(1){
				angle = 180.0;
				printf("angle:%.1f \n",angle);
				Angle2Per(angle);
				delay_1ms(400);
			
				angle = 0.0;
				printf("angle:%.1f \n",angle);
				Angle2Per(angle);
				delay_1ms(400);
		}
	
}
