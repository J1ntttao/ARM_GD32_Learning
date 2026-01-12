#ifndef _BSP_W25Q64_H__
#define _BSP_W25Q64_H__

#include "stm32f4xx.h"
#include "spi.h"


#define W25Q_CS_PORT			GPIOA
#define W25Q_CS_PIN				GPIO_PIN_4

#define W25Q_CS_SELECT()		  HAL_GPIO_WritePin(W25Q_CS_PORT, W25Q_CS_PIN, GPIO_PIN_RESET)
#define W25Q_CS_UNSELECT()		HAL_GPIO_WritePin(W25Q_CS_PORT, W25Q_CS_PIN, GPIO_PIN_SET)

#define W25Q_SPI_RD_WR(data)	MX_SPI1_write_read(data)

void W25Q64_init(void);
uint16_t W25Q64_readID(void);
void W25Q64_write(uint8_t* buffer, uint32_t addr, uint16_t numbyte);
void W25Q64_read(uint8_t* buffer,uint32_t read_addr,uint16_t read_length) ;
#endif
