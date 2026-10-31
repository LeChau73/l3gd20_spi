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
#define MAX_NUMBER_REGISTER \
    (sizeof(table_l3gd20_register) / sizeof(table_l3gd20_register[0]))
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



typedef enum {
    // Identification & Control
    L3GD20_REG_WHO_AM_I      = 0x0F,
    L3GD20_REG_CTRL_REG1     = 0x20,
    L3GD20_REG_CTRL_REG2     = 0x21,
    L3GD20_REG_CTRL_REG3     = 0x22,
    L3GD20_REG_CTRL_REG4     = 0x23,
    L3GD20_REG_CTRL_REG5     = 0x24,

    // Data & Status Registers
    L3GD20_REG_REFERENCE     = 0x25,
    L3GD20_REG_OUT_TEMP      = 0x26,
    L3GD20_REG_STATUS_REG    = 0x27,
    L3GD20_REG_OUT_X_L       = 0x28,
    L3GD20_REG_OUT_X_H       = 0x29,
    L3GD20_REG_OUT_Y_L       = 0x2A,
    L3GD20_REG_OUT_Y_H       = 0x2B,
    L3GD20_REG_OUT_Z_L       = 0x2C,
    L3GD20_REG_OUT_Z_H       = 0x2D,

    // FIFO Registers
    L3GD20_REG_FIFO_CTRL     = 0x2E,
    L3GD20_REG_FIFO_SRC      = 0x2F,

    // Interrupt 1 Registers
    L3GD20_REG_INT1_CFG      = 0x30,
    L3GD20_REG_INT1_SRC      = 0x31,
    L3GD20_REG_INT1_TSH_XH   = 0x32,
    L3GD20_REG_INT1_TSH_XL   = 0x33,
    L3GD20_REG_INT1_TSH_YH   = 0x34,
    L3GD20_REG_INT1_TSH_YL   = 0x35,
    L3GD20_REG_INT1_TSH_ZH   = 0x36,
    L3GD20_REG_INT1_TSH_ZL   = 0x37,
    L3GD20_REG_INT1_DURATION = 0x38
} l3gd20_reg_t;

typedef struct 
{
    char name[30];
    l3gd20_reg_t register_addr;
}ty_table_l3gd20_register;

static const ty_table_l3gd20_register table_l3gd20_register[] = {
    /* Identification & Control Registers */
    {"WHO_AM_I",      L3GD20_REG_WHO_AM_I},
    {"CTRL_REG1",     L3GD20_REG_CTRL_REG1},
    {"CTRL_REG2",     L3GD20_REG_CTRL_REG2},
    {"CTRL_REG3",     L3GD20_REG_CTRL_REG3},
    {"CTRL_REG4",     L3GD20_REG_CTRL_REG4},
    {"CTRL_REG5",     L3GD20_REG_CTRL_REG5},

    /* Data & Status Registers */
    {"REFERENCE",     L3GD20_REG_REFERENCE},
    {"OUT_TEMP",      L3GD20_REG_OUT_TEMP},
    {"STATUS_REG",    L3GD20_REG_STATUS_REG},
    {"OUT_X_L",       L3GD20_REG_OUT_X_L},
    {"OUT_X_H",       L3GD20_REG_OUT_X_H},
    {"OUT_Y_L",       L3GD20_REG_OUT_Y_L},
    {"OUT_Y_H",       L3GD20_REG_OUT_Y_H},
    {"OUT_Z_L",       L3GD20_REG_OUT_Z_L},
    {"OUT_Z_H",       L3GD20_REG_OUT_Z_H},

    /* FIFO Registers */
    {"FIFO_CTRL",     L3GD20_REG_FIFO_CTRL},
    {"FIFO_SRC",      L3GD20_REG_FIFO_SRC},

    /* Interrupt 1 Registers */
    {"INT1_CFG",      L3GD20_REG_INT1_CFG},
    {"INT1_SRC",      L3GD20_REG_INT1_SRC},
    {"INT1_TSH_XH",   L3GD20_REG_INT1_TSH_XH},
    {"INT1_TSH_XL",   L3GD20_REG_INT1_TSH_XL},
    {"INT1_TSH_YH",   L3GD20_REG_INT1_TSH_YH},
    {"INT1_TSH_YL",   L3GD20_REG_INT1_TSH_YL},
    {"INT1_TSH_ZH",   L3GD20_REG_INT1_TSH_ZH},
    {"INT1_TSH_ZL",   L3GD20_REG_INT1_TSH_ZL},
    {"INT1_DURATION", L3GD20_REG_INT1_DURATION}
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
HAL_StatusTypeDef write_l3gd20(uint8_t address_register, uint8_t mode, uint16_t size, uint16_t* data_write);
int read_l3gd20(uint8_t register_add, uint8_t read_mode, uint16_t size, uint8_t* buff_data);
uint8_t read1byte_l3gd20(uint8_t register_add);






#endif