#include "gd32f4xx.h"
#include "systick.h"
#include <stdio.h>
#include <string.h>
#include "main.h"
#include "USART0.h"
#include "EXTI.h"
#include "SPI0.h"

/************************

测试不同的睡眠模式

- 睡眠模式
- 深度睡眠模式
- 待机模式

*************************/

uint8_t flag = 0x00;
void USART0_on_recv(uint8_t* data, uint32_t len) {
    // 此代码是在中断里执行的, 不要做耗时操作 (delay_ms)    
    flag = data[0];
}

void EXTI1_on_trig(){
    printf("PC1 key down\n");
}

void sleep_mode(){ // 睡眠模式    
    // RCU
    rcu_periph_clock_enable(RCU_PMU);
    
    printf("睡眠模式 sleep_mode_1\n");
    
    pmu_to_sleepmode(WFI_CMD);
        
    printf("睡眠模式 sleep_mode_2\n");        
}

void deep_sleep_mode(){ // 深度睡眠模式
    // RCU
    rcu_periph_clock_enable(RCU_PMU);
    
    printf("深度睡眠模式 deep_sleep_mode 1\n");
    
    pmu_to_deepsleepmode(PMU_LDO_LOWPOWER, PMU_LOWDRIVER_ENABLE, WFI_CMD);
    
    /* 把主频设置回来 */
    SystemInit();
    
    printf("深度睡眠模式 deep_sleep_mode 2\n");
}

void standy_mode(){ // 待机模式
    // RCU
    rcu_periph_clock_enable(RCU_PMU);
    printf("待机模式 standy_mode_1\n");
    
    /* 清理待机模式标记 */
    pmu_flag_clear(PMU_FLAG_RESET_STANDBY);
    
    /* 启用唤醒按钮PA0 enable PMU wakeup pin */
    pmu_wakeup_pin_enable();
    
    /* 进入待机模式 PMU work in standby mode */
    pmu_to_standbymode();
    
    printf("待机模式 standy_mode_2\n");
}

static void GPIO_config(rcu_periph_enum rcu, uint32_t port, uint32_t pin) {
  // 1. 时钟初始化
  rcu_periph_clock_enable(rcu);
  // 2. 配置GPIO 输入输出模式
  gpio_mode_set(port, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, pin);
  // 3. 配置GPIO 输出选项
  gpio_output_options_set(port, GPIO_OTYPE_PP, GPIO_OSPEED_MAX, pin);
  // 4. 默认输出电平
  gpio_bit_write(port, pin, RESET);
}

void delay(){

    uint32_t i = 10000000;
    while(i--){
        __NOP();
    }
}

int main(void) {

    // 配置全局中断优先级分组
    nvic_priority_group_set(NVIC_PRIGROUP_PRE2_SUB2);
    // 滴答定时器初始化
//    systick_config();
    // 初始化串口
    USART0_init();
    // 外部中断
    EXTI_init();
    
    GPIO_config(RCU_GPIOB, GPIOB, GPIO_PIN_2);
    
    
    printf("Init Complete\n" );

    while(1) {
        // 让PB2切换亮灭
        gpio_bit_toggle(GPIOB, GPIO_PIN_2);
        
        // 使用systick睡眠(借用中断计时)
//        delay_1ms(500);
        delay();
        
        if(flag > 0){
            switch(flag){
                case 0x01: sleep_mode(); break;
                case 0x02: deep_sleep_mode(); break;
                case 0x03: standy_mode(); break;
                default: break;
            }
        
            flag = 0;
        }
        
    }
}
