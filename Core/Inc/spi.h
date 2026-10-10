#ifndef _SPI_H
#define _SPI_H
#include<stdint.h>
#include<stdio.h>
#include "stm32f4xx_hal.h"
#include "log.h"

#define OK          0
#define ERROR      (-1)



//SPI
int spi_write(uint8_t* data, uint16_t size,  uint8_t timeout);
int spi_read(uint8_t* buff_data, uint16_t size, uint8_t timeout);





#endif