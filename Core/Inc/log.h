#ifndef LOG_H
#define LOG_H
#include "stm32f4xx_hal.h"

void UART_PrintChar(char c);
void UART_PrintStr(const char *str);
void UART_PrintHex(uint8_t num);
void UART_PrintRegister(uint32_t reg);

#endif

