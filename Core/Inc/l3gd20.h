#ifndef L3GD20_H
#define L3GD20_H

#include <stdint.h>
#include <stdio.h>
#include "stm32f4xx_hal.h"
#include "spi.h"
//#include "stm32f4xx_hal_gpio.h"

#define OK          0
//#define ERROR       (-1)

#define SINGLE_MODE 0
#define MUTILPLE_MODE 1

#define GYRO_CS_LOW()   HAL_GPIO_WritePin(GPIOE, GPIO_PIN_3, GPIO_PIN_RESET)
#define GYRO_CS_HIGH()  HAL_GPIO_WritePin(GPIOE, GPIO_PIN_3, GPIO_PIN_SET)

// Variable

typedef enum {
    SPI,
    I2C
}ty_protocol;


typedef struct
{
    uint8_t clk_phase_and_polar;
    uint8_t f_clk;        //range 1 - > 10 MHZ . Example : f_clk = 5 ~ 5MHZ
    ty_protocol protocol;
    // ... 
}ty_l3gd20;


//Todo: Design additions wrapper interface to lower layer
// Interface protocol


int config_l3gd20(ty_l3gd20* l3gd20);

int write_l3gd20(uint8_t register_add, uint8_t write_mode, uint8_t data);
int read_l3gd20(uint8_t register_add, uint8_t read_mode, uint16_t size, uint8_t* buff_data);







#endif