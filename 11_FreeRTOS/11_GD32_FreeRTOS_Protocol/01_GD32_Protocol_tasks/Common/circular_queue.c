#include "circular_queue.h"

/**
 * @brief 创建消息队列函数
 * 
 * @param size 队列大小
 * @return QueueType_t* 队列指针
 */
QueueType_t * QueueCreate(uint16_t size) {
    // 分配队列结构体内存
    QueueType_t *queue = (QueueType_t *)malloc(sizeof(QueueType_t));
    if (queue == NULL) return NULL; // 分配队列结构体内存失败

    queue->head = 0;    // 队首索引初始化为0
    queue->tail = 0;    // 队尾索引初始化为0
    queue->size = size; // 队列总大小
    // 根据数据类型分配队列缓冲区内存
    queue->buffer = (uint8_t *)malloc(size * sizeof(uint8_t));
    if (queue->buffer == NULL) {
        // 分配队列缓冲区内存失败
        free(queue); // 释放队列结构体内存
        return NULL;
    }
    return queue;
}

/**
 * @brief 队列销毁回收函数
 * 
 * @param queue 队列指针
 */
void QueueDestroy(QueueType_t *queue) {
    if (queue == NULL) return;
    free(queue->buffer);
    free(queue);
}
// 队列是否为空
bool QueueIsEmpty(QueueType_t *queue){
    return (queue->head == queue->tail);
}

// 队列是否已满
bool QueueIsFull(QueueType_t *queue){
    return ((queue->tail + 1) % queue->size == queue->head);
}

/**
 * @brief 入队函数(从尾部入队)
 * 
 * @param queue 队列指针
 * @param data 要入队的数据
 * @return QueueStatus_t 队列状态
 */
 QueueStatus_t QueuePush(QueueType_t *queue, uint8_t data) {
    if (queue == NULL) return QUEUE_ERROR;
    // 检查队列是否已满
    if (QueueIsFull(queue)) return QUEUE_FULL;
    // 入队
    queue->buffer[queue->tail] = data;
    // 更新队尾指针
    queue->tail = (queue->tail + 1) % queue->size; // %size 防止越界, 回到队头
    return QUEUE_OK;
}

/**
 * @brief 出队函数(从头部出队)
 * 
 * @param queue 队列指针
 * @param data 出队的数据指针
 * @return QueueStatus_t 队列状态
 */
QueueStatus_t QueuePop(QueueType_t *queue, uint8_t *data) {
    if (queue == NULL || data == NULL) return QUEUE_ERROR;
    // 检查队列是否为空
    if (QueueIsEmpty(queue)) return QUEUE_EMPTY;
    // 出队
    *data = queue->buffer[queue->head];
    // 更新队头指针
    queue->head = (queue->head + 1) % queue->size; // %size 防止越界, 回到队头
    return QUEUE_OK;
}

// 入队一组数据(Enqueue)
QueueStatus_t QueuePushBatch(QueueType_t *queue, uint8_t *data, uint16_t len){
    if (queue == NULL || data == NULL || len == 0) return QUEUE_ERROR;
    // 入队
    while(len--){
        if(QueuePush(queue, *data++) != QUEUE_OK){
            return QUEUE_ERROR;
        }
    }
    return QUEUE_OK;
}

// 出队一组数据(Dequeue)
QueueStatus_t QueuePopBatch(QueueType_t *queue, uint8_t *data, uint16_t len){
    if (queue == NULL || data == NULL || len == 0) return QUEUE_ERROR;
    // 出队
    while(len--){
        if(QueuePop(queue, data++) != QUEUE_OK){
            return QUEUE_ERROR;
        }
    }
    return QUEUE_OK;
}

/**
 * @brief 获取队列当前元素个数
 * 
 * @param queue 队列指针
 * @return uint16_t 当前元素个数
 */
uint16_t QueueGetSize(QueueType_t *queue){
    if (queue == NULL) return 0;
    // return (queue->tail - queue->head + queue->size) % queue->size;

    // 队尾在队头之后
    if(queue->tail >= queue->head){
        return queue->tail - queue->head;
    }

    // 队尾在队头之前
    return queue->tail + queue->size - queue->head;
}

