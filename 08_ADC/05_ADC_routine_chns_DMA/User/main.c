#include "gd32f4xx.h"
#include "systick.h"
#include <stdio.h>
#include <string.h>
#include "main.h"
#include "USART0.h"

/************************
模板代码

开启扫描模式, 配合DMA进行多通道采样 

- IN14 PC4 电位器电压
- IN16 芯片温度

*************************/

void USART0_on_recv(uint8_t* data, uint32_t len) {
    // 此代码是在中断里执行的, 不要做耗时操作 (delay_ms)
    printf("recv[%d]->%s\n", len, data);
}

#define ADC_DMA_CH      DMA1, DMA_CH0

#define ADC_CH_LEN  2U

uint16_t g_adc_buff[ADC_CH_LEN];

void ADC_DMA_config(){
    // DMA开启时钟
    rcu_periph_clock_enable(RCU_DMA1);
    /* 重置 deinitialize DMA a channel registers */
    dma_deinit(ADC_DMA_CH);
    
    dma_single_data_parameter_struct init_struct;    
    /* 设置默认参数DMA initialize the DMA single data mode parameters struct with the default values */
    dma_single_data_para_struct_init(&init_struct);
    // 搬运方向
    init_struct.direction           = DMA_PERIPH_TO_MEMORY;
    // 外设地址: SRC
    init_struct.periph_addr         = (uint32_t)(&ADC_RDATA(ADC0));
    init_struct.periph_inc          = DMA_PERIPH_INCREASE_DISABLE;
    // 内存地址: DST
    init_struct.memory0_addr        = (uint32_t)g_adc_buff;
    init_struct.memory_inc          = DMA_MEMORY_INCREASE_ENABLE;
    // 数据宽度和个数
    init_struct.periph_memory_width = DMA_PERIPH_WIDTH_16BIT;
    init_struct.number              = ADC_CH_LEN;
    
    // 不循环
    init_struct.circular_mode       = DMA_CIRCULAR_MODE_DISABLE;
    init_struct.priority            = DMA_PRIORITY_LOW;

    /* 初始化 DMA single data mode initialize */
    dma_single_data_mode_init(ADC_DMA_CH, &init_struct);
    
    // 配置DMA子集
    dma_channel_subperipheral_select(ADC_DMA_CH, DMA_SUBPERI0);
    
    // 开启循环模式
    dma_circulation_enable(ADC_DMA_CH);
    
    // 启用DMA
    dma_flag_clear(ADC_DMA_CH, DMA_FLAG_FTF); // 清理标记
    dma_channel_enable(ADC_DMA_CH);
}

