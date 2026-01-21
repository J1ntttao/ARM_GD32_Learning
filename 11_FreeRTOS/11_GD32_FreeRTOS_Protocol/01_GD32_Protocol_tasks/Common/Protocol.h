#ifndef __PROTOCOL_H__
#define __PROTOCOL_H__

#include "gd32f4xx.h"

#define FRAME_HEAD_1    0xAA
#define FRAME_HEAD_2    0xAA
#define FRAME_TAIL      0xBB

#define DATA_PACKAGE_LEN_MIN    6
#define DATA_PACKAGE_LEN_MAX    20

typedef void (*on_protocol_send)(uint8_t* data_arr, uint32_t data_len);

void Protocol_parse(uint8_t data);

void Protocol_send(uint8_t cmd, uint8_t* payload, uint16_t payload_len, on_protocol_send  send_cb );

#endif