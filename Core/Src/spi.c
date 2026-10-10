#include "spi.h"


extern SPI_HandleTypeDef hspi1;


int spi_write(uint8_t* data, uint16_t size,  uint8_t timeout) {
    // 
    if( ( hspi1.Instance->CR1 & ( 0x1 << 6 ) ) == 0 )
        hspi1.Instance->CR1 |= (0x1 << 6);
    
    if( data == NULL || size == 0 )
        return ERROR;

    while( hspi1.Instance->SR & ( 0x1 << 7 ) ); //Wait until BUSY bit == 0

    while (size--)
    {
        
        while ( !(hspi1.Instance->SR & ( 0x1 << 1 ) ) ); // Wait TXE empty (buffer) , TXE == 1
        
        hspi1.Instance->DR = *data;
    }

    return OK;
}


int spi_read(uint8_t* buff_data, uint16_t size, uint8_t timeout) {
        // 
    if( ( hspi1.Instance->CR1 & ( 0x1 << 6) ) == 0 ) 
        return ERROR;
    
    if( buff_data == NULL)
        return ERROR;

    while( hspi1.Instance->SR & ( 0x1 << 7 ) ); //Wait until BUSY bit == 0

    while (size--)
    {
        while ( !(( hspi1.Instance->SR & 0x1 ) == 1) ); // Wait RXE is not empty (buffer), RXE = 1
        *buff_data = hspi1.Instance->DR;
        UART_PrintChar(*buff_data);
        buff_data++;
    }

    *buff_data = '\0';

    return OK;
}




