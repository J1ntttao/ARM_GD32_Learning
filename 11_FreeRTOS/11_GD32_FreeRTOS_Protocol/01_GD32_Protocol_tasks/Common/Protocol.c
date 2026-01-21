#include "Protocol.h"
#include "utils.h"
#include <string.h>

// weak 回调函数
__attribute__((weak)) void Protocol_callback(uint8_t cmd, uint8_t* data, uint32_t data_len){
    printf("未实现的回调函数: cmd: %02X data_len: %d\n", cmd, data_len);
}

// 数据解析缓冲区
static uint8_t  g_read_buf[DATA_PACKAGE_LEN_MAX];
// 已经缓存的字节个数
static uint32_t g_recv_cnt = 0;
// 要接收的数据长度(不包含帧头,命令位,校验码和帧尾)
static uint32_t g_recv_data_cnt = 0;

#define BUF_RESET() do{ g_recv_cnt = 0; g_recv_data_cnt = 0;}while(0)

void Protocol_parse(uint8_t data){
    // 缓存数据
    g_read_buf[g_recv_cnt++] = data;

    switch (g_recv_cnt) {
        case 1:
            if(data != FRAME_HEAD_1) {
                BUF_RESET();
                return;
            }
            break;
        case 2:
            if(data != FRAME_HEAD_2) {
                BUF_RESET();
                return;
            }
            break;
        case 4:
            // 记录要接收的数据长度
            g_recv_data_cnt = data;
            // 检查是否在合理范围
            if(g_recv_data_cnt + DATA_PACKAGE_LEN_MIN > DATA_PACKAGE_LEN_MAX) {
                BUF_RESET();
                return;
            }
            break;
        default:         
            break;
    }

    // 校验数据个数
    int package_len = g_recv_data_cnt + DATA_PACKAGE_LEN_MIN;
    if(g_recv_cnt < package_len) return;
    
//    print_bytes("Recv", g_read_buf, package_len);

    // 校验帧尾
    if(g_read_buf[package_len - 1] != FRAME_TAIL) BUF_RESET();
    // 计算校验码
    // 获取期望的校验码
    uint8_t except_checksum = g_read_buf[package_len - 2];
    // 计算真实的检验码
    uint8_t actual_checksum = check_sum(&g_read_buf[2], 2 + g_recv_data_cnt);

    // 校验校验码
    if(except_checksum != actual_checksum){
        printf("校验失败except: %02X actual: %02X\n", except_checksum, actual_checksum);
        BUF_RESET();
        return;
    }
    print_bytes("Passed", g_read_buf, package_len);

    // 执行回调
    // 第一个字节是命令位, 后面是数据长度和数据位
    Protocol_callback(g_read_buf[2], &g_read_buf[4], g_recv_data_cnt);

    // 清理缓冲区
    BUF_RESET();
}
/*****************
    0  1      2        3        4   5      6      7 
	帧头   命令位  数据长度     数据位   校验位   帧尾 
	AA AA    F0       02       00  63     55     BB

target-> AA AA F0 02 00 63 55 BB
kp ->    AA AA E0 05 00 3F 0C CC CD C9 BB
ki ->    AA AA E0 05 01 3C 34 39 58 E7 BB
kd ->    AA AA E0 05 02 3F 8C CC CD 4B BB
*****************/
void Protocol_send(uint8_t cmd, uint8_t* payload, uint16_t payload_len,
        on_protocol_send  send_cb ){
            
    // 动态申请堆内存
//    uint8_t* data = malloc(DATA_PACKAGE_LEN_MIN + payload_len);

    uint8_t data[DATA_PACKAGE_LEN_MIN + payload_len];
    data[0] = FRAME_HEAD_1; // 帧头1
    data[1] = FRAME_HEAD_2; // 帧头2
    data[2] = cmd;          // 命令位
    data[3] = payload_len;  // 数据长度
    // 把payload数据拷贝到data[4]开始的地方
    memcpy(data + 4, payload, payload_len); // 4, 5
    
    data[4 + payload_len] = check_sum(data + 2, 2 + payload_len); // 计算校验和
    data[5 + payload_len] = FRAME_TAIL;
    
    // 执行回调函数
    send_cb(data, DATA_PACKAGE_LEN_MIN + payload_len);
            
    // 释放内存
//    free(data);
}

