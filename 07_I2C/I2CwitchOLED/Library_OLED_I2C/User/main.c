#include "gd32f4xx.h"
#include "systick.h"
#include <stdio.h>
#include <string.h>
#include "main.h"
#include "I2C.h"
#include "USART0.h"

//#include "PCF8563.h"
#include "OLED/oled.h"
#include "OLED/bmp.h"

/************************
任务目标: 使用CPU拉高拉低GPIO软实现I2C

*************************/

void USART0_on_recv(uint8_t* data, uint32_t len) {
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
    // 初始化I2C
	I2C_init();

	// printf("Init complete!\n");
    u8 t=' ';
    OLED_Init();				//初始化OLED
    while(1) {
        OLED_ShowPicture(0,0,128,64,BMP1,1);
        OLED_Refresh();
        delay_1ms(500);
        OLED_Clear();
        OLED_ShowChinese(0,0,0,16,1);//中
        OLED_ShowChinese(18,0,1,16,1);//景
        OLED_ShowChinese(36,0,2,16,1);//园
        OLED_ShowChinese(54,0,3,16,1);//电
        OLED_ShowChinese(72,0,4,16,1);//子
        OLED_ShowChinese(90,0,5,16,1);//技
        OLED_ShowChinese(108,0,6,16,1);//术
        OLED_ShowString(8,16,"ZHONGJINGYUAN",16,1);
        OLED_ShowString(20,32,"2014/05/01",16,1);
        OLED_ShowString(0,48,"ASCII:",16,1);
        OLED_ShowString(63,48,"CODE:",16,1);
        OLED_ShowChar(48,48,t,16,1);//显示ASCII字符
        t++;
        if(t>'~')t=' ';
        OLED_ShowNum(103,48,t,3,16,1);
        OLED_Refresh();
        delay_1ms(500);
        OLED_Clear();
        OLED_ShowChinese(0,0,0,16,1);  //16*16 中
        OLED_ShowChinese(16,0,0,24,1); //24*24 中
        OLED_ShowChinese(24,20,0,32,1);//32*32 中
        OLED_ShowChinese(64,0,0,64,1); //64*64 中
        OLED_Refresh();
        delay_1ms(500);
        OLED_Clear();

        char* str = "ABC";
        OLED_ShowString(0,0,"ABC",8,1);//6*8 “ABC”
        OLED_ShowString(0,8,"ABC",12,1);//6*12 “ABC”
        OLED_ShowString(0,20,"ABC",16,1);//8*16 “ABC”
        OLED_ShowString(0,36,str,24,1);//12*24 “ABC”
        OLED_Refresh();
        delay_1ms(500);
        OLED_ScrollDisplay(11,4,1);

    }
}
