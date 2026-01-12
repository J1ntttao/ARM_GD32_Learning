#ifndef __USART0_H__
#define __USART0_H__

#include "gd32f4xx.h"

void USART0_init();

// 发送1个字节数据
void USART0_send_data(uint8_t data);

// 发送多个字节(字节数字)
void USART0_send_data_array(uint8_t* arr, uint32_t len);

// 发送字符串 '\0'
void USART0_send_string(char* str);

extern void USART0_on_recv(uint8_t* data, uint32_t len);

#endif