#include "gd32f4xx.h"
#include "systick.h"
#include <stdio.h>
#include "main.h"
#include "USART0.h"
#include "Buzzer.h"

/************************
任务目标: 使用基本定时器

使用逻辑分析仪, 查看PD15引脚的切换时间间隔
10000Hz
1000Hz
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

//// 天空之城
//// 音符
//u8 notes[] = {
//    N6, N7, 			 H1, N7, H1, H3, 	  N7, N3, N3, 			N6, N5, N6, H1, 		 N5, N0, N3, N3,		N4, N3, N4, H1,
//    N3, N0, H1, H1, H1,  N7, N4, N4, N7,	  N7, N0, N6, N7, 		H1, N7, H1, H3,			 N7, N0, N3, N3,		N6, N5, N6, H1,
//    N5, N0, N3, 		 N4, H1, N7, N7, H1,  H2, H2, H3, H1, N0,   H1, N7, N6, N6, N7, N5,	 N6, N0, H1, H2,		H3, H2, H3, H5,
//    H2, N0, N5,	N5,		 H1, N7, H1, H3,	  H3, N0, N0,			N6, N7, H1, N7, H2, H2,  H1, N5, N5, N0,		H4, H3, H2, H1,
//    H3, 		 		 H3, N0, H3,		  H6, H5, H5,			H3, H2, H1, N0, H1, 	 H2, H1, H2, H2, H5, 	H3, N0, H3,
//    H6,	H5,				 H3, H2, H1, N0, H1,  H2, H1, H2, H2, N7,   N6, N0,
//};

//// 延时时长
//u8 durations[] = {
//    2, 2, 				6, 2, 4, 4, 		  12, 2, 2, 			6, 2, 4, 4, 			8, 4, 2, 2,				6, 2, 2, 6,
//    8, 2, 2, 2, 2, 	    6, 2, 4, 4,			  8, 4, 2, 2, 			6, 2, 4, 4,				8, 4, 2, 2,				6, 2, 4, 4,
//    12, 2, 2, 			4, 2, 2, 4, 4,		  2, 2, 2, 4, 4, 		4, 2, 2, 2, 4, 4,		8, 4, 2, 2,				6, 2, 4, 4,
//    8, 4, 2, 2,			2, 2, 4, 4,			  8, 4, 4,				2, 2, 4, 4, 2, 2, 		6, 2, 4, 4,				4, 4, 4, 4,
//    16, 				8, 4,	4,			  8, 4, 4,				2, 2, 4, 2, 2, 			4, 2, 2, 2, 4, 		    8, 4, 4,
//    8, 8,				2, 2, 8, 2, 2, 		  4, 2, 2, 2, 4, 		8, 4, 
//};

void USART0_on_recv(uint8_t* data, uint32_t len){
    // 此代码是在中断里执行的, 不要做耗时操作 (delay_ms)
    printf("recv[%d]->%s\n", len, data);

}


int main(void) {
    // 配置全局中断优先级分组
    nvic_priority_group_set(NVIC_PRIGROUP_PRE2_SUB2);
    // 滴答定时器初始化
    systick_config();
    // 初始化串口
    USART0_init();
    
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
