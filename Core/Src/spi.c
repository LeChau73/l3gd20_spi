#include "spi.h"


extern SPI_HandleTypeDef hspi1;


int spi_write(uint8_t* data, uint16_t size,  uint8_t timeout) {
#if(DEBUG_SPI)
    printf("File spi.c, func: spi_write : ");
    UART_PrintHexMutilple(data, size);
    printf(" , size = %d , timeoout = %d\n", size, timeout);
#endif
    // Enable SPI
    if( ( hspi1.Instance->CR1 & ( 0x1 << 6 ) ) == 0 )
        hspi1.Instance->CR1 |= (0x1 << 6);
    
    if( data == NULL || size == 0 )
        return ERROR;

    // Check lieu rang interface cos dang su dung khong
    while( hspi1.Instance->SR & ( 0x1 << 7 ) ); //Wait until BUSY bit == 0
    while (size--)
    {
        while ( !(hspi1.Instance->SR & ( 0x1 << 1 ) ) ); // Wait TXE empty (buffer) , TXE == 1
        hspi1.Instance->DR = *data++;
#if(DEBUG_SPI)
    printf("READ :DR = %x\n", (uint8_t)hspi1.Instance->DR);
#endif
    }

    return OK;
}


int spi_read(uint8_t* buff_data, uint16_t size, uint8_t timeout) {
    uint8_t index = 0;

    if( ( hspi1.Instance->CR1 & ( 0x1 << 6) ) == 0 ) 
        return ERROR;
    
    if( buff_data == NULL)
        return ERROR;

    while( hspi1.Instance->SR & ( 0x1 << 7 ) ); //Wait until BUSY bit == 0
    while (size--)
    {
        // RXE = 1 (not empty)
        //TODO: can phai cos timeout
        while ( !( ( hspi1.Instance->SR & 0x1 ) == 1 ) ); // Block when RXE = 0 (empty)
        buff_data[index] = hspi1.Instance->DR;
#if(DEBUG_SPI)
    printf("spi_read :buff_data[%d] = %x\n", index, buff_data[index]);
#endif
        index++;
    }

    buff_data[index] = '\0';
#if(DEBUG_SPI)
    printf("File spi.c, func: spi_read :Result read data = %s\n", buff_data);
#endif

    return OK;
}




//TODO: Hieu cachs no nhan data