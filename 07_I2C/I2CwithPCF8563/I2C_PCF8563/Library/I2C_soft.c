#include "I2C_soft.h"
#include "systick.h"


#define SCL_RCU     RCU_GPIOB
#define SCL_PORT    GPIOB
#define SCL_PIN     GPIO_PIN_6

#define SDA_RCU     RCU_GPIOB
#define SDA_PORT    GPIOB
#define SDA_PIN     GPIO_PIN_7

#define DELAY()     delay_1us(5)

#define SDA(bit)    gpio_bit_write(SDA_PORT, SDA_PIN, bit ? SET : RESET)
#define SCL(bit)    gpio_bit_write(SCL_PORT, SCL_PIN, bit ? SET : RESET)

#define SDA_IN()    gpio_mode_set(SDA_PORT, GPIO_MODE_INPUT, GPIO_PUPD_PULLUP, SDA_PIN)
#define SDA_OUT()   gpio_mode_set(SDA_PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_PULLUP, SDA_PIN)
#define SDA_STATE() gpio_input_bit_get(SDA_PORT, SDA_PIN)

void I2C_soft_init(){
    
    
    // 软实现: CPU直接操作GPIO
    // 硬实现: 片上外设,AF复用
    
    // 标准模式: 100kbit/s (100kbps), 快速模式400kbit/s
    // 1 000 000 us / 100 000bit =  10us/bit     =>    5us休眠
    // 1 000 000 us / 400 000bit = 2.5us/bit     => 1.25us休眠
    
    // 将I2C引脚初始化为开漏输出(内部上拉)
    // PB6 SCL
    rcu_periph_clock_enable(SCL_RCU);
    gpio_mode_set(SCL_PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_PULLUP, SCL_PIN);
    gpio_output_options_set(SCL_PORT, GPIO_OTYPE_OD, GPIO_OSPEED_MAX, SCL_PIN);
    // PB7 SDA
    rcu_periph_clock_enable(SDA_RCU);
    gpio_mode_set(SDA_PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_PULLUP, SDA_PIN);
    gpio_output_options_set(SDA_PORT, GPIO_OTYPE_OD, GPIO_OSPEED_MAX, SDA_PIN);
    
    
}

// 函数声明
static void start();                // 开始信号
static void send(uint8_t data);     // 写地址|寄存器|数据
static uint8_t wait_ack();          // 等待响应(0成功, 其他失败)
static void stop();                 // 停止信号

static uint8_t recv();              // 接收一个字节
static void send_ack();             // 发送响应
static void send_nack();            // 发送响应

/**********************************************************
 * @brief I2C写数据到指定设备的寄存器
 * @param addr 设备地址7bit, 需要 << 1
 * @param reg  寄存器地址 
 * @param data 字节数组
 * @param len  数据个数
 * @return 错误码: 0无错误
 **********************************************************/
int8_t I2C_soft_write(uint8_t addr, uint8_t reg, uint8_t* data, uint32_t len){
    
    // 通用开头 --------------------------------------------↓
    // 发送开始信号
    start();
    
    // 发送设备地址(写地址)
    send(addr << 1);    // 0xA2
    // 等待响应
    if(wait_ack()) return 1; // 设备不存在
    
    // 发送寄存器地址
    send(reg);
    // 等待响应
    if(wait_ack()) return 2; // 寄存器不存在
    // 通用开头 --------------------------------------------↑
    
    // 循环发送所有的数据(字节)
    for(uint32_t i = 0; i < len; i++){
        // 发送数据
        send(data[i]);
        // 等待响应
        if(wait_ack()) return 3; // 数据写失败
    }
    
    // 发送停止信号
    stop();
    
    return 0;
}

/**********************************************************
 * @brief I2C从指定设备的寄存器读取数据
 * @param addr 设备地址7bit (例如0x51)
 * @param reg  寄存器地址 
 * @param data 用来接收数据的字节数组
 * @param len  要读取的数据个数
 * @return 错误码: 0无错误
 **********************************************************/
int8_t I2C_soft_read(uint8_t addr, uint8_t reg, uint8_t* data, uint32_t len){
    
    // 通用开头 --------------------------------------------↓
    // 发送开始信号
    start();
    
    // 发送设备地址(写地址)
    send(addr << 1);            // 0xA2
    // 等待响应
    if(wait_ack()) return 1; // 设备不存在
    
    // 发送寄存器地址
    send(reg);
    // 等待响应
    if(wait_ack()) return 2; // 寄存器不存在
    // 通用开头 --------------------------------------------↑
    
    // 再发送开始信号
    start();
    
    // 发送设备地址(读地址)
    send(addr << 1 | 0x01);    // 0xA3
    // 等待响应
    if(wait_ack()) return 3;   // 设备不存在
    
    /*******************循环接收数据*****************/
    for(uint32_t i = 0; i < len; i++){
        // 接收字节
        data[i] = recv();
        
        if(i != len - 1){
            send_ack(); // 发送响应信号
        }else {
            send_nack();// 发送空响应信号
        }
    }
    
    /************************************************/
    
    // 发送停止信号
    stop();
    
    return 0;
}


