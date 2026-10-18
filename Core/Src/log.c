#include "log.h"

extern UART_HandleTypeDef huart2;


/* ------------------ Utility Functions cho UART ------------------ */

void UART_PrintChar(char c)
{
  HAL_UART_Transmit(&huart2, (uint8_t *)&c, 1, HAL_MAX_DELAY);
}

void UART_PrintStr(const char *str)
{
  while (*str)
  {
    UART_PrintChar(*str++);
  }
}

void UART_PrintHexMutilple(const char *str, uint8_t size)
{
  while (size--)
  {
    UART_PrintHex( *str++ );
  }
}

void UART_PrintHex(uint8_t num)
{
  char hex_chars[] = "0123456789ABCDEF";
  UART_PrintChar(hex_chars[(num >> 4) & 0x0F]);
  UART_PrintChar(hex_chars[num & 0x0F]);
}

void UART_PrintRegister(volatile uint32_t reg) {
    char hex_chars[] = "0123456789ABCDEF";
    uint8_t byte_decode;

    UART_PrintStr("0x");

    for(int i = 0; i < 8; i++)
    {
        byte_decode = ( reg & 0xF0000000 ) >> 28;
        UART_PrintChar(hex_chars[byte_decode]);
        reg = reg << 4;       
    }
                                                  
    UART_PrintStr("\n");
}

int __io_putchar(int ch) {
  HAL_UART_Transmit(&huart2, (uint8_t *)&ch, 1, HAL_MAX_DELAY);
}