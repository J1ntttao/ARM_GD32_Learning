#include "gd32f4xx.h"
#include "systick.h"
#include <stdio.h>
#include "main.h"
#include "USART0.h"

/************************
任务目标: 使用通用定时器

TIMER3CH0   PD12: LED5
TIMER3CH1   PD13: LED6
TIMER3CH2   PD14: LED7
TIMER3CH3   PD15: LED8

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

void timer_gpio_config(rcu_periph_enum rcu, 
    uint32_t port, uint32_t pin, uint32_t alt_func_num){
    
    rcu_periph_clock_enable(rcu);
    // GPIO模式 AF
    gpio_mode_set(port, GPIO_MODE_AF, GPIO_PUPD_NONE, pin);
    // GPIO输出选项
    gpio_output_options_set(port, GPIO_OTYPE_PP, GPIO_OSPEED_MAX, pin);
    // GPIO引脚 AF 复用
    gpio_af_set(port, alt_func_num, pin);
    
}
    
void timer_init_config(uint32_t timer_periph, uint16_t prescaler, uint32_t period){
    // 重置Timer
    timer_deinit(timer_periph);
    // 初始化结构体参数
    timer_parameter_struct initpara;
    /* 初始化为默认参数 initialize TIMER init parameter struct */
    timer_struct_para_init(&initpara);
    // 配置分频系数 Max: 65535
    initpara.prescaler         = prescaler - 1U;    
    // 配置一个周期的计数值
    initpara.period            = period - 1U;
    /* 初始化TIMER定时器 initialize TIMER counter */
    timer_init(timer_periph, &initpara);
    // 启用Timer
    timer_enable(timer_periph);
}

void timer_channel_config(uint32_t timer_periph, uint32_t channel, uint8_t enable){
     // 初始化输出通道-------------------------------------------------
    timer_oc_parameter_struct ocpara;
    /* 初始化默认参数 initialize TIMER channel output parameter struct */
    timer_channel_output_struct_para_init(&ocpara);
    /* 启用TIMER0_CH0的OP极(正极)  Positive*/
    ocpara.outputstate  = enable ? TIMER_CCX_ENABLE : TIMER_CCX_DISABLE;
    /* 启用TIMER0_CH0的ON极(负极)  Negative*/
    ocpara.outputnstate = enable ? TIMER_CCXN_ENABLE : TIMER_CCXN_DISABLE;
    /* 配置TIMER0_CH0的OP极(正极)极性 */    
    ocpara.ocpolarity   = TIMER_OC_POLARITY_HIGH;
    /* 配置TIMER0_CH0的ON极(负极)极性 */    
    ocpara.ocnpolarity  = TIMER_OCN_POLARITY_HIGH;
    
    timer_channel_output_config(timer_periph, channel, &ocpara);

    /* 配置通道输出的比较模式 configure TIMER channel output compare mode */
    timer_channel_output_mode_config(timer_periph, channel, TIMER_OC_MODE_PWM0);
}
 
#define TIMER_PRESCALER     10U 
#define TIMER_FREQ          1000U
#define TIMER_PERIOD        (SystemCoreClock / TIMER_PRESCALER / TIMER_FREQ)

#define LED1     TIMER0, TIMER_CH_0
#define LED2     TIMER0, TIMER_CH_0

void Timer_config(){
    // GPIO --------------------------------
    timer_gpio_config(RCU_GPIOE, GPIOE, GPIO_PIN_8, GPIO_AF_1);
    timer_gpio_config(RCU_GPIOE, GPIOE, GPIO_PIN_9, GPIO_AF_1);
    
    // TIMER -------------------------------
    rcu_periph_clock_enable(RCU_TIMER0);
    // 升级TIMER频率 x2=84MHz  x4=168MHz
    rcu_timer_clock_prescaler_config(RCU_TIMER_PSC_MUL4);
    // 初始化Timer
    timer_init_config(TIMER0, TIMER_PRESCALER, TIMER_PERIOD);
    
    // TIMER Channel -----------------------
    timer_channel_config(LED1, ENABLE);
    
    // 针对Timer0和Timer7高级定时器, 必须配置break保护电路, 才能输出
    /* TIMER通道互补保护电路 */
    timer_break_parameter_struct breakpara;
    /* 初始化TIMER break参数结构体 */
    timer_break_struct_para_init(&breakpara);
    /* break输入的极性 HIGH */
    breakpara.breakpolarity   = TIMER_BREAK_POLARITY_HIGH;
    /* 输出自动的启用 */
    breakpara.outputautostate = TIMER_OUTAUTO_ENABLE;
    /* break输入的启用*/
    breakpara.breakstate      = TIMER_BREAK_ENABLE;
    /* 死区时间 */    
    breakpara.deadtime        = 0U;
    /* 配置TIMER0 break */
    timer_break_config(TIMER0, &breakpara);
    /* 启用TIMER0 break */
    timer_break_enable(TIMER0);
}

void PWM_update(uint32_t timer_periph, uint32_t channel, float duty){ // [0, 100]

    if(duty > 100) duty = 100;
    else if(duty < 0) duty = 0; 
    
    uint32_t pulse = (TIMER_PERIOD - 1) * duty / 100.0f;
    
    // 配置通道输出的脉冲计数值(占空比)configure TIMER channel output pulse value
    timer_channel_output_pulse_value_config(timer_periph, channel, pulse);
}

int main(void) {
    // 配置全局中断优先级分组
    nvic_priority_group_set(NVIC_PRIGROUP_PRE2_SUB2);
    // 滴答定时器初始化
    systick_config();
    // 初始化串口
    USART0_init();    
    // 初始化并拉低总开关
    GPIO_config(RCU_GPIOC, GPIOC, GPIO_PIN_6);    
    // 初始化Timer及通道
    Timer_config();
    
    PWM_update(LED1, 60.0f);
    
    // 0 -> 100 -> 0 -> 100
    float duty = 0;
    int8_t dir = 1;
    while(1) {
        if(duty >= 100) dir = -1;
        else if(duty <= 0) dir = 1;        
        duty += dir;
        
//        PWM_update(LED1, duty);
     
        delay_1ms(10);        
    }
}
