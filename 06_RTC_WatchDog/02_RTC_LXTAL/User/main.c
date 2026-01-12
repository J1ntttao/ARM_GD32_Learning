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

// 十位取出左移4位 + 个位 (得到BCD数)
#define DEC_2_BCD(val) 	((val / 10) << 4) + (val % 10)
// 将高4位乘以10 + 低四位 (得到10进制数)
#define BCD_2_DEC(val) 	(val >> 4) * 10 + (val & 0x0F)

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
    
	/* 重置备份域（不重置可能无法设置晶振，RTC时钟不走字）*/
	rcu_bkp_reset_enable();
	rcu_bkp_reset_disable();
    
    //  2. 设置RTC日期时间 crystal oscillator
#if 0       
    rcu_osci_on(RCU_IRC32K); // 低速内部晶振 IRC32K
    /* 等待晶振稳定 */
    rcu_osci_stab_wait(RCU_IRC32K);
    /* 配置RTC时钟源 32K内部低速晶振 configure the RTC clock source selection*/
    rcu_rtc_clock_config(RCU_RTCSRC_IRC32K);
#endif
    
    rcu_osci_on(RCU_LXTAL); // 低速外部晶振 LXTAL 32.768KHz
    /* 等待晶振稳定 */
    rcu_osci_stab_wait(RCU_LXTAL);
    /* 配置RTC时钟源 configure the RTC clock source selection*/
    rcu_rtc_clock_config(RCU_RTCSRC_LXTAL);
    
    //  3. 开启RTC时钟
    rcu_periph_clock_enable(RCU_RTC);
    // 等时间日期的寄存器和总线同步
    rtc_register_sync_wait();    
}

void RTC_read(){
    rtc_parameter_struct rps;
    /* get current time and date */
    rtc_current_time_get(&rps); // 0x25 -> 25
    uint16_t year       = BCD_2_DEC(rps.year) + 2000;
    uint8_t month       = BCD_2_DEC(rps.month);
    uint8_t date        = BCD_2_DEC(rps.date);
    uint8_t hour        = BCD_2_DEC(rps.hour);
    uint8_t minute      = BCD_2_DEC(rps.minute);
    uint8_t second      = BCD_2_DEC(rps.second);
    uint8_t day_of_week = BCD_2_DEC(rps.day_of_week);
        
    printf("-> %04d-%02d-%02d ", year, month, date);
    printf("%02d:%02d:%02d w->%d\n", hour, minute, second, day_of_week);

//    printf("%02X-%02X-%02X ", rps.year, rps.month, rps.date);
//    printf("%02X:%02X:%02X w->%d\n", rps.hour, rps.minute, rps.second, rps.day_of_week);

}

void RTC_write(){
    // config之后, 不要调用rtc_deinit, 否则不走字

    rtc_parameter_struct rps; // 数据都是BCD格式
    rps.year         = DEC_2_BCD(25);
    rps.month        = DEC_2_BCD(12);
    rps.date         = DEC_2_BCD(22);
    rps.day_of_week  = DEC_2_BCD(01);      // [1, 7]
    rps.hour         = DEC_2_BCD(23);
    rps.minute       = DEC_2_BCD(59);
    rps.second       = DEC_2_BCD(55);
    rps.am_pm        = RTC_PM;
    rps.display_format = RTC_24HOUR;
    // 配置两个分频系数(决定时钟走字速度)
    rps.factor_asyn = 0x7F; // 7bit 异步分频器[0x00, 0x7F]
    rps.factor_syn  = 255; //15bit 异步分频器[0x0000, 0x7FFF]
    // ck_spre = 1Hz  每秒1Hz
    // ck_spre = rtc_clk / ((factor_a + 1) * (factor_s + 1))
    
    // IRC32K 内部低速32K晶振 -------------------------------------
    // ck_spre = 1, rtc_clk = 32K, factor_a = 127
    // 1 = 32K / ((0x7F + 1) * (factor_s + 1))
    // factor_s + 1 = 32K / 128
    // factor_s = 32K / 128 - 1
    // factor_s = 249 (0xF9)
    
    // LXTAL 外部低速32.768K晶振 -------------------------------------
    // ck_spre = 1, rtc_clk = 32768, factor_a = 127
    // 1 = 32768 / ((0x7F + 1) * (factor_s + 1))
    // factor_s + 1 = 32768 / 128
    // factor_s = 32768 / 128 - 1
    // factor_s = 255 (0xFF)
    
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
