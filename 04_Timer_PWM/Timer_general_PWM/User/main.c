#include "gd32f4xx.h"
#include "systick.h"
#include <stdio.h>
#include "main.h"
#include "USART0.h"

/************************
任务目标: 使用基本定时器

使用逻辑分析仪, 查看PD15引脚的切换时间间隔
10000Hz
1000Hz
*************************/

void USART0_on_recv(uint8_t* data, uint32_t len){
    // 此代码是在中断里执行的, 不要做耗时操作 (delay_ms)
    printf("recv[%d]->%s\n", len, data);

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


#define TIMER_PRESCALER   10U 
#define TIMER_FREQ        1000U
#define TIMER_PERIOD      (SystemCoreClock / TIMER_PRESCALER / TIMER_FREQ)

void Timer_config(){
    // 通用定时器 Timer3
    // 和引脚有关 PD14 -> TIMER3_CH2
    // 初始化GPIO -------------------------------------------------
    rcu_periph_clock_enable(RCU_GPIOD);
    // GPIO模式 AF
    gpio_mode_set(GPIOD, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_14);
    // GPIO输出选项
    gpio_output_options_set(GPIOD, GPIO_OTYPE_PP, GPIO_OSPEED_MAX, GPIO_PIN_14);
    // GPIO引脚 AF 复用
    gpio_af_set(GPIOD, GPIO_AF_2, GPIO_PIN_14);
    
    // 初始化定时器Timer ------------------------------------------
    rcu_periph_clock_enable(RCU_TIMER3);
    // 升级TIMER频率 42MHz x2=84MHz  x4=168MHz
    rcu_timer_clock_prescaler_config(RCU_TIMER_PSC_MUL4);
    
    // 重置Timer
    timer_deinit(TIMER3);
    
    // 初始化结构体参数
    timer_parameter_struct initpara;
    /* 初始化为默认参数 initialize TIMER init parameter struct */
    timer_struct_para_init(&initpara);
    // 根据需要配置参数
    // 配置分频系数 Max: 65535
    initpara.prescaler         = TIMER_PRESCALER - 1U;    
    // 配置一个周期的计数值
    /**************************************
    使用分频系数, 可以实现更低的Timer输出频率
    
    要求: 分母 >= 2564
    当前一个周期的计数值(Max: 65535) 16800 - 1
    ***************************************/
    initpara.period            = TIMER_PERIOD - 1U;
    /* 初始化TIMER定时器 initialize TIMER counter */
    timer_init(TIMER3, &initpara);
    
    // 初始化输出通道-------------------------------------------------
    timer_oc_parameter_struct ocpara;
    /* 初始化默认参数 initialize TIMER channel output parameter struct */
    timer_channel_output_struct_para_init(&ocpara);
    /* 启用TIMER3_CH2的OP极(正极) */
    ocpara.outputstate  = TIMER_CCX_ENABLE;
//    ocpara.outputnstate = TIMER_CCXN_DISABLE;
//    /* 输出的极性 */
//    ocpara.ocpolarity   = TIMER_OC_POLARITY_HIGH;
//    ocpara.ocnpolarity  = TIMER_OCN_POLARITY_HIGH;
    /* configure TIMER channel output function */
    timer_channel_output_config(TIMER3, TIMER_CH_2, &ocpara);

    /* 配置通道输出的比较模式 configure TIMER channel output compare mode */
    timer_channel_output_mode_config(TIMER3, TIMER_CH_2, TIMER_OC_MODE_PWM0);
    /* 配置通道输出的脉冲计数值(占空比)configure TIMER channel output pulse value */
    timer_channel_output_pulse_value_config(TIMER3, TIMER_CH_2, TIMER_PERIOD * 0.6f);

    // 启用Timer
    timer_enable(TIMER3);
}

// 0, 50, 100, 50, 0, 50, 100 ......
void PWM_update(float duty){ // [0.0, 100.0]

    // 将duty限制在[0, 100]
    if(duty > 100) duty = 100;
    else if(duty < 0) duty = 0;
    
    uint32_t pulse = TIMER_PERIOD * duty / 100.0f;
    /* 配置通道输出的脉冲计数值(占空比)configure TIMER channel output pulse value */
    timer_channel_output_pulse_value_config(TIMER3, TIMER_CH_2, pulse);
    
}

int main(void) {
    // 配置全局中断优先级分组
    nvic_priority_group_set(NVIC_PRIGROUP_PRE2_SUB2);
    // 滴答定时器初始化
    systick_config();
    // 初始化串口
    USART0_init();
    
    GPIO_config(RCU_GPIOC, GPIOC, GPIO_PIN_6);
    
    // 初始化Timer
    Timer_config();
    
    uint8_t cnt = 0;
    float duty = 0.0f;
    int8_t dir = 1; // 变化的方向和幅度
    while(1) {
        // 0 -> 100 -> 0 -> 100 .....
        
        if(duty >= 100){
            dir = -1;
        }else if(duty <= 0){
            dir = 1;
        }
        
        duty += dir;
        printf("duty->%.1f\n", duty);
        
        PWM_update(duty);
        
        delay_1ms(10);        
    }
}
