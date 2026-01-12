#include "gd32f4xx.h"
#include "systick.h"
#include <stdio.h>
#include <string.h>
#include "main.h"

#include "USART0.h"
#include "I2C_soft.h"

/************************
任务目标: 使用CPU拉高拉低GPIO软实现I2C

*************************/

void USART0_on_recv(uint8_t* data, uint32_t len){
    // 此代码是在中断里执行的, 不要做耗时操作 (delay_ms)
    printf("recv[%d]->%s\n", len, data);
}

uint8_t read_data[3] = {0};

int main(void) {
    // 配置全局中断优先级分组
    nvic_priority_group_set(NVIC_PRIGROUP_PRE2_SUB2);
    // 滴答定时器初始化
    systick_config();
    // 初始化串口
    USART0_init();
    // 初始化I2C
    I2C_soft_init();
    
    uint8_t dev_addr = 0x51; // 设备地址7bit PCF8563
//    uint8_t dev_addr = 0x3C; // 设备地址7bit I2C屏幕
    uint8_t dev_reg  = 0x02; // 秒钟寄存器
    uint8_t data[] = {0x56, 0x59, 0x10}; // 秒,分,时
    // 向0x51(0xA2)设备的0x02寄存器写0x12, 0x11, 0x10
    int8_t rst = I2C_soft_write(dev_addr, dev_reg, data, 3);
        
    printf("rst->%d\n", rst);
    
    while(1) { 
        
        I2C_soft_read(dev_addr, dev_reg, read_data, 3);
        
        printf("read_data: %02X:%02X:%02X\n", read_data[2], read_data[1], read_data[0]);
        
        delay_1ms(1000);
    }
}
