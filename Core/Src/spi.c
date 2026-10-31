#include "spi.h"


extern SPI_HandleTypeDef hspi1;


int spi_write(uint8_t* data, uint16_t size,  uint16_t timeout) {
#if(DEBUG_SPI)
    printf("File spi.c, func: spi_write : ");
    fflush(stdout);
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
        while ( !(hspi1.Instance->SR & ( 0x1 << 1 ) )  ); // Wait TXE empty (buffer) , TXE == 1
        hspi1.Instance->DR = *data++;
    }

    return OK;
}


int spi_read(uint8_t* buff_data, uint16_t size, uint16_t timeout) {
    uint8_t index = 0;
    uint32_t first_tick = HAL_GetTick();
    uint32_t current_tick = 0;

    // Check SPI is enable ?
    if( ( hspi1.Instance->CR1 & ( 0x1 << 6 ) ) == 0 ) 
        return ERROR;
    
    if( buff_data == NULL)
        return ERROR;

    while( hspi1.Instance->SR & ( 0x1 << 7 ) ); //Wait until BUSY bit == 0

    while (size--)
    {
        // RXE = 1 (not empty) & cannot > timeout  // Block when RXE = 0 (empty)
        //TODO: check condition
        while ( ( !( ( hspi1.Instance->SR & 0x1 ) == 1 ) ) && ( current_tick < timeout ) ) {
            current_tick =  HAL_GetTick() - first_tick;
        }

        if ( current_tick > timeout  ) {

            printf("SPI READ TIMEOUT : TimeWait = %d , Time Actual Wait = %d \n", timeout, current_tick );
            printf("Bit RXE = %d | | %s \n", (uint8_t)hspi1.Instance->SR & 0x1, ( ((uint8_t)hspi1.Instance->SR & 0x1) == 0 ) ? "DR empty data" : "DR has data" );
            return HAL_TIMEOUT;
        }

        buff_data[index] = hspi1.Instance->DR;

#if(DEBUG_SPI)
    printf("spi_read :buff_data[%d] = %x\n", index, buff_data[index]);
#endif

        index++;
    }

#if(DEBUG_SPI)
    printf("File spi.c, func: spi_read :Result read data = ");
    for (int i = 0; i < index; i++)
    {
        printf("%x", buff_data[i]);
    }
    
    printf("\n");
#endif

    return OK;
}