void ADC_config(void)
{    
    // GPIO 引脚配置-----------------------------------------
    rcu_periph_clock_enable(RCU_GPIOC);
    gpio_mode_set(GPIOC, GPIO_MODE_ANALOG, GPIO_PUPD_NONE, GPIO_PIN_4);
    
    // ADC 通用配置------------------------------------------
    /* 启用ADC时钟 enable ADC clock */
    rcu_periph_clock_enable(RCU_ADC0);
    /* 配置分频系数 config ADC clock  84M / 4 = 21MHz */
    adc_clock_config(ADC_ADCCK_PCLK2_DIV4);
    /* 设置多ADC同步模式: 独立模式, 避免相互干扰 ADC mode config */
    adc_sync_mode_config(ADC_SYNC_MODE_INDEPENDENT);
    
    // ADC0 基本配置------------------------------------------
    /* 配置数据对齐方式 ADC data alignment config */
    adc_data_alignment_config(ADC0, ADC_DATAALIGN_RIGHT);
    /* 配置分辨率 configure ADC resolution */
    adc_resolution_config(ADC0, ADC_RESOLUTION_12B);
    
    /* 连续模式配置 ADC contineous function disable */
    adc_special_function_config(ADC0, ADC_CONTINUOUS_MODE, DISABLE);
    /* 扫描模式配置 ADC scan mode disable */
    adc_special_function_config(ADC0, ADC_SCAN_MODE, ENABLE);

    // ADC0 通道配置------------------------------------------
    /* 配置ADC的通道规则, 通道个数 ADC channel length config 
       常规通道 ADC_ROUTINE_CHANNEL  (规则组) [1,16]
       插入通道 ADC_INSERTED_CHANNEL (注入组) [1,4]
    */
    adc_channel_length_config(ADC0, ADC_ROUTINE_CHANNEL, 2);
    /* 通达每轮的采样循环次数 ADC routine channel config
      总转换时间=采样时间+12 个 CK_ADC周期
      0.375us  = (3 + 12)  * (1Mus / 40M) = 15 / 40 (us)
      1.286us  = (15 + 12) * (1Mus / 21M) = 27 / 21 (us)
    */
    
    adc_routine_channel_config(ADC0, 1, ADC_CHANNEL_16, ADC_SAMPLETIME_144);
    adc_routine_channel_config(ADC0, 0, ADC_CHANNEL_14, ADC_SAMPLETIME_144);
//    adc_routine_channel_config(ADC0, 1, ADC_CHANNEL_17, ADC_SAMPLETIME_15);
//    adc_routine_channel_config(ADC0, 2, ADC_CHANNEL_18, ADC_SAMPLETIME_15);

    /* ADC触发源配置 ADC trigger config */
//    adc_external_trigger_source_config(ADC0, ADC_ROUTINE_CHANNEL, ADC_EXTTRIG_ROUTINE_T0_CH0); 
//    adc_external_trigger_config(ADC0, ADC_ROUTINE_CHANNEL, EXTERNAL_TRIGGER_DISABLE);
    
    /* ADC Vbat channel enable */
//    adc_channel_16_to_18(ADC_VBAT_CHANNEL_SWITCH,ENABLE);
    /* ADC temperature and Vref enable */
    adc_channel_16_to_18(ADC_TEMP_VREF_CHANNEL_SWITCH,ENABLE);

    /* ADC DMA function enable */
    adc_dma_request_after_last_enable(ADC0);
    adc_dma_mode_enable(ADC0);

    /* 启用ADC0 enable ADC interface */
    adc_enable(ADC0);
    /* 等待稳定 wait for ADC stability */
    delay_1ms(1);
    /* ADC标定及重置 ADC calibration and reset calibration */
    adc_calibration_enable(ADC0);

    /* 软件触发采样 enable ADC software trigger */
//    adc_software_trigger_enable(ADC0, ADC_ROUTINE_CHANNEL);
}


void ADC_get(){
    /* 软件触发采样 enable ADC software trigger */
    adc_software_trigger_enable(ADC0, ADC_ROUTINE_CHANNEL);
    
    // 芯片温度 ---------------------------------------------------
    uint16_t adc_val = g_adc_buff[0];
        // ADC数值[0,4095] -> 电压值(V_temp / 3.3V = adc_val / 4095)
    float V_temp = adc_val * 3.3 / 4095;   
    // 电压值 -> 温度值
    // 温度 (℃) = {(V25 – Vtemperature) / Avg_Slope} + 25
    float T_rst = ((1.45 - V_temp) * 1000 / 4.1f) + 25;
    
    printf("adc_val: %d V_temp: %5.3fV T_rst: %5.3f℃\n",adc_val, V_temp, T_rst);
 
    // 电位器---------------------------------------------------
    adc_val = g_adc_buff[1];
    float Voltage = adc_val * 3.3 / 4095;
    
    printf("adc_val: %d Voltage: %5.3fV\n", adc_val, Voltage);
}


int main(void) {

    // 配置全局中断优先级分组
    nvic_priority_group_set(NVIC_PRIGROUP_PRE2_SUB2);
    // 滴答定时器初始化
    systick_config();
    // 初始化串口
    USART0_init();    
    
    // ADC搬运的DMA
    ADC_DMA_config();
    
    // 初始化ADC
    ADC_config();

	printf("Init complete!\n");
 
    while(1) {
//        ADC_get_voltage();
//        
//        ADC_get_teamperature();
        
        ADC_get();
        
        delay_1ms(500);       
    }
}
