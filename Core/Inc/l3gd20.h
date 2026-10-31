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
#define MAX_NUMBER_REGISTER 20
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

typedef struct 
{
    char name[15];
    uint8_t register_addr;
}ty_table_l3gd20_register;


static ty_table_l3gd20_register table_l3gd20_register[MAX_NUMBER_REGISTER] = {
    {"WHO_AM_I",  0xF},
    {"CTRL_REG1", 0x20}
};


static inline char* find_name_reister ( const ty_table_l3gd20_register* table , const uint8_t addr_nb ) {
    for (uint8_t i = 0; i < MAX_NUMBER_REGISTER; i++)
    {
        if ( table[i].register_addr == addr_nb ) 
        {
            return table->name;
        }
    }

    return "No Find";
}



//Todo: Design additions wrapper interface to lower layer
// Interface protocol


int config_l3gd20(ty_l3gd20* l3gd20);
static inline char* find_name_reister ( const ty_table_l3gd20_register* table , const uint8_t addr_nb );
int write_l3gd20(uint8_t register_add, uint8_t write_mode, uint8_t data);
int read_l3gd20(uint8_t register_add, uint8_t read_mode, uint16_t size, uint8_t* buff_data);







#endif