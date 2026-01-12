#include "gd32f4xx.h"
#include "systick.h"
#include <stdio.h>
#include "main.h"
#include "USART0.h"
#include <string.h>

/************************
任务目标: 初始化并配置RTC时钟, 循环读取时钟

- 设置时钟
    1. 解除备份域寄存器的写保护
    2. 设置RTC日期时间
    3. 开启RTC时钟
    4. 设置时期时间, 分频系数

- 循环读取时钟

*************************/


void USART0_on_recv(uint8_t* data, uint32_t len){
    // 此代码是在中断里执行的, 不要做耗时操作 (delay_ms)
    printf("recv[%d]->%s\n", len, data);
}

void RTC_config(){    
    //  1. 解除备份域寄存器的写保护
    /* 启用PMU电源管理模块时钟 */
    rcu_periph_clock_enable(RCU_PMU);    
    /* 启用备份域写数据功能 enable write access to the registers in backup domain */
    pmu_backup_write_enable();
    
    //  2. 设置RTC日期时间 crystal oscillator
    rcu_osci_on(RCU_IRC32K);
    /* 等待晶振稳定 */
    rcu_osci_stab_wait(RCU_IRC32K);
    /* 配置RTC时钟源 32K内部低速晶振 configure the RTC clock source selection*/
    rcu_rtc_clock_config(RCU_RTCSRC_IRC32K);
    
    //  3. 开启RTC时钟
    rcu_periph_clock_enable(RCU_RTC);
    // 等时间日期的寄存器和总线同步
    rtc_register_sync_wait();    
}

void RTC_read(){
    rtc_parameter_struct rps;
    /* get current time and date */
    rtc_current_time_get(&rps);
    
    printf("%02X-%02X-%02X ", rps.year, rps.month, rps.date);
    printf("%02X:%02X:%02X w->%d\n", rps.hour, rps.minute, rps.second, rps.day_of_week);

}

void RTC_write(){
    // config之后, 不要调用rtc_deinit, 否则不走字

    rtc_parameter_struct rps; // 数据都是BCD格式
    rps.year         = 0x25;
    rps.month        = 0x12;
    rps.date         = 0x22;
    rps.day_of_week  = 0x01;      // [1, 7]
    rps.hour         = 0x23;
    rps.minute       = 0x59;
    rps.second       = 0x55;
    rps.am_pm        = RTC_PM;
    rps.display_format = RTC_24HOUR;
    // 配置两个分频系数(决定时钟走字速度)
    rps.factor_asyn = 0x7F; // 7bit 异步分频器[0x00, 0x7F]
    rps.factor_syn  = 0xF9; //15bit 异步分频器[0x0000, 0x7FFF]
    // ck_spre = 1Hz  每秒1Hz
    // ck_spre = rtc_clk / ((factor_a + 1) * (factor_s + 1))
    
    // IRC32K 内部低速32K晶振 -------------------------------------
    // ck_spre = 1, rtc_clk = 32K, factor_a = 127
    // 1 = 32K / ((0x7F + 1) * (factor_s + 1))
    // factor_s + 1 = 32K / 128
    // factor_s = 32K / 128 - 1
    // factor_s = 249 (0xF9)
    
    /* 初始化寄存器 initialize RTC registers */
    rtc_init(&rps);
}

int main(void) {
    // 配置全局中断优先级分组
    nvic_priority_group_set(NVIC_PRIGROUP_PRE2_SUB2);
    // 滴答定时器初始化
    systick_config();
    // 初始化串口
    USART0_init();
    
    RTC_config();    
    
    // 设置日期时间
    RTC_write();
    
    while(1) { 
        // 每隔1s读取一次日期时间
        RTC_read();
        
        delay_1ms(1000);
    }
}
