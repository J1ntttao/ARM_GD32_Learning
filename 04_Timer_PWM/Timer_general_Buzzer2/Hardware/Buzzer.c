#include "Buzzer.h"

//			 C	 D     E 	F	 G	  A	   B	 C`
// u16 hz[] = {523, 587, 659, 698, 784, 880, 988, 1047};
//			 C`	   D`     E`   F`	  G`	A`	  B`    C``
//u16 hz[] = {1047, 1175, 1319, 1397, 1568, 1760, 1976, 2093};
u16 FREQS[] = {
	523 * 1, 587 * 1, 659 * 1, 698 * 1, 784 * 1, 880 * 1, 988 * 1, 
	523 * 2, 587 * 2, 659 * 2, 698 * 2, 784 * 2, 880 * 2, 988 * 2, 
	523 * 4, 587 * 4, 659 * 4, 698 * 4, 784 * 4, 880 * 4, 988 * 4, 
	523 * 8, 587 * 8, 659 * 8, 698 * 8, 784 * 8, 880 * 8, 988 * 8, 
};
// TIMER1_CH1

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
    
#define TIMER_RCU           RCU_TIMER1
#define TIMER_PERIPH        TIMER1
#define TIMER_CH            TIMER_CH_1

#define TIMER_PRESCALER     10U 
//#define TIMER_FREQ          1000U
//#define TIMER_PERIOD        (SystemCoreClock / TIMER_PRESCALER / TIMER_FREQ)

void timer_init_config(uint32_t timer_periph, uint16_t prescaler, uint32_t period){
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
    /* 启用TIMER3_CH2的OP极(正极) */
    ocpara.outputstate  = enable ? TIMER_CCX_ENABLE : TIMER_CCX_DISABLE;
    timer_channel_output_config(timer_periph, channel, &ocpara);

    /* 配置通道输出的比较模式 configure TIMER channel output compare mode */
    timer_channel_output_mode_config(timer_periph, channel, TIMER_OC_MODE_PWM0);
}

// 初始化PB9 TIMER1_CH1
void Buzzer_init(){    
    // 初始化GPIO -------------------------------------------------
    timer_gpio_config(RCU_GPIOB, GPIOB, GPIO_PIN_9, GPIO_AF_1);
    
    // 初始化定时器Timer ------------------------------------------
    rcu_periph_clock_enable(TIMER_RCU);
    // 升级TIMER频率 42MHz x2=84MHz  x4=168MHz
    rcu_timer_clock_prescaler_config(RCU_TIMER_PSC_MUL4);
    // 重置Timer
    timer_deinit(TIMER_PERIPH);
    // 初始化timer
//    timer_init_config(TIMER_PERIPH, TIMER_PRESCALER, TIMER_PERIOD);
    // 初始化输出通道-------------------------------------------------    
    timer_channel_config(TIMER_PERIPH, TIMER_CH, ENABLE);
}

void Buzzer_play(u16 hz_val) {
    // 根据系统主频, 预分频系数, 计算周期计数值
    uint32_t period = SystemCoreClock / TIMER_PRESCALER / hz_val;
    
    // 配置Timer分频系数和周期
    timer_init_config(TIMER_PERIPH, TIMER_PRESCALER, period);
    // 启用Timer的通道       
    timer_channel_config(TIMER_PERIPH, TIMER_CH, ENABLE);
    // 配置通道输出的脉冲计数值(占空比)configure TIMER channel output pulse value
    timer_channel_output_pulse_value_config(TIMER_PERIPH, TIMER_CH, period * 0.6f);
}

void Buzzer_beep(u16 idx){
    if(idx == 0){ // 不发音
        Buzzer_stop();
        return;
    }
    
    Buzzer_play(FREQS[idx - 1]);
}

void Buzzer_stop(){
    // 禁用TIMER的通道
    timer_channel_config(TIMER_PERIPH, TIMER_CH, DISABLE);
}
