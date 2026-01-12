#include "gd32f4xx.h"
#include "systick.h"
#include <stdio.h>
#include "main.h"
#include "USART0.h"
#include <string.h>

/**********************************************************************************
	*		任务目标: 内存到内存数据拷贝, 使用DMA完成Memory To Memory的动态拷贝内存数据
  *
	*			1. CPU配置DMA拷贝信息
	*			2. CPU通知开始DMA开始干活
	*			3. DMA请求获取源数据src
	*			4. DMA将获取到的数据交给目标dst
	*	
 **********************************************************************************/

void USART0_on_recv(uint8_t* data, uint32_t len){
    // 此代码是在中断里执行的, 不要做耗时操作 (delay_ms)
		printf("recv[%d]: %s\n", len, data);
}

int main(void) {
    // 配置全局中断优先级分组
    nvic_priority_group_set(NVIC_PRIGROUP_PRE2_SUB2);
    // 滴答定时器初始化
    systick_config();
    // 初始化串口
    USART0_init();
    
    uint64_t start = get_sys_tick();
    
    // 单个字节
    USART0_send_data('S');
    
    // 字节数组
    uint8_t arr[] = { 'i', 'n', 'g', 'l', 'e', '\n'};
    USART0_send_data_array(arr, 6);
    
    // 字符串
    USART0_send_string("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz\n");
    
    // 打印耗时: 约5000us -> 5ms (DMA可以通过中断的形式通知完毕, 避免阻塞)
    printf("duration: %f ms\n", (double)(get_sys_tick() - start)/1000);
      
    printf("USART0_data Address: 0x%X\n", ((uint32_t)&USART_DATA(USART0)));
        
    while(1) { 
        
    }
}
