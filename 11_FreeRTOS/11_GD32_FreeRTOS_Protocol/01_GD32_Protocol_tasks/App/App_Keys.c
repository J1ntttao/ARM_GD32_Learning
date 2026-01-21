#include "App.h"
#include "bsp_keys.h"

// 高内聚, 低耦合

void Keys_on_keydown(uint8_t key){
    printf("Key %d Pressed main\n", key);
    
    switch(key){
        case 0: App_Battery_start();    break;
        case 1: App_Battery_stop();     break;
        case 2: App_Battery_update();   break;
        case 3: App_Battery_show();     break;
        default: break;
    }
    
}
void Keys_on_keyup(uint8_t key){
//    printf("Key %d Released main\n", key);
}

void App_Keys_init(){
    // 初始化keys
    bsp_keys_init();
}

void App_Keys_task(){

    // 扫描按键
    bsp_keys_scan();
    
//    printf("scan\n");
}