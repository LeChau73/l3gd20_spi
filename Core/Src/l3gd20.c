#include "l3gd20.h"
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
// struct internal, Only use this file
static ty_l3gd20* l3gd20_Config_Internal = NULL;
static uint8_t write_cache[10];
static uint8_t read_cache_buff[10];


int config_l3gd20(ty_l3gd20* l3gd20) {

    if( l3gd20 == NULL )
        return ERROR;
    
    if (!l3gd20_Config_Internal)
    {
        l3gd20_Config_Internal = (ty_l3gd20*)malloc(sizeof(ty_l3gd20));
        if (l3gd20_Config_Internal == NULL)
        {
            return ERROR;
        }
        
        memcpy( l3gd20_Config_Internal, l3gd20, sizeof(ty_l3gd20) );

    }


    return OK;
    
}



/* Describle : read data from sensor */
int read_l3gd20(uint8_t register_add, uint8_t read_mode, uint16_t size, uint8_t* buff_data) {

    uint8_t Timeout = 100; //100ms
    uint8_t number_byte_send;

    ty_l3gd20 config = *l3gd20_Config_Internal;

    // 1. Prepare data
     //Bit0 : R/W mode
    uint8_t cmd[3] = {0x00, 0xFF, 0xFF};


    cmd[0] |= 0x80; // READMODE , BIT 7 = 1

     //Bit1 : Multiple mode ?
    if (read_mode == MUTILPLE_MODE)
        cmd[0] |= 0x40;         // MUTIPLE MODE , BIT 6 = 1

    // Address of register
    cmd[0] |= (register_add & 0x3F);

#if(DEBUG_L3GD20)
    UART_PrintStr("File L3gd20.c: cmd[2] = ");
    UART_PrintHex(cmd[2]);
    UART_PrintChar('\n');
#endif
    number_byte_send = (read_mode == MUTILPLE_MODE ) ? 3 : 2;

    switch (config.protocol)
    {
    case SPI:
        // pull CS
        GYRO_CS_LOW();
        spi_write(&cmd[0], number_byte_send, Timeout);
        spi_read(buff_data, size, Timeout);
        GYRO_CS_HIGH();
        break;
    
    default:
        break;
    }

    
}




