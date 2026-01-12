#include "gd32f4xx.h"
#include "systick.h"
#include <stdio.h>
#include <string.h>
#include "main.h"
#include "USART0.h"
#include "SPI0.h"

#include "SPI_OLED/oled.h"
#include "SPI_OLED/bmp.h"

#include "bsp_w25q64.h"

/************************
驱动SPI屏幕

*************************/

uint8_t cnt = 0;
void USART0_on_recv(uint8_t* data, uint32_t len) {
    // 此代码是在中断里执行的, 不要做耗时操作 (delay_ms)
    printf("recv[%d]->%s\n", len, data);

    printf("recv: %X\r\n", data[0]);

    if(data[0] == 0x00) {
        printf("ID = %X\r\n",W25Q64_readID());
    } else if(data[0] == 0x01) {

        char buff[128];
        sprintf(buff, "hello: %d", cnt++);
        printf("write: %s (%d)\r\n", buff, strlen(buff));

        W25Q64_write((uint8_t*)buff, 0, strlen(buff));
    }  else if(data[0] == 0x02) {
        uint8_t buff[128] = {0};
        W25Q64_read(buff, 0, 16); // 0xFF

        printf("read: %s (%d)\r\n", (char*)buff, strlen((char*)buff));
    }
}



int main(void) {

    // 配置全局中断优先级分组
    nvic_priority_group_set(NVIC_PRIGROUP_PRE2_SUB2);
    // 滴答定时器初始化
    systick_config();
    // 初始化串口
    USART0_init();

    SPI0_init();
    // Flash  ------------------------------------------
    W25Q64_init();

    printf("Flash ID: 0x%X\n", W25Q64_readID());

    uint8_t buff[12] = {0};

    // 读取数据: 第1次
    W25Q64_read(buff, 0x00FF00, 8);
    printf("buff1 = %s %X\n", buff, buff[0]);

    // 写数据 GB2312 (GBK) 一个汉字2字节
    W25Q64_write((uint8_t *)"传智教育", 0x00FF00, 8);

    // 读取数据: 第2次
    W25Q64_read(buff, 0x00FF00, 8);
    printf("buff2 = %s\n", buff);

    // OLED ------------------------------------------
    OLED_Init();				//初始化OLED
    OLED_ColorTurn(0);//0正常显示，1 反色显示
    OLED_DisplayTurn(0);//0正常显示 1 屏幕翻转显示
    while(1) {
        OLED_Display_128x64(bmp1);
        delay_1ms(500);
        OLED_Display_GB2312_string(0, 0, "12864,带中文字库");  /*在第1页，第1列，显示一串16x16点阵汉字或8x16的ASCII字*/
        OLED_Display_GB2312_string(0, 2, "16X16简体汉字库,");  /*显示一串16x16点阵汉字或8x16的ASCII字.以下雷同*/
        OLED_Display_GB2312_string(0, 4, "或8X16点阵ASCII,");
        OLED_Display_GB2312_string(0, 6, "或5X7点阵ASCII码");

        delay_1ms(500);
        OLED_Clear();
    }
}
