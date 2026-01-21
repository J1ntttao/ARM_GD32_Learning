#include "App.h"

/* 全局变量定义 */
static unsigned char anim_index = 0;  /* 动画索引，控制流动 */
static const unsigned char diagonal_path[28] = {
    /* 斜向流动顺序：从左上到右下 */
    0, 1, 4, 2, 5, 8, 3, 6, 9, 12, 7, 10, 13, 16, 11, 14, 17, 20, 15, 18, 21, 24, 19, 22, 25, 26, 23, 27
};

void StandbyAnimationWave() {
    unsigned char i;
    const unsigned char rainbow[7] = {0, 1, 2, 3, 4, 5, 6};  /* 彩虹色 */
    unsigned char trail_length = 5;  /* 拖尾长度 */
    
    if(DeviceState != DeviceDefault) return;
    
    /* 1. 先清除所有灯为无色 */
    for(i = 0; i < 28; i++) {
        WS2812_WriteColor2Array(ANS, i, 7);
    }
    
    /* 2. 绘制彩虹斜向流动效果 */
    /* 原理：在斜向路径上，从当前位置向前设置拖尾，每个拖尾点颜色不同 */
    for(i = 0; i < trail_length; i++) {
        /* 计算路径位置：当前位置+偏移 */
        unsigned char pos_index = (anim_index + i) % 28;
        unsigned char led_pos = diagonal_path[pos_index];
        
        /* 计算彩虹颜色：拖尾从前到后是完整彩虹 */
        unsigned char color_index = (i + anim_index) % 7;
        
        WS2812_WriteColor2Array(ANS, led_pos, rainbow[color_index]);
    }
    
    /* 3. 刷新显示 */
    WS2812_RefreshLEDColor(SPI_0);
    
    /* 4. 更新索引，控制流动速度 */
    anim_index++;
    if(anim_index >= 28) {
        anim_index = 0;
    }
}