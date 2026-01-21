#ifndef __PROTOCOL_H__
#define __PROTOCOL_H__

#include "gd32f4xx.h"

#define FRAME_HEAD_1    0xAA
#define FRAME_HEAD_2    0xAA
#define FRAME_TAIL      0xBB

#define DATA_PACKAGE_LEN_MIN    6
#define DATA_PACKAGE_LEN_MAX    20


void Protocol_parse(uint8_t data);

#endif