#include "SPI0.h"


#if SPI0_SOFT

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


#define SCL(bit)     gpio_bit_write(SPI0_SCL_GPIO, bit ? SET : RESET)
#define MOSI(bit)    gpio_bit_write(SPI0_MOSI_GPIO, bit ? SET : RESET)
#define MISO()       gpio_input_bit_get(SPI0_MISO_GPIO)

void SPI0_init(){

    
    GPIO_config_output(SPI0_SCL_RCU, SPI0_SCL_GPIO);   // SCLK
    GPIO_config_output(SPI0_MOSI_RCU, SPI0_MOSI_GPIO);   // MOSI
        
    GPIO_config_input(SPI0_MISO_RCU, SPI0_MISO_GPIO);    // MISO
    
    
    // 默认电平取决于: 时钟极性CPOL
    SCL(1);
}

void SPI0_write(uint8_t dat){ // 按照大端模式写出数据

    // 1101 0011
    for(uint8_t i = 0; i < 8; i++){
        // 拉低SCL
        SCL(0);
        // 取出最高位发送数据(修改MOSI电平)
        MOSI(dat & 0x80); // 0x80 0x00
        // 拉高
        SCL(1);
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
        SCL(0);
        
        __NOP(); // 稍微延时一会儿, 给从机准备数据的时间
        
        // 拉高
        SCL(1);      
        
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
        SCL(0);
        
        // 左移数据
        read <<= 1;// 稍微延时一会儿, 给从机准备数据的时间        
        // 如果收到1, 将最低位置1
        if(MISO()) read++; 
        
        // 拉高
        SCL(1);                
    }
    
    return read;
}

uint8_t SPI0_write_read(uint8_t dat){
    uint8_t read = 0x00;
    for(uint8_t i = 0; i < 8; i++){
        // 拉低SCL ---------------------------
        SCL(0);
        
        // 取出最高位发送数据(修改MOSI电平)
        MOSI(dat & 0x80); // 0x80 0x00
        // 将数据左移1位
        dat <<= 1;    
        
        // 拉高SCL ---------------------------
        SCL(1);      
        
        // 左移数据
        read <<= 1;
        // 如果收到1, 将最低位置1
        if(MISO()) read++;        
    }
    
    
    return read;
}


#else

static void GPIO_config(rcu_periph_enum rcu, uint32_t port, uint32_t pin) {
  // 1. 时钟初始化
  rcu_periph_clock_enable(rcu);
  // 2. 配置GPIO 输入输出模式
  gpio_mode_set(port, GPIO_MODE_AF, GPIO_PUPD_NONE, pin);
  // 3. 配置GPIO 输出选项
  gpio_output_options_set(port, GPIO_OTYPE_PP, GPIO_OSPEED_MAX, pin);
  // 4. 设置引脚AF复用
  gpio_af_set(port, GPIO_AF_5, pin);
}



// SPI硬件实现
void SPI0_init(){
    // GPIO ----------------------------------------------------    
    GPIO_config(SPI0_SCL_RCU, SPI0_SCL_GPIO);       // SCLK
    GPIO_config(SPI0_MOSI_RCU, SPI0_MOSI_GPIO);     // MOSI        
    GPIO_config(SPI0_MISO_RCU, SPI0_MISO_GPIO);     // MISO
    
    // SPI  ----------------------------------------------------
    rcu_periph_clock_enable(RCU_SPI0);
    
    /* 重置 deinitialize SPI and I2S */
    spi_i2s_deinit(SPI0);
    spi_parameter_struct spi_struct;    
    /* 初始化默认参数 initialize the parameters of SPI struct with default values */
    spi_struct_para_init(&spi_struct);
    
    spi_struct.device_mode          = SPI_MASTER;                // 设备模式: 主机
    spi_struct.trans_mode           = SPI_TRANSMODE_FULLDUPLEX;  // 传输模式: 全双工
    spi_struct.frame_size           = SPI_FRAMESIZE_8BIT;        // 每帧bit数: 8bit
    spi_struct.nss                  = SPI_NSS_SOFT;              // 软片选
    spi_struct.clock_polarity_phase = SPI_CK_PL_HIGH_PH_2EDGE;   // CPOL:1, CPHA:1
    spi_struct.prescale             = SPI_PSC_32;                 // 84MH / 8 = 10.5M
//    spi_struct.prescale             = SPI_PSC_128;                 // 84MH / 8 = 10.5M
    spi_struct.endian               = SPI_ENDIAN_MSB;            // 大小端模式
    
    /* 执行SPI的初始化 initialize SPI parameter */
    spi_init(SPI0, &spi_struct);
    /* 启用SPI enable SPI */
    spi_enable(SPI0);

}

void SPI0_write(uint8_t dat){
    /* 循环等待发送缓冲区, 直到为空 */
    while(RESET == spi_i2s_flag_get(SPI0, SPI_FLAG_TBE));    
    /* SPI transmit data */
    spi_i2s_data_transmit(SPI0, dat);
    
    /* 循环等待接收缓冲区, 直到不为空 */
    while(RESET == spi_i2s_flag_get(SPI0, SPI_FLAG_RBNE));     
    /* SPI receive data */
    spi_i2s_data_receive(SPI0);
}

uint8_t SPI0_read(){
    /* 循环等待发送缓冲区, 直到为空 */
    while(RESET == spi_i2s_flag_get(SPI0, SPI_FLAG_TBE));    
    /* SPI transmit data */
    spi_i2s_data_transmit(SPI0, 0x00);
    
    /* 循环等待接收缓冲区, 直到不为空 */
    while(RESET == spi_i2s_flag_get(SPI0, SPI_FLAG_RBNE));     
    /* SPI receive data */
    uint8_t dat = spi_i2s_data_receive(SPI0);
    return dat;
}

uint8_t SPI0_write_read(uint8_t dat){
    /* 循环等待发送缓冲区, 直到为空 */
    while(RESET == spi_i2s_flag_get(SPI0, SPI_FLAG_TBE));    
    /* SPI transmit data */
    spi_i2s_data_transmit(SPI0, dat);
    
    /* 循环等待接收缓冲区, 直到不为空 */
    while(RESET == spi_i2s_flag_get(SPI0, SPI_FLAG_RBNE));     
    /* SPI receive data */
    return spi_i2s_data_receive(SPI0);
}


#endif
















