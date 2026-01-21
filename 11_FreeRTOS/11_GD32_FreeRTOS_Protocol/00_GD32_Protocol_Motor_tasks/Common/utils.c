#include "utils.h"

// 打印字节数组
void print_bytes(const char* label, uint8_t bytes[], uint32_t len) {
    printf("%s-> ", label);
    for (int i = 0; i < len; i++) {
        printf("0x%02X ", bytes[i]);
    }
    printf("\r\n");
}


uint8_t check_sum(uint8_t* data, uint32_t len){

    uint16_t sum = 0;
    
    for(uint8_t i = 0; i < len; i++){    
        sum += data[i];
    }
    
    return (uint8_t)(sum & 0xFF);
}


uint8_t isLittleEndian(){ // 如果返回1,则是小端
	
	union {
		uint32_t value;   // 4字节整数
		uint8_t bytes[4]; // 4字节的数组 3, 2, 1, 0
	} test;
	
	test.value = 0x11223344;
	// 检查低地址位是44(小端little-endian) 还是11(大端big-endian)
	return test.bytes[0] == 0x44;
	
}



float bytesToFloat(uint8_t bytes[4]){
    union {    
        float value;
        uint8_t b[4];
    }u;
    u.b[0] = bytes[3];
    u.b[1] = bytes[2];
    u.b[2] = bytes[1];
    u.b[3] = bytes[0];    
    return u.value;
}


uint8_t isFloatLittleEndian(){ // 如果返回1,则是小端
	
	union {
		float value;   // 4字节整数
		uint8_t bytes[4]; // 4字节的数组 3, 2, 1, 0
	} test;
	
	test.value = 0.72f; // 0x3F3851EC
	// 检查低地址位是EC 还是 3F
	printf("test  0x%02X%02X%02X%02X\n", 
		test.bytes[3],test.bytes[2],test.bytes[1],test.bytes[0]);
    
    uint8_t* test1 = (uint8_t*)(&test.value);
	printf("test1 0x%02X%02X%02X%02X\n", 
		test1[3],test1[2],test1[1],test1[0]);
    
    
	return test.bytes[0] == 0xEC;	
}