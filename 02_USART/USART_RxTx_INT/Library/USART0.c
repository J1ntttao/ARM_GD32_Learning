#include "USART0.h"
#include <stdio.h>
#include <string.h>

void callback();
void USART0_on_recv(uint8_t* data, uint32_t len){
    // 此代码是在中断里执行的, 不要做耗时操作 (delay_ms)
    printf("RECV(len=%d) -> %s\n", len, data);
    callback();
}

static void USART_config(){
    // GPIO引脚初始化 --------------------------------------------------------
    /* 启用GPIO时钟 */    
    rcu_periph_clock_enable(RCU_GPIOA);
    
    /* 配置PA9的GPIO模式 configure USART0 TX as alternate function push-pull */
    gpio_mode_set(GPIOA, GPIO_MODE_AF, GPIO_PUPD_PULLUP, GPIO_PIN_9);
    gpio_output_options_set(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_9);

    /* 配置PA10的GPIO模式 configure USART0 RX as alternate function push-pull */
    gpio_mode_set(GPIOA, GPIO_MODE_AF, GPIO_PUPD_PULLUP, GPIO_PIN_10);
    // gpio_output_options_set(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_10);
    
    /* 复用功能配置 configure the USART0 TX pin and USART0 RX pin */
    gpio_af_set(GPIOA, GPIO_AF_7, GPIO_PIN_9);
    gpio_af_set(GPIOA, GPIO_AF_7, GPIO_PIN_10);
    
    // 串口初始化 -----------------------------------------------------------
    /* 启用USART0时钟 */    
    rcu_periph_clock_enable(RCU_USART0);
    /* 重置复位USART0 */
    usart_deinit(USART0);
    /* 配置串口参数: 波特率*, 数据位, 校验位, 停止位, 大小端模式 */
    usart_baudrate_set(USART0, 115200); 
    usart_word_length_set(USART0, USART_WL_8BIT);   // 数据位: 默认8bit
    usart_parity_config(USART0, USART_PM_NONE);     // 校验位: 默认无校验
    usart_stop_bit_set(USART0, USART_STB_1BIT);     // 停止位: 默认1bit
    usart_data_first_config(USART0, USART_MSBF_LSB);// 数据模式: 小端模式
    /* 启用发送功能 */
    usart_transmit_config(USART0, USART_TRANSMIT_ENABLE);
    /* 启用接收功能 */
    usart_receive_config(USART0, USART_RECEIVE_ENABLE);
    
    // 中断初始化 --------------------------------------------------------------
    // 配置中断优先级 
    nvic_irq_enable(USART0_IRQn, 2, 2);
    // 启用中断
    // 启用RBNE中断: 读取数据缓冲区不为空中断read data buffer not empty interrupt
    usart_interrupt_enable(USART0, USART_INT_RBNE);
    // 启用IDLE中断: 当发送者停止发送数据, 空闲时间超过1个字符帧, 触发
    usart_interrupt_enable(USART0, USART_INT_IDLE);
    
    /* 启用USART */
    usart_enable(USART0);
}

void USART0_init() {
    USART_config();
}

// 发送1个字节数据
void USART0_send_data(uint8_t data){
    // 通过USART0发送字节
    usart_data_transmit(USART0, data);
    // 阻塞等待: 直到上边这个字节发送完毕(buffer为空, 返回SET, 循环结束解除阻塞)
    // Transmit data Buffer Empty 发送数据缓冲区为空
    while(RESET == usart_flag_get(USART0, USART_FLAG_TBE));    
}

// 发送多个字节(字节数字)
void USART0_send_data_array(uint8_t* arr, uint32_t len){
    while(arr && len--){ // 指针不为空, 长度 > 0
        USART0_send_data(*arr);
        arr++;
    }
}
// 发送字符串 \0
void USART0_send_string(char* str){
    // 循环继续条件: 1. data指针不能为空 2.要发的数据不是\0 (0x00字符串结束的标记)
    while(str && *str){ // (*str) != '\0'        
        USART0_send_data((uint8_t)*str);
        str++;
    }    
}

// 配置printf数据打印实现
int fputc(int ch, FILE *f){  
    USART0_send_data((uint8_t) ch);
    return ch;
} 

// // // // 不// 要// 用// 阻// 塞// 式// // // //
uint8_t usart0_recive_blocking(){
    // 阻塞式等待数据 Read data Buffer Not Empty
    while(RESET == usart_flag_get(USART0, USART_FLAG_RBNE));        
    // 接收数据
    return usart_data_receive(USART0);
}


#define  RX_BUF_LEN  1024
// 数据接收缓冲区
uint8_t  g_rx_buffer[RX_BUF_LEN];
// 收到的字节个数
uint32_t g_rx_cnt = 0;


/*******
中断函数: 收到任意标记信号, 立即执行
1. 触发中断的原因(标记)很多
2. 需要区分是哪个标记触发的中断函数
3. 清理中断标记, 避免重复多次触发

recv[4]->%s\n
********/
void USART0_IRQHandler(void){
    // 需要区分是哪个标记触发的中断函数
    if(SET == usart_interrupt_flag_get(USART0, USART_INT_FLAG_RBNE)){         
//        printf("RBNE\n"); 收到N个字节就会执行N次
        // 清理中断标记, 避免重复多次触发
        usart_interrupt_flag_clear(USART0, USART_INT_FLAG_RBNE);
        
        // 读取寄存器里的数据
        uint8_t data = usart_data_receive(USART0);
        
        // 缓存到buffer里
        g_rx_buffer[g_rx_cnt++] = data;
        
        // 避免缓冲溢出
        if(g_rx_cnt >= RX_BUF_LEN) g_rx_cnt = 0;
//        USART0_send_data(data); // 把每个字节原样返回      
        
    }
    
    // 需要区分是哪个标记触发的中断函数
    if(SET == usart_interrupt_flag_get(USART0, USART_INT_FLAG_IDLE)){  
//        printf("IDLE\n"); // 只会在接收空闲时执行(超过一个字节帧没数据)
        // 不能通过clear函数清理标记, 需要读取一次数据(数据不可用), 则会自动清理标记
        usart_data_receive(USART0);
        
        // 添加字符串结束标记
        g_rx_buffer[g_rx_cnt] = '\0';
        
//        printf("recv[%d]->%s\n", g_rx_cnt, g_rx_buffer);
        USART0_on_recv(g_rx_buffer, g_rx_cnt);
        
        // 把缓冲区收的数据清理为0x00 (可选)            
//        memset(g_rx_buffer, 0x00, g_rx_cnt);
        g_rx_cnt = 0;
    }
}





