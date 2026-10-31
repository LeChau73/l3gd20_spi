#include "l3gd20.h"
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
// struct internal, Only use this file
static ty_l3gd20* l3gd20_Config_Internal = NULL;
static uint8_t write_cache[10];
static uint8_t read_cache_buff[10];

#define DEBUG_L3GD20 0


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
    HAL_StatusTypeDef status;

    ty_l3gd20 config = *l3gd20_Config_Internal;

    // 1. Prepare data
     //Bit0 : R/W mode
    uint8_t cmd[2] = {0x00, 0xFF};


    cmd[0] |= 0x80; // READMODE , BIT 7 = 1

     //Bit1 : Multiple mode ?
    if (read_mode == MUTILPLE_MODE)
        cmd[0] |= 0x40;         // MUTIPLE MODE , BIT 6 = 1

    // Address of register
    cmd[0] |= (register_add & 0x3F);

    number_byte_send = (read_mode == MUTILPLE_MODE ) ? 2 : 1;

    switch (config.protocol)
    {
    case SPI:
        // pull CS
        GYRO_CS_LOW();

        /* Request to sensor */
        spi_write(&cmd[0], 1, Timeout);
        spi_read(buff_data, 1, Timeout);
        memset(buff_data, 0x00, 1);

        /* Read Actual Data */
        uint8_t i;
        for (i = 0; i < number_byte_send; i++)
        {
            status = spi_write(&cmd[1], 1, Timeout);
            status = spi_read(buff_data+i, 1, Timeout);
        }

        if(status != HAL_OK) {
            printf("Fail Read Write\n");
            return status;
        }
        
        buff_data[i] = '\0'; 

#if(DEBUG_L3GD20)
    //TODO: DEBUG tai sao k break dduwocj line printf
    printf("File l3gd20.c, func: Result read from [%s] = ", find_name_reister( table_l3gd20_register, register_add ));
    fflush(stdout);
    UART_PrintHexMutilple(buff_data, number_byte_send);
    printf("\n");
#endif

        GYRO_CS_HIGH();
        break;
    
    default:
        break;
    }

    return HAL_OK;
}

uint8_t read1byte_l3gd20(uint8_t register_add) {
    // 1. Prepare data
     //Bit0 : R/W mode
    uint8_t cmd[2] = {0x00, 0xFF};
    uint8_t data_reciver;

    cmd[0] |= 0x80; // READMODE , BIT 7 = 1

    // Address of register
    cmd[0] |= (register_add & 0x3F);                                                                                                                                                                                                                                                                                                       

    // pull CS
    GYRO_CS_LOW();

    /* Request to sensor */
    spi_write(&cmd[0], 1, 100);
    spi_read(&data_reciver, 1, 100);

    data_reciver = 0;

    /* Read Actual Data */
    spi_write(&cmd[1], 1, 1000);
    spi_read(&data_reciver, 1, 100);

    GYRO_CS_HIGH();

    return data_reciver;
}


HAL_StatusTypeDef write_l3gd20(uint8_t address_register, uint8_t mode, uint16_t size, uint16_t* data_write) {

    /* [1 byte : medata] + [ data ] */
    /* Note: Data of user should be MSB first */

    // Check input value is valid
    //TODO: macro

    uint8_t buffer[10] = {0};
    uint8_t length_to_Send = 0;

    if (mode == MUTILPLE_MODE)
        buffer[0] |= 0x40;                                  // MUTIPLE MODE , BIT 6 = 1

    buffer[0] |= (address_register & 0x3F);                 // add more

    memcpy(buffer + 1, data_write, size);                   // copy 


    length_to_Send = ( mode == SINGLE_MODE ) ? 2 : ( size + 1 );
    GYRO_CS_LOW();
    for (size_t i = 0; i < length_to_Send; i++)
    {

        if (spi_write(&buffer[i], 1, 100) == ERROR)
        {
            printf("Cannot pheripheral spi write \n");
            GYRO_CS_HIGH();
            return HAL_BUSY;
        }
        //HAL_Delay(100); //=> why is here i should wait 100ms?

    }
    GYRO_CS_HIGH();
    HAL_OK;
}