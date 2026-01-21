#include "task_timer.h"
// TIMER 初始化
static void timer_init_config(rcu_periph_enum rcu, 
    uint32_t timer_periph, uint16_t prescaler, uint32_t period){    
    rcu_periph_clock_enable(rcu);
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

#define TIMER_PRESCALER  10U
#define TIMER_FREQ       1000U
    
void task_timer_init(){
    // 升级所有TIMER频率 x2=84MHz  x4=168MHz
    rcu_timer_clock_prescaler_config(RCU_TIMER_PSC_MUL4);
        
    // 初始化Timer5 --------------------------------------------------
    // 按照1000Hz的频率调用中断函数    
    uint32_t period = SystemCoreClock / TIMER_PRESCALER / TIMER_FREQ;    
    timer_init_config(RCU_TIMER5, TIMER5, TIMER_PRESCALER, period);
    
    // 初始化中断 NVIC --------------------------------------------
    // 配置中断的优先级
    nvic_irq_enable(TIMER5_DAC_IRQn, 2, 2);
    // 启用Timer中断
    timer_interrupt_enable(TIMER5, TIMER_INT_UP);

}

#include "tasks.h"

void TIMER5_DAC_IRQHandler(void){

    if(SET == timer_interrupt_flag_get(TIMER5, TIMER_INT_FLAG_UP)){
        // 判断并清理标记
        timer_interrupt_flag_clear(TIMER5, TIMER_INT_FLAG_UP);
        
        Task_switch_handler();
    }
    
}