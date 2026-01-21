#include "App.h"

#include "USART0.h"
#include "queue.h"
#include "Protocol.h"

// 企业协议(标准协议)
#define PROTOCOL_enterprise   1

/**********************************************************
 * @brief 发送当前角度信息给上位机 
 * @param 当前舵机角度
 * 0xB0 data0 data1 0x0D 0x0A
 **********************************************************/
void sendCurrentAngle(uint16_t cur_angle){
    
#if !PROTOCOL_enterprise
    // static uint8_t dataCur[5] = {0xB0, 0x00, 0x00, '\r', '\n'}; // 0x0D, 0x0A   
    uint8_t dataCur[5] = {0}; // 0x0D, 0x0A   
    dataCur[0] = 0xB0;
    dataCur[1] = (cur_angle >> 8) & 0xFF;
    dataCur[2] = cur_angle & 0xFF;
    dataCur[3] = '\r';
    dataCur[4] = '\n';
    
    USART0_send_data_array(dataCur, 5);
#else
    //                      0     1     2     3     4     5     6     7
    uint8_t dataCur[8] = {0xAA, 0xAA, 0xB0, 0x02, 0x00, 0x00, 0x00, 0xBB};
    
    // 01 A2
    
    dataCur[4] = (cur_angle >> 8) & 0xFF;
    dataCur[5] = cur_angle & 0xFF;
    dataCur[6] = check_sum(dataCur + 2, 2 + 2);
    
    USART0_send_data_array(dataCur, 8);
#endif
}

// 目标位置
uint16_t targetAngle = 0;

// PID参数
float kp = 0.5f;
float ki = 0.01f;
float kd = 1.0f;

/**
 * @brief 发送当前PID信息给上位机 
 * @param 当前PID值
 * 需要将1个float类型的数据转成4字节数组
 * pid三个参数一次性发送, 需要4字节*3=12字节
 * 0xB1 12个字节 0x0D 0x0A
 */
void sendCurrentPID(){
    // 1. 将kp, ki, kd 分别转成4字节数组
    uint8_t* kp_bytes = (uint8_t*)&kp;
    uint8_t* ki_bytes = (uint8_t*)&ki;
    uint8_t* kd_bytes = (uint8_t*)&kd;
    
#if !PROTOCOL_enterprise
    // 简易协议
    static uint8_t dataPID[15] = {0xB1,     // 0
        0x00, 0x00, 0x00, 0x00,             // 1,2,3,4
        0x00, 0x00, 0x00, 0x00,             // 5,6,7,8
        0x00, 0x00, 0x00, 0x00,             // 9,10,11,12
        '\r', '\n'}; // 0x0D, 0x0A          // 13, 14

    // 2. 按照小端模式(小字节在低位) 填充dataPID
    memcpy(dataPID + 1, kp_bytes, 4); 
    memcpy(dataPID + 5, ki_bytes, 4);
    memcpy(dataPID + 9, kd_bytes, 4);
    
    // 3. 发送dataPID
    USART0_send_data_array(dataPID, 15);
    
#else
    // 企业标准协议
    static uint8_t dataPID[18] = {0};
    dataPID[0] = 0xAA;
    dataPID[1] = 0xAA;
    dataPID[2] = 0xB1;
    dataPID[3] = 0x0C;
    // 放置数据
    
    memcpy(&dataPID[4], kp_bytes, 4); // [4,7]
    memcpy(&dataPID[8], ki_bytes, 4); // [8,11]
    memcpy(&dataPID[12], kd_bytes, 4);// [12,15]
    
    // 校验位
    dataPID[16] = check_sum(&dataPID[2], 2 + 12);
    dataPID[17] = 0xBB;
    
    USART0_send_data_array(dataPID, 18);
#endif
}

