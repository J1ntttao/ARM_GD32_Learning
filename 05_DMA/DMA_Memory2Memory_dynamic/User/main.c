#include "gd32f4xx.h"
#include "systick.h"
#include <stdio.h>
#include "main.h"
#include "USART0.h"
#include <string.h>

/************************
任务目标: 内存到内存数据拷贝, 使用DMA完成Memory To Memory的动态拷贝内存数据

1. CPU配置DMA拷贝信息
2. CPU通知开始DMA开始干活
3. DMA请求获取源数据src
4. DMA将获取到的数据交给目标dst

*************************/

#define ARR_LEN 1024

// 数据源 source
uint8_t src[ARR_LEN] = {8, 7, 6, 5, 4, 3, 2, 1}; // 
char* str = "itheima";
// 数据目标 destination
uint8_t dst[ARR_LEN] = {0};

#define DMA_PERIPH_CH       DMA1, DMA_CH0

void USART0_on_recv(uint8_t* data, uint32_t len){
    // 此代码是在中断里执行的, 不要做耗时操作 (delay_ms)
    printf("recv[%d]->%s\n", len, data);
    
    /* 配置数据源地址 set DMA peripheral base address */
    dma_periph_address_config(DMA_PERIPH_CH, (uint32_t)data);
    /* 配置要拷贝的数据个数(+1是为了把\0拷贝到dst) set the number of remaining data to be transferred by the DMA */
    dma_transfer_number_config(DMA_PERIPH_CH, len + 1); // \0
    // 2. CPU通知开始DMA开始干活(非阻塞) enable DMA channel
    dma_channel_enable(DMA_PERIPH_CH);
}


void DMA_config(){
    // M2M, 内存到内存拷贝, 方向src -> dst
    // DMA初始化 --------------------------------
    rcu_periph_clock_enable(RCU_DMA1); // m2m只能使用DMA1
    
    /* 重置DMA1 deinitialize DMA a channel registers */
    dma_deinit(DMA_PERIPH_CH); // 内存到内存, 可随意指定通道
    
    dma_single_data_parameter_struct init_struct;
    /* 初始化默认参数 initialize the DMA single data mode parameters struct with the default values */
    dma_single_data_para_struct_init(&init_struct);
    /* 配置拷贝方向: 内存到内存 */
    init_struct.direction           = DMA_MEMORY_TO_MEMORY;
    
    /* 配置数据源地址: 如果拷贝方向是m2m, 数据源借用periph_addr */
//    init_struct.periph_addr         = (uint32_t)src;
    init_struct.periph_inc          = DMA_PERIPH_INCREASE_ENABLE; // 自增increase
    
    /* 配置目标地址 */
    init_struct.memory0_addr        = (uint32_t)dst;
    init_struct.memory_inc          = DMA_MEMORY_INCREASE_ENABLE;
    
    /* 配置数据的宽度和总个数 */
    init_struct.periph_memory_width = DMA_PERIPH_WIDTH_8BIT;
//    init_struct.number              = sizeof(src) / sizeof(src[0]);
    
    /* 循环模式(关闭) */
    init_struct.circular_mode       = DMA_CIRCULAR_MODE_DISABLE;
    init_struct.priority            = DMA_PRIORITY_LOW; // 优先级

    /* 执行DMA的初始化 DMA single data mode initialize */
    dma_single_data_mode_init(DMA_PERIPH_CH, &init_struct);

    
    // 中断初始化 --------------------------------
    // 开启传输完成中断(可选)
    nvic_irq_enable(DMA1_Channel0_IRQn, 2, 2);
    // 启用中断标记 enable DMA interrupt
    dma_interrupt_enable(DMA_PERIPH_CH, DMA_INT_FTF);
}

uint8_t complete_flag = 0;

// 定义DMA中断函数: DMA1 CH0
void DMA1_Channel0_IRQHandler(){
    // 判断并清理标记
    if(SET == dma_interrupt_flag_get(DMA_PERIPH_CH, DMA_INT_FLAG_FTF)){
        // 清理标记
        dma_interrupt_flag_clear(DMA_PERIPH_CH, DMA_INT_FLAG_FTF);
                    
        complete_flag = 1;
        printf("INT. dst: %d %d %d %d %s\n", dst[0], dst[1], dst[2], __LINE__, __FUNCTION__);
    }
}

int main(void) {
    // 配置全局中断优先级分组
    nvic_priority_group_set(NVIC_PRIGROUP_PRE2_SUB2);
    // 滴答定时器初始化
    systick_config();
    // 初始化串口
    USART0_init();
    printf("init complete\n");
    
#if 0
    // 不使用DMA, 将src内容拷贝给dst (会产生阻塞)
    // 参数: 目标, 源头, 个数
    memcpy(dst, src, ARR_LEN);    
    printf("dst: %d %d %d\n", dst[0], dst[1], dst[2]);
    printf("dst: %s\n", dst);
#endif
        
    // 1. CPU配置DMA拷贝信息
    DMA_config();
    
    // 2. CPU通知开始DMA开始干活(非阻塞)
//    dma_channel_enable(DMA_PERIPH_CH);

    printf("src_addr: 0x%X\n", (uint32_t)src);
    printf("dst_addr: 0x%X\n", (uint32_t)dst);
    printf("USART0_data: 0x%X\n", (uint32_t)(&USART_DATA(USART0)));
        
    while(1) { 
        if(complete_flag){
            complete_flag = 0;
                            
            printf("while. dst: %s\n", dst);   
        }
        
    }
}
