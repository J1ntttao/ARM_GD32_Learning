#include "gd32f4xx.h"
#include "systick.h"
#include <stdio.h>
#include <string.h>
#include "main.h"
#include "USART0.h"


#define I2C0_SCL_RCU     RCU_GPIOB
#define I2C0_SCL_PORT    GPIOB
#define I2C0_SCL_PIN     GPIO_PIN_6
// PB7,PB9
#define I2C0_SDA_RCU     RCU_GPIOB
#define I2C0_SDA_PORT    GPIOB
#define I2C0_SDA_PIN     GPIO_PIN_7

/************************
任务目标: 使用CPU拉高拉低GPIO软实现I2C

*************************/

void USART0_on_recv(uint8_t* data, uint32_t len) {
    // 此代码是在中断里执行的, 不要做耗时操作 (delay_ms)
    printf("recv[%d]->%s\n", len, data);
}
/* USER CODE BEGIN Includes */
#include "u8g2/u8g2.h"

static void GPIO_config(rcu_periph_enum rcu, uint32_t port, uint32_t pin) {
    // 1. 时钟初始化
    rcu_periph_clock_enable(rcu);
    // 2. 配置GPIO 输入输出模式
    gpio_mode_set(port, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, pin);
    // 3. 配置GPIO 输出选项
    gpio_output_options_set(port, GPIO_OTYPE_OD, GPIO_OSPEED_MAX, pin);
    // 4. 默认输出电平
    gpio_bit_write(port, pin, RESET);
}

uint8_t u8g2_gpio_and_delay_gd32(U8X8_UNUSED u8x8_t *u8x8, U8X8_UNUSED uint8_t msg, U8X8_UNUSED uint8_t arg_int, U8X8_UNUSED void *arg_ptr)
{
    switch(msg) {
    //Initialize SPI peripheral
    case U8X8_MSG_GPIO_AND_DELAY_INIT:
        /* HAL initialization contains all what we need so we can skip this part. */
        GPIO_config(RCU_GPIOB, GPIOB, GPIO_PIN_6);
        GPIO_config(RCU_GPIOB, GPIOB, GPIO_PIN_7);

        break;

    //Function which implements a delay, arg_int contains the amount of ms
    case U8X8_MSG_DELAY_MILLI:
        delay_1ms(arg_int);

        break;
    //Function which delays 10us
    case U8X8_MSG_DELAY_10MICRO:
        delay_1us(10);
        break;
    //Function which delays 100ns
    case U8X8_MSG_DELAY_100NANO:
        __NOP();

        break;
    case U8X8_MSG_GPIO_I2C_CLOCK:

        if (arg_int) gpio_bit_write(I2C0_SCL_PORT, I2C0_SCL_PIN, SET);
        else gpio_bit_write(I2C0_SCL_PORT, I2C0_SCL_PIN, RESET);

        break;

    case U8X8_MSG_GPIO_I2C_DATA:
        if (arg_int) gpio_bit_write(I2C0_SDA_PORT, I2C0_SDA_PIN, SET);
        else gpio_bit_write(I2C0_SDA_PORT, I2C0_SDA_PIN, RESET);
        break;

    default:
        return 0; //A message was received which is not implemented, return 0 to indicate an error
    }

    return 1; // command processed successfully.
}

void test_draw(u8g2_t *u8g2)
{
    u8g2_ClearBuffer(u8g2);  // 清空缓冲区
    u8g2_SetFontMode(u8g2, 1);
    u8g2_SetFontDirection(u8g2, 0); // 设置方向
    u8g2_SetFont(u8g2, u8g2_font_fub20_tf); // 设置字体
    u8g2_DrawStr(u8g2, 0, 28, "ICHEIMA");  // 设置x,y坐标及字体内容

    u8g2_DrawHLine(u8g2, 2, 35, 47); // 水平线起点，x,y，长度
    u8g2_DrawHLine(u8g2, 3, 36, 47);
    u8g2_DrawVLine(u8g2, 45, 32, 12);
    u8g2_DrawVLine(u8g2, 46, 33, 12);

    u8g2_SetFont(u8g2, u8g2_font_6x10_tf);
    u8g2_DrawStr(u8g2, 1,54,"www.icheima.com");

    u8g2_SendBuffer(u8g2); // 上面步骤画完，一定要记得将数据发送到屏幕
    delay_1ms(1000);
}
// 参考：https://github.com/olikraus/u8g2/blob/bde09fbf787892c79a184e88b124aa5c79393aed/sys/arm/lpc824/u8g2_logo/main.c#L34
void test_draw_logo(u8g2_t *u8g2)
{
    u8g2_ClearBuffer(u8g2);  // 清空缓冲区
    u8g2_SetFontMode(u8g2, 1);	// Transparent
    u8g2_SetFontDirection(u8g2, 0);
    u8g2_SetFont(u8g2, u8g2_font_inb24_mf);
    u8g2_DrawStr(u8g2, 0, 30, "U");
    
    u8g2_SetFontDirection(u8g2, 1);
    u8g2_SetFont(u8g2, u8g2_font_inb30_mn);
    u8g2_DrawStr(u8g2, 21,8,"8");
        
    u8g2_SetFontDirection(u8g2, 0);
    u8g2_SetFont(u8g2, u8g2_font_inb24_mf);
    u8g2_DrawStr(u8g2, 51,30,"g");
    u8g2_DrawStr(u8g2, 67,30,"\xb2");
    
    u8g2_DrawHLine(u8g2, 2, 35, 47);
    u8g2_DrawHLine(u8g2, 3, 36, 47);
    u8g2_DrawVLine(u8g2, 45, 32, 12);
    u8g2_DrawVLine(u8g2, 46, 33, 12);
  
    u8g2_SetFont(u8g2, u8g2_font_4x6_tr);
    u8g2_DrawStr(u8g2, 1,54,"github.com/olikraus/u8g2");
	u8g2_SendBuffer(u8g2); // 上面步骤画完，一定要记得将数据发送到屏幕
	delay_1ms(200);
}
int main(void) {

    // 配置全局中断优先级分组
    nvic_priority_group_set(NVIC_PRIGROUP_PRE2_SUB2);
    // 滴答定时器初始化
    systick_config();
    // 初始化串口
    USART0_init();

    u8g2_t u8g2;

    u8g2_Setup_ssd1306_i2c_128x64_noname_f(&u8g2, U8G2_R0, u8x8_byte_sw_i2c, u8g2_gpio_and_delay_gd32);
    u8g2_InitDisplay(&u8g2); // send init sequence to the display, display is in sleep mode after this,
    u8g2_SetPowerSave(&u8g2, 0); // wake up display

    delay_1ms(200);

    printf("init success\r\n");

    // 画两条线试试
    u8g2_DrawLine(&u8g2, 0,0, 127, 63);
    u8g2_DrawLine(&u8g2, 127,0, 0,63);
    u8g2_SendBuffer(&u8g2); // 将数据发送到屏幕

    delay_1ms(1000);
    test_draw(&u8g2);
    
    delay_1ms(1000);
    test_draw_logo(&u8g2);
    while(1) {

    }
}