void taskSend(void *pvParameters) {


    uint8_t cnt = 0;
    uint16_t angle = 0;
    while(1) { // 0x012B
        // 100ms发送一次当前位置 
        sendCurrentAngle(angle);
        angle = (angle + 1) % 300;
        
        // 2000ms发送一次当前PID
        if(cnt++ >= 20){ // 100ms * 20 = 2000ms
            sendCurrentPID();
            cnt = 0;
//            kp += 0.1f;
        }
        
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}


QueueHandle_t xQueueRecv;

#if !PROTOCOL_enterprise
void USART0_on_recv(uint8_t* data, uint32_t len) {
    // 此代码是在中断里执行的, 不要做耗时操作 (delay_ms)
//    printf("recv[%d]->%s\n", len, data);
    
//    print_bytes("recv", data, len);
//    for(uint32_t i = 0; i < len; i++){
//        xQueueSendFromISR(xQueueRecv, data + i, NULL);
//    }
    
    if(data[0] == 0xF0){ // 目标角度信息
        //  0xF0 0x01 0x26 
        targetAngle = (data[1] << 8) | data[2];
    }else if(data[0] == 0xE0){ // PID
        uint8_t type = data[1]; // 用于区分是什么类型数据
        float value = bytesToFloat(&data[2]); // 具体的float数值
        
        if(type == 0x00){   // kp  0xE0 0x00 0x414574BC
            kp = value;
        }else if(type == 0x01){ // ki
            ki = value;
        }else if(type == 0x02){ // kd
            kd = value;
        }
    }
    
    printf("P: %.3f I: %.3f D: %.3f Tar: %d\n", kp, ki, kd, targetAngle);
    
}

#else

// 每收到一个字节, 就放到消息队列里, 等待处理
void USART0_on_byte_recv(uint8_t byte){
    // 从队尾添加数据
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    xQueueSendFromISR(xQueueRecv, &byte, &xHigherPriorityTaskWoken);
    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}
#endif

/*******************************************
执行数据解析任务 独立任务 main

    0  1      2        3        4   5      6      7 
	帧头   命令位  数据长度     数据位   校验位   帧尾 
	AA AA    F0       02       00  63     55     BB

target-> AA AA F0 02 00 63 55 BB
kp ->    AA AA E0 05 00 3F 0C CC CD C9 BB
ki ->    AA AA E0 05 01 3C 34 39 58 E7 BB
kd ->    AA AA E0 05 02 3F 8C CC CD 4B BB
1. 循环丢弃不符合协议的数据
2. 根据数据长度等待完整数据包
3. 对数据包进行校验（ADD8）
4. 校验成功则将数据返回给用户

支持断包 
AA AA E0 05 01 3C 
34 39 58 E7 BB

支持粘包 AA AA E0 05 00 3F 0C CC CD C9 BB AA AA E0 05 00 3F 0C CC CD C9 BB

*********************************************/


// 数据解析缓冲区
uint8_t  g_read_buf[DATA_PACKAGE_LEN_MAX];
// 已经缓存的字节个数
uint32_t g_recv_cnt = 0;

// 重置缓冲区
#define RESET_BUF() g_recv_cnt = 0

void taskRecv(void *pvParameters) {
    xQueueRecv = xQueueCreate(128, sizeof(uint8_t));
    uint32_t g_recv_data_cnt = 0;
    uint8_t data;
    BaseType_t xSuccess;
    while(1) {
        // 从队首取出数据        
        xSuccess = xQueueReceive(xQueueRecv, &data, portMAX_DELAY);
        
        #if 1          
        Protocol_parse(data);
        continue;
        
        #endif
        
//        printf("0x%02X\n", data);           
        // 把字节放在数组最新的位置
        g_read_buf[g_recv_cnt++] = data;
         
        // ---------------------- 判定内容 -----------------------
        // 判断0位是否是 0xAA
        if(g_recv_cnt == 1 && g_read_buf[0] != FRAME_HEAD_1){
            RESET_BUF();
            continue;
        }
        // 判断0位是否是 0xAA
        if(g_recv_cnt == 2 && g_read_buf[1] != FRAME_HEAD_2){
            RESET_BUF();
            continue;
        }
        
        if(g_recv_cnt == 3) continue;
        
        // 记录要接收的数据长度
        if(g_recv_cnt == 4) {
            g_recv_data_cnt = g_read_buf[3];
            // 检查是否在合理范围
            if(g_recv_data_cnt + DATA_PACKAGE_LEN_MIN > DATA_PACKAGE_LEN_MAX){
                RESET_BUF();
                continue;
            }
        }
        
        // 一直等待, 直到收到的数据个数符合要求
        int package_len = g_recv_data_cnt + DATA_PACKAGE_LEN_MIN;
        if(g_recv_cnt < package_len){
            continue;
        }
        
        // ------------------------ 开始校验 --------------------
//        print_bytes("Recv", g_read_buf, package_len);
        if(g_read_buf[package_len - 1] != FRAME_TAIL){
            printf("帧尾校验失败\n");
            RESET_BUF();
            continue;
        }
        
        // 计算校验码
        // 获取期望的校验码
        uint8_t except_checksum = g_read_buf[package_len - 2];
        // 计算真实的检验码
        uint8_t actual_checksum = check_sum(&g_read_buf[2], 2 + g_recv_data_cnt);
        
        if(except_checksum != actual_checksum){
            printf("校验失败except: %02X actual: %02X\n", except_checksum, actual_checksum);
            RESET_BUF();
            continue;
        }
        
        print_bytes("Passed", g_read_buf, package_len);        
        // 校验通过了, 在这里才可以安全地使用数据
        
        // 清理数据
        RESET_BUF();        
    }
}

