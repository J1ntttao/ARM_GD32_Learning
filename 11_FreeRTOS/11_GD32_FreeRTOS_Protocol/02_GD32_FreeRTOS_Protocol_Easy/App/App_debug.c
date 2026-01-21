#include "App.h"

#include "USART0.h"
#include "queue.h"

/**********************************************************
 * @brief 发送当前角度信息给上位机 
 * @param 当前舵机角度
 * 0xB0 data0 data1 0x0D 0x0A
 **********************************************************/
void sendCurrentAngle(uint16_t cur_angle){
    // static uint8_t dataCur[5] = {0xB0, 0x00, 0x00, '\r', '\n'}; // 0x0D, 0x0A   
    uint8_t dataCur[5] = {0}; // 0x0D, 0x0A   
    dataCur[0] = 0xB0;
    dataCur[1] = (cur_angle >> 8) & 0xFF;
    dataCur[2] = cur_angle & 0xFF;
    dataCur[3] = '\r';
    dataCur[4] = '\n';
    
    USART0_send_data_array(dataCur, 5);
}

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
    static uint8_t dataPID[15] = {0xB1,     // 0
        0x00, 0x00, 0x00, 0x00,             // 1,2,3,4
        0x00, 0x00, 0x00, 0x00,             // 5,6,7,8
        0x00, 0x00, 0x00, 0x00,             // 9,10,11,12
        '\r', '\n'}; // 0x0D, 0x0A          // 13, 14

    // 1. 将kp, ki, kd 分别转成4字节数组
    uint8_t* kp_bytes = (uint8_t*)&kp;
    uint8_t* ki_bytes = (uint8_t*)&ki;
    uint8_t* kd_bytes = (uint8_t*)&kd;
    
    #if 1
    // 2. 按照小端模式(小字节在低位) 填充dataPID
    memcpy(dataPID + 1, kp_bytes, 4); 
    memcpy(dataPID + 5, ki_bytes, 4);
    memcpy(dataPID + 9, kd_bytes, 4);
        
    #else
    //  2. 按照大端模式 将kp, ki, kd 分别转成4字节数组填充dataPID
    for (uint8_t i = 0; i < 4; i++){
        //  i == 0 -> 3
        //  i == 1 -> 2
        //  i == 2 -> 1
        //  i == 3 -> 0
        dataPID[1 + i] = kp_bytes[3 - i];
        dataPID[5 + i] = ki_bytes[3 - i];
        dataPID[9 + i] = kd_bytes[3 - i];
    }
    #endif
    // 3. 发送dataPID
    USART0_send_data_array(dataPID, 15);
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
            kp += 0.1f;
        }
        
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}

uint16_t targetAngle = 0;

QueueHandle_t xQueueRecv;

void USART0_on_recv(uint8_t* data, uint32_t len) {
    // 此代码是在中断里执行的, 不要做耗时操作 (delay_ms)
//    printf("recv[%d]->%s\n", len, data);
    print_bytes("recv", data, len);
//    BaseType_t xHigherPriorityTaskWoken;
//    for(uint32_t i = 0; i < len; i++){
//        xQueueSendFromISR(xQueueRecv, data + i, &xHigherPriorityTaskWoken);
//    }
//    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
    
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

void taskRecv(void *pvParameters) {
    
    xQueueRecv = xQueueCreate(128, sizeof(uint8_t));
    
    uint8_t data;
    BaseType_t xSuccess;
    while(1) {
//        xSuccess = xQueueReceive(xQueueRecv, &data, portMAX_DELAY);
//        
//        printf("0x%02X", data);
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

