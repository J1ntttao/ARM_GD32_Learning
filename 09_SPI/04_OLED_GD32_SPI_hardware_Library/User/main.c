#include "gd32f4xx.h"
#include "systick.h"
#include <stdio.h>
#include <string.h>
#include "main.h"
#include "USART0.h"
#include "SPI0.h"

#include "SPI_OLED/oled.h"
#include "SPI_OLED/bmp.h"

/************************
驱动SPI屏幕 

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
    SPI0_init();

    OLED_Init();				//初始化OLED
    OLED_ColorTurn(0);//0正常显示，1 反色显示
    OLED_DisplayTurn(0);//0正常显示 1 屏幕翻转显示
    while(1) {
        OLED_Display_128x64(bmp1);
        delay_1ms(500);
        OLED_Display_GB2312_string(0,0,"12864,带中文字库");	/*在第1页，第1列，显示一串16x16点阵汉字或8x16的ASCII字*/
        OLED_Display_GB2312_string(0,2,"16X16简体汉字库,");  /*显示一串16x16点阵汉字或8x16的ASCII字.以下雷同*/
        OLED_Display_GB2312_string(0,4,"或8X16点阵ASCII,");
        OLED_Display_GB2312_string(0,6,"或5X7点阵ASCII码");
        delay_1ms(500);
        OLED_Clear();
        OLED_Display_GB2312_string(24,0,"中景园电子");
        OLED_Display_GB2312_string(0,2,"主要生产OLED模块");
        OLED_Display_GB2312_string(0,4,"顾客至上真诚服务");
        OLED_Display_GB2312_string(0,6,"诚信与质量第一！");
        delay_1ms(500);
        OLED_Clear();
        OLED_Display_GB2312_string(0,0,"GB2312简体字库及");
        OLED_Display_GB2312_string(0,2,"有图型功能，可自");
        OLED_Display_GB2312_string(0,4,"编大字或图像或生");
        OLED_Display_GB2312_string(0,6,"僻字，例如：");
        OLED_Display_16x16(97,6,jiong1);					/*在第7页，第81列显示单个自编生僻汉字“囧”*/
        OLED_Display_16x16(113,6,lei1);
        delay_1ms(500);
        OLED_Clear();
        OLED_Display_GB2312_string(0,0,"<!@#$%^&*()_-+]/");	/*在第1页，第1列，显示一串16x16点阵汉字或8*16的ASCII字*/
        OLED_Display_string_5x7(0,2,"<!@#$%^&*()_-+]/;.,?[");/*在第3页，第1列，显示一串5x7点阵的ASCII字*/
        OLED_Display_string_5x7(0,3,"XY electronics Co., ");/*显示一串5x7点阵的ASCII字*/
        OLED_Display_string_5x7(0,4,"Ltd. established at  ");/*显示一串5x7点阵的ASCII字*/
        OLED_Display_string_5x7(0,5,"year 2010.Focus OLED ");/*显示一串5x7点阵的ASCII字*/
        OLED_Display_string_5x7(0,6,"Mobile:13265585975");/*显示一串5x7点阵的ASCII字*/
        OLED_Display_string_5x7(0,7,"Tel:0755-32910715");/*显示一串5x7点阵的ASCII字*/
        delay_1ms(500);
        OLED_Clear();
        OLED_Display_GB2312_string(0,0,"啊阿埃挨哎唉哀皑");	/*在第1页，第1列，显示一串16x16点阵汉字或8x16的ASCII字*/
        OLED_Display_GB2312_string(0,2,"癌蔼矮艾碍爱隘鞍");  /*显示一串16x16点阵汉字或8x16的ASCII字.以下雷同*/
        OLED_Display_GB2312_string(0,4,"氨安俺按暗岸胺案");
        OLED_Display_GB2312_string(0,6,"肮昂盎凹敖熬翱袄");
        delay_1ms(500);
        OLED_Clear();
        OLED_Display_GB2312_string(0,0,"鬟鬣麽麾縻麂麇麈");
        OLED_Display_GB2312_string(0,2,"麋麒鏖麝麟黛黜黝");
        OLED_Display_GB2312_string(0,4,"黠黟黢黩黧黥黪黯");
        OLED_Display_GB2312_string(0,6,"齄鼬鼹鼷鼽鼾鼢鼯");
        // ../User/main.c(96): warning: illegal character encoding in string literal [-Wno-invalid-source-encoding]
        delay_1ms(500);
        OLED_Clear();
    }
}