// 开始信号
static void start(){
    
    // 拉高两个引脚
    SDA(1);
    DELAY();
    SCL(1);
    DELAY();
    
    // 按顺序拉低
    SDA(0); // 核心: SCL高电平时, 拉低SDA
    DELAY();
    SCL(0);
    DELAY();
    
}

// 写地址|寄存器|数据
static void send(uint8_t data){
    // 8bit, 先发高位
    // 1101 0010
    // 101 0010            << 1
    // 01 0010            << 1
    
    //&1000 0000
    
    for(uint8_t i = 0; i < 8; i++){
        // 根据要发的最高位决定SDA高低
        if(data & 0x80){ // 获取最高位
           SDA(1); // 是1
        }else {
           SDA(0);
        }
        // 左移data
        data <<= 1;
        DELAY();
        
        SCL(1); // 从机在此时读数据
        DELAY();// 高电平数据有效期, SDA不能动
        SCL(0);
        DELAY();// 低电平数据非有效期, SDA可以修改
        
    }
    
}
// 等待响应(0成功, 其他失败)
static uint8_t wait_ack(){
    // 拉高SDA, 等待从设备拉低
    SDA(1);
    DELAY();
    
    // 拉高SCL, 同时释放SDA控制权(变为输入模式)
    SCL(1);
    SDA_IN();
    DELAY(); // 从机此时会拉低SDA
    
    if(SDA_STATE() == RESET){
        // 从机拉低了SDA, 应答成功
        SCL(0);
        SDA_OUT();
    }else {
        // 无人应答, 返回错误
        SCL(0);
        SDA_OUT();
        // 发送stop信号
        stop();
        return 1;
    }
    
    return 0;
}          
// 停止信号
static void stop(){
    
//    SCL(0);
//    DELAY();

    // 拉低SDA引脚
    SDA(0);
    DELAY();
    
    SCL(1); // 确保SCL是高电平
    DELAY();
    SDA(1); // 核心: SCL高电平时, 拉高SDA
    DELAY();
}

// 接收一个字节
static uint8_t recv(){
    // 释放SDA控制权, 进入输入模式
    SDA_IN();
    
    uint8_t cnt  = 8;    // 1字节->8bit
    uint8_t data = 0x00; // 空的容器,接收数据
    
    while(cnt--){       // 接收一个bit位(先收高位)
        // SCL拉低
        SCL(0);
        DELAY();        // 从机即可修改SDA数据
        
        SCL(1);         // 设置数据有效性
        
#if 0
        // 0000 0000 -> 1101 0010
        // 0000 0000
        // 0000 0001    7
        // 0000 0011    6
        // 0000 0110    5
        // 0000 1101    4
        // 0001 1010    3
        // 0011 0100    2
        // 0110 1001    1
        // 1101 0010    0
        data <<= 1;
        
        if(SDA_STATE()){
            data++;
        }
#endif
        
        // 0000 0000 -> 1101 0010
        // 1000 0000 7
        // 1100 0000 6
        // 1100 0000 5
        // 1101 0000 4
        // 1101 0000 3
        // 1101 0000 2
        // 1101 0010 1
        // 1101 0010 0
        
        if(SDA_STATE()){
            data |= (1 << cnt);
        }
        
        // SCL在高电平Delay一会儿, 进入下一个循环
        DELAY();
    }
    
    // SCL拉低
    SCL(0);
    
    return data;
}

// 发送响应
static void send_ack(){
    // 获取SDA控制权
    SDA_OUT();
    
    // 拉低SDA
    SDA(0);
    DELAY();
    
    // 拉高SCL
    SCL(1);
    DELAY();
    
    // 拉低SCL
    SCL(0);
    DELAY();
}
// 发送响应
static void send_nack(){
    // 获取SDA控制权
    SDA_OUT();
    
    // 拉低SDA
    SDA(1);
    DELAY();
    
    // 拉高SCL
    SCL(1);
    DELAY();
    
    // 拉低SCL
    SCL(0);
    DELAY();
} 

