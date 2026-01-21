#ifndef __CIRCULAR_QUEUE_H__
#define __CIRCULAR_QUEUE_H__

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

#ifndef NULL
#define NULL  (void *)0                   /* see <stddef.h> */
#endif

// RingBuffer    环形缓冲区
// CircularQueue 循环队列

typedef struct {
    uint16_t head;  // 队首索引
    uint16_t tail;  // 队尾索引
    uint16_t size;  // 队列总大小
    // uint16_t count; // 当前有效元素数量 [0, count]
    uint8_t *buffer;// 队列缓冲区    
} QueueType_t;

typedef enum {
    QUEUE_OK = 0, // 队列操作成功
    QUEUE_FULL,   // 队列已满
    QUEUE_EMPTY,  // 队列已空
    QUEUE_ERROR,  // 队列错误
} QueueStatus_t;

// 创建消息队列函数
QueueType_t * QueueCreate(uint16_t size);

// 队列销毁回收函数
void QueueDestroy(QueueType_t *queue);

// 入队函数(Enqueue从尾部入队)
QueueStatus_t QueuePush(QueueType_t *queue, uint8_t data);

// 出队函数(Dequeue从头部出队)
QueueStatus_t QueuePop(QueueType_t *queue, uint8_t *data);

// 入队一组数据(Enqueue)
QueueStatus_t QueuePushBatch(QueueType_t *queue, uint8_t *data, uint16_t len);

// 出队一组数据(Dequeue)
QueueStatus_t QueuePopBatch(QueueType_t *queue, uint8_t *data, uint16_t len);

// 获取队列当前元素数量
uint16_t QueueGetSize(QueueType_t *queue);

// 队列是否为空
bool QueueIsEmpty(QueueType_t *queue);

// 队列是否已满
bool QueueIsFull(QueueType_t *queue);

#endif