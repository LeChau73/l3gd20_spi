/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"
#include "l3gd20.h"
#include "spi.h"
#include "log.h"
/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */
void UART_PrintInt(int32_t n);
void UART_PrintStr(const char *str);
void UART_PrintChar(char c);
/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */
void ConfigDMA2_SPI1_Init(void);
/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/

/* USER CODE BEGIN Private defines */
void Read_Gyro_WHO_AM_I();
void TC_Write_Single_mode_CTRL_REG3_22H(void);
void TC_Write_Mul_Mode_CTRL_REG1_20H_21H(void);
void TC_Read_Mul_Mode_CTRL_REG1_20H_21H(void);
/* USER CODE END Private defines */

/* Maxtrix for quick look up channel and stream*/
typedef struct
{
   DMA_Stream_TypeDef *stream;
   uint8_t channel;
   uint8_t stream_index;
}lk_stream_and_channel;


typedef enum {
    DMA_REQ_NONE       = 0,
    DMA_REQ_ADC1       = (1u << 0),
    DMA_REQ_SPI1_RX    = (1u << 1),
    DMA_REQ_SPI1_TX    = (1u << 2),
    DMA_REQ_SPI4_RX    = (1u << 3),
    DMA_REQ_SPI4_TX    = (1u << 4),
    DMA_REQ_SPI5_RX    = (1u << 5),
    DMA_REQ_SPI5_TX    = (1u << 6),
    DMA_REQ_USART1_RX  = (1u << 7),
    DMA_REQ_USART1_TX  = (1u << 8),
    DMA_REQ_USART6_RX  = (1u << 9),
    DMA_REQ_USART6_TX  = (1u << 10),
    DMA_REQ_SDIO       = (1u << 11),
    DMA_REQ_TIM1_CH1   = (1u << 12),
    DMA_REQ_TIM1_CH2   = (1u << 13),
    DMA_REQ_TIM1_CH3   = (1u << 14),
    DMA_REQ_TIM1_CH4   = (1u << 15),
    DMA_REQ_TIM1_TRIG  = (1u << 16),
    DMA_REQ_TIM1_COM   = (1u << 17),
    DMA_REQ_TIM1_UP    = (1u << 18),
} pheripheral_name;

#define CHANNEL 8
#define STREAM  8







#ifdef __cplusplus
}
#endif



#endif /* __MAIN_H */
