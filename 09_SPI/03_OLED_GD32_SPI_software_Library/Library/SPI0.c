#include "SPI0.h"

static void GPIO_config_output(rcu_periph_enum rcu, uint32_t port, uint32_t pin) {
  // 1. 时钟初始化
  rcu_periph_clock_enable(rcu);
  // 2. 配置GPIO 输入输出模式
  gpio_mode_set(port, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, pin);
  // 3. 配置GPIO 输出选项
  gpio_output_options_set(port, GPIO_OTYPE_PP, GPIO_OSPEED_MAX, pin);
  // 4. 默认输出电平
  gpio_bit_write(port, pin, RESET);
}
static void GPIO_config_input(rcu_periph_enum rcu, uint32_t port, uint32_t pin){
  rcu_periph_clock_enable(rcu);
  gpio_mode_set(port, GPIO_MODE_INPUT, GPIO_PUPD_PULLUP, pin);
}

#define SCLK(bit)    gpio_bit_write(GPIOA, GPIO_PIN_5, bit ? SET : RESET)
#define MOSI(bit)    gpio_bit_write(GPIOA, GPIO_PIN_7, bit ? SET : RESET)
#define MISO()       gpio_input_bit_get(GPIOA, GPIO_PIN_6)

void SPI0_init(){

    
    GPIO_config_output(RCU_GPIOA, GPIOA, GPIO_PIN_5);   // SCLK
    GPIO_config_output(RCU_GPIOA, GPIOA, GPIO_PIN_7);   // MOSI
        
    GPIO_config_input(RCU_GPIOA, GPIOA, GPIO_PIN_6);    // MISO
    
    
    // 默认电平取决于: 时钟极性CPOL
    SCLK(1);
}

void SPI0_write(uint8_t dat){ // 按照大端模式写出数据

    // 1101 0011
    for(uint8_t i = 0; i < 8; i++){
        // 拉低SCL
        SCLK(0);
        // 取出最高位发送数据(修改MOSI电平)
        MOSI(dat & 0x80); // 0x80 0x00
        // 拉高
        SCLK(1);
        // 将数据左移1位
        dat <<= 1;        
    }    
}

uint8_t SPI0_read(){
    
    // 大端模式 0000 0000 <- 1101 0011 先收到高位数据
    // 0000 0001    0
    // 0000 0011    1
    // 0000 0110    2
    // 0000 1101    3
    // 0001 1010    4
    // 0011 0100    5
    // 0110 1001    6
    // 1101 0011    7    
    uint8_t read = 0x00;
    for(uint8_t i = 0; i < 8; i++){
        // 拉低SCL
        SCLK(0);
        
        __NOP(); // 稍微延时一会儿, 给从机准备数据的时间
        
        // 拉高
        SCLK(1);      
        
        // 左移数据
        read <<= 1;
        // 如果收到1, 将最低位置1
        if(MISO()) read++;
          
    }
    
    return read;
}


uint8_t SPI0_read_backup(){
    
    // 大端模式 0000 0000 <- 1101 0011 先收到高位数据
    uint8_t read = 0x00;
    for(uint8_t i = 0; i < 8; i++){
        // 拉低SCL
        SCLK(0);
        
        // 左移数据
        read <<= 1;// 稍微延时一会儿, 给从机准备数据的时间        
        // 如果收到1, 将最低位置1
        if(MISO()) read++; 
        
        // 拉高
        SCLK(1);                
    }
    
    return read;
}
