#include "I2C.h"


#define SCL_RCU     RCU_GPIOB
#define SCL_PORT    GPIOB
#define SCL_PIN     GPIO_PIN_6

#define SDA_RCU     RCU_GPIOB
#define SDA_PORT    GPIOB
#define SDA_PIN     GPIO_PIN_7

#define i2cx    I2C0

void I2C_init(){    
    // 硬实现: 片上外设, AF复用
    
    // 标准模式: 100kbit/s (100kbps), 快速模式400kbit/s
    // 1 000 000 us / 100 000bit =  10us/bit     =>    5us休眠
    // 1 000 000 us / 400 000bit = 2.5us/bit     => 1.25us休眠
    
    // GPIO引脚 --------------------------------------------------
    // 初始化SCL引脚为上拉复用AF
    rcu_periph_clock_enable(SCL_RCU);
    gpio_mode_set(SCL_PORT, GPIO_MODE_AF, GPIO_PUPD_PULLUP, SCL_PIN);
    gpio_output_options_set(SCL_PORT, GPIO_OTYPE_OD, GPIO_OSPEED_MAX, SCL_PIN);
    gpio_af_set(SCL_PORT, GPIO_AF_4, SCL_PIN);
    // 初始化SDA引脚为上拉复用AF
    rcu_periph_clock_enable(SDA_RCU);
    gpio_mode_set(SDA_PORT, GPIO_MODE_AF, GPIO_PUPD_PULLUP, SDA_PIN);
    gpio_output_options_set(SDA_PORT, GPIO_OTYPE_OD, GPIO_OSPEED_MAX, SDA_PIN);
    gpio_af_set(SDA_PORT, GPIO_AF_4, SDA_PIN);
    
    // I2C外设 --------------------------------------------------
    // 时钟
    rcu_periph_clock_enable(RCU_I2C0);
    /* 重置 reset I2C */
    i2c_deinit(i2cx);
    /* 配置速度, 低高电平比例 configure I2C clock */
    i2c_clock_config(i2cx, 100000, I2C_DTCY_2);
    /* configure I2C address */
    i2c_mode_addr_config(i2cx, I2C_I2CMODE_ENABLE, I2C_ADDFORMAT_7BITS, 0x00);
    /* 启用  */
    i2c_enable(i2cx);
    
    /* whether or not to send an ACK */
    i2c_ack_config(i2cx, I2C_ACK_ENABLE);
}

#define	TIMEOUT	50000
// 等待信号变为SET, 返回0为成功
static uint8_t I2C_wait(uint32_t flag) {
    uint16_t cnt = 0;

    while(!i2c_flag_get(i2cx, flag)) {
        cnt++;
        if(cnt > TIMEOUT) return 1;
    }
    return 0;
}

// 等待信号变为RESET, 返回0为成功
static uint8_t I2C_waitn(uint32_t flag) {
    uint16_t cnt = 0;

    while(i2c_flag_get(i2cx, flag)) {
        cnt++;
        if(cnt > TIMEOUT) return 1;
    }
	return 0;
}

/**********************************************************
 * @brief I2C写数据到指定设备的寄存器
 * @param addr 设备地址7bit
 * @param reg  寄存器地址 
 * @param data 字节数组
 * @param len  数据个数
 * @return 错误码: 0无错误
 **********************************************************/
