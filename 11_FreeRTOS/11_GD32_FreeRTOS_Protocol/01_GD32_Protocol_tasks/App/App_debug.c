#include "App.h"
#include "Protocol.h"
#include "circular_queue.h"

QueueType_t* queuePtr;

void App_debug_init(){
    // 初始化消息队列
    queuePtr = QueueCreate(61); // 一帧数据最大个数 * 3 + 1
}


// 每收到一个字节, 就放到消息队列里, 等待处理
void USART0_on_byte_recv(uint8_t byte){
//    printf("0x%02X_", byte);
    QueuePush(queuePtr, byte);
}

/**********************************************************
 * @brief 发送当前角度信息给上位机 
 * @param 当前舵机角度
 * 0xB0 data0 data1 0x0D 0x0A
 **********************************************************/
// 100ms
void App_debug_sendCur_task(){
    static uint16_t angle = 0;
//    printf("sendCur_task\n");

    uint8_t payload[] = {angle >> 8, angle & 0xFF};
    Protocol_send(0xB0, payload, 2, USART0_send_data_array);
    
    angle += 1;
    angle %= 300;
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
// 2000ms
void App_debug_sendPID_task(){
//    printf("sendPID_task\n");
    
    static uint8_t dataPID[12] = {0};
    // 将kp, ki, kd 转成12字节数组
    memcpy(dataPID,     &kp, 4);  //  [0,3]
    memcpy(dataPID + 4, &ki, 4);  //  [4,7]
    memcpy(dataPID + 8, &kd, 4);  //  [8,11]
    
    Protocol_send(0xB1, dataPID, sizeof(dataPID) / sizeof(uint8_t), USART0_send_data_array);
    
}


//  从消息队列里取数据, 并进行分析
void App_debug_recv_task() { // 每10ms执行一次
//    printf("recv_task\n");
    
    uint8_t data;
    // 把所有已有字节数据出队
    while(QUEUE_OK == QueuePop(queuePtr, &data)){
        // 解析数据
        Protocol_parse(data);        
    }
        
}


void Protocol_callback(uint8_t cmd, uint8_t* data, uint32_t data_len){
    printf("Success: cmd: %02X data_len: %d\n", cmd, data_len);
}