int8_t I2C_write(uint8_t addr, uint8_t reg, uint8_t* data, uint32_t len){
    uint8_t write_addr = addr << 1;
    
    /************* start ***********************/
    // 等待I2C闲置
    if(I2C_waitn(I2C_FLAG_I2CBSY)) return 1;
    // start
    i2c_start_on_bus(i2cx);
    // 等待I2C主设备成功发送起始信号
    if(I2C_wait(I2C_FLAG_SBSEND)) return 2;
    
    /************* device address **************/
    // 发送设备地址
    i2c_master_addressing(i2cx, write_addr, I2C_TRANSMITTER);
    // 等待地址发送完成
    if(I2C_wait(I2C_FLAG_ADDSEND)) return 3;    
    i2c_flag_clear(i2cx, I2C_FLAG_ADDSEND);
    
    /************ register address ************/
    // 寄存器地址
    // 等待发送数据缓冲区为空
    if(I2C_wait(I2C_FLAG_TBE)) return 4;
    // 发送数据
    i2c_data_transmit(i2cx, reg);
    // 等待数据发送完成
    if(I2C_wait(I2C_FLAG_BTC)) return 5;
    
    /***************** data ******************/
    // 发送数据
    uint32_t i;
    for(i = 0; i < len; i++) {

        // 等待发送数据缓冲区为空
        if(I2C_wait(I2C_FLAG_TBE)) return 6;

        // 发送数据
        i2c_data_transmit(i2cx, data[i]);

        // 等待数据发送完成
        if(I2C_wait(I2C_FLAG_BTC)) return 7;
    }
    
    /***************** stop ********************/
    // stop
    i2c_stop_on_bus(i2cx);

    while(I2C_CTL0(I2C0) & I2C_CTL0_STOP);    
    i2c_ack_config(i2cx, I2C_ACK_ENABLE);
    
    return 0;
}

/**********************************************************
 * @brief I2C从指定设备的寄存器读取数据
 * @param addr 设备地址7bit
 * @param reg  寄存器地址 
 * @param data 用来接收数据的字节数组
 * @param len  要读取的数据个数
 * @return 错误码: 0无错误
 **********************************************************/
int8_t I2C_read(uint8_t addr, uint8_t reg, uint8_t* data, uint32_t len){
    uint8_t write_addr = addr << 1;
    uint8_t read_addr  = (addr << 1) | 1;
    // 等待I2C空闲
    if(I2C_waitn(I2C_FLAG_I2CBSY)) return 1;
    
    /************* start ***********************/
    // 发送启动信号
    i2c_start_on_bus(i2cx);
    // 等待I2C主设备成功发送起始信号
    if(I2C_wait(I2C_FLAG_SBSEND)) return 2;
    
    /************* device address **************/
    // 发送从设备写地址
    i2c_master_addressing(i2cx, write_addr, I2C_TRANSMITTER);
    if(I2C_wait(I2C_FLAG_ADDSEND)) return 3;
    i2c_flag_clear(i2cx, I2C_FLAG_ADDSEND);
    
    /********** register address **************/
    // 等待发送缓冲区	
    if(I2C_wait(I2C_FLAG_TBE)) return 4;
    // 发送寄存器地址
    i2c_data_transmit(i2cx, reg);
    // 等待发送数据完成	
    if(I2C_wait(I2C_FLAG_BTC)) return 5;
    
    /************* start ***********************/
    // 发送再启动信号
    i2c_start_on_bus(i2cx);
    if(I2C_wait(I2C_FLAG_SBSEND)) return 7;
    
    /************* device address **************/
    // 发送从设备读地址
    i2c_master_addressing(i2cx, read_addr, I2C_RECEIVER);
    if(I2C_wait(I2C_FLAG_ADDSEND)) return 8;
    i2c_flag_clear(i2cx, I2C_FLAG_ADDSEND);

    /************* data **************/
    //ack
    i2c_ack_config(i2cx, I2C_ACK_ENABLE);
    // 接收一个数据后，自动发送ACK
    i2c_ackpos_config(i2cx, I2C_ACKPOS_CURRENT);
    // 确认ACK已启用
    if(I2C_wait(I2C_CTL0(i2cx) & I2C_CTL0_ACKEN)) return 11;

    // 读取数据
    uint32_t i;
    for (i = 0; i < len; i++) {
        if (i == len - 1) {
            // 在读取最后一个字节之前，禁用ACK，配置为自动NACK
            i2c_ack_config(i2cx, I2C_ACK_DISABLE);
        }

        // 等待接收缓冲区不为空
        if(I2C_wait(I2C_FLAG_RBNE)) return 10;
        
        data[i] = i2c_data_receive(i2cx);
    }
    
    /***************** stop ********************/
    i2c_stop_on_bus(i2cx);
    while(I2C_CTL0(I2C0) & I2C_CTL0_STOP);
    
    i2c_ack_config(i2cx, I2C_ACK_ENABLE);
    
    return 0;
}