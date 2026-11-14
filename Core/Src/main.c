/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : STM32F411 Discovery Onboard Gyroscope SPI1 Test
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "stdlib.h"

/* Private variables ---------------------------------------------------------*/
SPI_HandleTypeDef hspi1;
UART_HandleTypeDef huart2;
DMA_HandleTypeDef dma1_spi_trans;


/* Hàng = Channel, cột = Stream */
const pheripheral_name maxtrix_dma2[CHANNEL][STREAM] = {
    /* Stream:   0                 1                 2                 3                 4                 5                 6                                                        7 */
    /* Ch0 */ { DMA_REQ_ADC1,      DMA_REQ_NONE,     DMA_REQ_NONE,     DMA_REQ_NONE,     DMA_REQ_ADC1,     DMA_REQ_NONE,     DMA_REQ_TIM1_CH1 | DMA_REQ_TIM1_CH2 | DMA_REQ_TIM1_CH3,  DMA_REQ_NONE },
    /* Ch1 */ { DMA_REQ_NONE,      DMA_REQ_NONE,     DMA_REQ_NONE,     DMA_REQ_NONE,     DMA_REQ_NONE,     DMA_REQ_NONE,     DMA_REQ_NONE,                                            DMA_REQ_NONE },
    /* Ch2 */ { DMA_REQ_NONE,      DMA_REQ_NONE,     DMA_REQ_SPI1_TX,  DMA_REQ_SPI5_RX,  DMA_REQ_SPI5_TX,  DMA_REQ_NONE,     DMA_REQ_NONE,                                            DMA_REQ_NONE },
    /* Ch3 */ { DMA_REQ_SPI1_RX,   DMA_REQ_NONE,     DMA_REQ_SPI1_RX,  DMA_REQ_SPI1_TX,  DMA_REQ_NONE,     DMA_REQ_SPI1_TX,  DMA_REQ_NONE,                                            DMA_REQ_NONE },
    /* Ch4 */ { DMA_REQ_SPI4_RX,   DMA_REQ_SPI4_TX,  DMA_REQ_USART1_RX,DMA_REQ_SDIO,     DMA_REQ_SPI4_RX,  DMA_REQ_USART1_RX,DMA_REQ_SDIO,                                            DMA_REQ_USART1_TX },
    /* Ch5 */ { DMA_REQ_NONE,      DMA_REQ_USART6_RX,DMA_REQ_USART6_RX,DMA_REQ_SPI4_RX,  DMA_REQ_SPI4_TX,  DMA_REQ_SPI5_TX,  DMA_REQ_USART6_TX,                                       DMA_REQ_USART6_TX },
    /* Ch6 */ { DMA_REQ_TIM1_TRIG, DMA_REQ_TIM1_CH1, DMA_REQ_TIM1_CH2, DMA_REQ_TIM1_CH1, DMA_REQ_TIM1_CH4 | DMA_REQ_TIM1_TRIG | DMA_REQ_TIM1_COM, DMA_REQ_TIM1_UP, DMA_REQ_TIM1_CH3,   DMA_REQ_NONE },
    /* Ch7 */ { DMA_REQ_NONE,      DMA_REQ_NONE,     DMA_REQ_NONE,     DMA_REQ_NONE,     DMA_REQ_NONE,     DMA_REQ_SPI5_RX,  DMA_REQ_SPI5_TX,                                         DMA_REQ_NONE },
};

DMA_Stream_TypeDef * const lk_str_dma2[STREAM] = {
    DMA2_Stream0, DMA2_Stream1, DMA2_Stream2, DMA2_Stream3,
    DMA2_Stream4, DMA2_Stream5, DMA2_Stream6, DMA2_Stream7,
};

/* Macro điều khiển chân Chip Select (CS) của Gyro onboard (PE3) */
#define TEST 1
 
/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_SPI1_Init(void);
static void MX_USART2_UART_Init(void);

void UART_PrintChar(char c);
void UART_PrintStr(const char *str);
void UART_PrintHex(uint8_t num);
lk_stream_and_channel dma2_find(const pheripheral_name name);

/* Private user code ---------------------------------------------------------*/

uint8_t flag_exti0 = 0;
volatile uint16_t exti_pin = 0;
uint8_t  flag_exti = 0;

void EXTI0_IRQHandler(void) {

   HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_0);
   UART_PrintStr("\r\n=======EXTI0_IRQHandler Complete=======\r\n");
   UART_PrintStr("\r\n=======Flag = {");
   UART_PrintChar(flag_exti0);
   UART_PrintStr("}---\r\n");

}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
  UNUSED(GPIO_Pin);

  exti_pin  = GPIO_Pin;
  flag_exti = 1;
  if (GPIO_Pin == GPIO_PIN_0) {
      if(!flag_exti0)
        flag_exti0 = 1;
  }


}

void DMA2_Stream2_IRQHandler(void) {
  //TODO: Clear ddang sai
  UART_PrintStr("\r\n=======DMA2_Stream2_IRQHandler Complete=======\r\n");
  SET_BIT(DMA2->LIFCR, DMA_LIFCR_CTCIF2);
  SET_BIT(DMA2->LIFCR, DMA_LIFCR_CHTIF2);
  //HAL_NVIC_ClearPendingIRQ(DMA2_Stream2_IRQn);
}


void Init_EXTI0_ButtonUser(void) {
  GPIO_InitTypeDef gpio = {0};
  __HAL_RCC_GPIOA_CLK_ENABLE();
  gpio.Mode = GPIO_MODE_IT_RISING;
  gpio.Pull = GPIO_NOPULL;

  gpio.Pin = GPIO_PIN_0;                 // Discovery / Black Pill
  HAL_GPIO_Init(GPIOA, &gpio);

  HAL_NVIC_SetPriority(EXTI0_IRQn, 2, 0);
  HAL_NVIC_EnableIRQ(EXTI0_IRQn);
}


inline lk_stream_and_channel dma2_find(const pheripheral_name name) {
    lk_stream_and_channel kq = { NULL, 0, 0};
    for(uint8_t x = 0; x < CHANNEL ; x++) {
      for (uint8_t i = 0; i < STREAM; i++)
      {
          if(maxtrix_dma2[x][i] == name){
              kq.channel = x;
              kq.stream = lk_str_dma2[i];
              kq.stream_index  = i;
          }
      }
      
    }
    return kq;
}


void SPI1_IRQHandler(void) {
  HAL_SPI_IRQHandler(&hspi1);
}

void HAL_SPI_ErrorCallback(SPI_HandleTypeDef *hspi) {
  UART_PrintStr("\r\n=======SPI ERROR=======\r\n");
}



static void DumpClockSystemAndSyTick() {
  
  printf("Clock System = %d\n", SystemCoreClock);
  printf("Frequence of systick = ");
  switch (HAL_GetTickFreq())
  {
  case HAL_TICK_FREQ_10HZ:
    printf("10HZ => T = 0,1s \n");
    break;
  case HAL_TICK_FREQ_100HZ:
    printf("100HZ => T = 0,01s \n");
    break;
  case HAL_TICK_FREQ_1KHZ:
    printf("1000HZ => T = 1ms \n");
    break;
  default:
    break;
  }
}

void HAL_SPI_TxCpltCallback(SPI_HandleTypeDef *hspi)
{
  printf("Complete SPI tran\n");

  /* Prevent unused argument(s) compilation warning */
  UNUSED(hspi);

  /* NOTE : This function should not be modified, when the callback is needed,
            the HAL_SPI_TxCpltCallback should be implemented in the user file
   */
}


/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* Khởi tạo hệ thống HAL, Clock và Peripherals */
  HAL_Init();
  SystemClock_Config();
  MX_GPIO_Init();
  MX_SPI1_Init();
  MX_USART2_UART_Init();

  ConfigDMA2_SPI1_Init();

  HAL_Delay(200);
  Init_EXTI0_ButtonUser();
  UART_PrintStr("\033[2J\033[H"); // Clear terminal

  UART_PrintStr("\r\n==================================\r\n");
  UART_PrintStr("  STM32F411 INIT    \r\n");
  UART_PrintStr("==================================\r\n");
  DumpClockSystemAndSyTick();
  uint16_t current_tick = HAL_GetTick();
  printf("First Systick = %d\n", current_tick);
  while (HAL_GetTick() - current_tick < 1000);
  printf("End Systick = %d\n", HAL_GetTick());


  UART_PrintStr("\r\n==================================\r\n");
  UART_PrintStr("  STM32F411 DISCOVERY SPI TEST    \r\n");
  UART_PrintStr("==================================\r\n");

#if(TEST)
  Read_Gyro_WHO_AM_I();
  TC_Write_Single_mode_CTRL_REG3_22H();
  TC_Write_Mul_Mode_CTRL_REG1_20H_21H();
  TC_Read_Mul_Mode_CTRL_REG1_20H_21H();
#endif

  uint8_t data[100] = {0};
  for (int i = 0 ; i < 100 ; i++) {
    data[i] = i;
  }

  spi_write_dma(data, 100);




  /* Vòng lặp chính */
  int count = 10U;
  while (1)
  {
    

  }
}




void ConfigDMA2_SPI1_Init(void)
{
  __HAL_RCC_DMA2_CLK_ENABLE();
  /* Because I use SPI1_TX so that I need to config Stream */
  DMA_InitTypeDef config_dma;

  lk_stream_and_channel str_channel;

  str_channel = dma2_find(DMA_REQ_SPI1_TX);

  if(str_channel.stream == NULL && str_channel.channel == 0 && str_channel.stream == 0){
    printf("Cannot find stream and channel when look up !!!!!!! LINE = %d\n ", __LINE__);
    return;
  }

  config_dma.Channel = DMA_CHANNEL_2;
  config_dma.Direction = DMA_MEMORY_TO_PERIPH;
  config_dma.PeriphInc = DMA_PINC_DISABLE;
  config_dma.MemInc = DMA_MINC_ENABLE;
  config_dma.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
  config_dma.MemDataAlignment = DMA_MDATAALIGN_BYTE;
  config_dma.Mode = DMA_NORMAL;
  config_dma.Priority = DMA_PRIORITY_MEDIUM;
  config_dma.FIFOMode = DMA_FIFOMODE_DISABLE;
  //config_dma.PeriphBurst = 
  //config_dma.FIFOThreshold = 

  dma1_spi_trans.Instance = DMA2_Stream2;
  dma1_spi_trans.Init = config_dma;
  dma1_spi_trans.Lock = HAL_UNLOCKED;
                                    /* Comment doan nay */
  dma1_spi_trans.StreamBaseAddress = DMA2_Stream2_BASE;
  dma1_spi_trans.StreamIndex = 2;
  //dma1_spi_trans.XferCpltCallback = 
  HAL_DMA_Init(&dma1_spi_trans);
  
  HAL_NVIC_SetPriority(DMA2_Stream2_IRQn, 2, 0);
  HAL_NVIC_EnableIRQ(DMA2_Stream2_IRQn);
}



/**
  * @brief System Clock Configuration (HSI 16MHz)
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK
                              | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief SPI1 Initialization Function
  */
static void MX_SPI1_Init(void)
{
  __HAL_RCC_SPI1_CLK_ENABLE();

  hspi1.Instance = SPI1;
  hspi1.Init.Mode = SPI_MODE_MASTER;
  hspi1.Init.Direction = SPI_DIRECTION_2LINES;
  hspi1.Init.DataSize = SPI_DATASIZE_8BIT;

  /* Cấu hình SPI Mode 3 (CPOL High, CPHA 2Edge) cho L3GD20 */
  hspi1.Init.CLKPolarity = SPI_POLARITY_HIGH;
  hspi1.Init.CLKPhase = SPI_PHASE_2EDGE;

  hspi1.Init.NSS = SPI_NSS_SOFT;
  hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_8; // Baudrate ~2MHz
  hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi1.Init.CRCPolynomial = 10;
  
  if (HAL_SPI_Init(&hspi1) != HAL_OK)
  {
    Error_Handler();
  }


  HAL_NVIC_SetPriority(SPI1_IRQn, 3, 0);
  HAL_NVIC_EnableIRQ(SPI1_IRQn);
}

/**
  * @brief USART2 Initialization Function (115200 Baud, 8N1)
  */
static void MX_USART2_UART_Init(void)
{
  __HAL_RCC_USART2_CLK_ENABLE();

  huart2.Instance = USART2;
  huart2.Init.BaudRate = 115200;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief GPIO Initialization Function
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* Bật Clock cho các Port GPIO cần thiết */
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOE_CLK_ENABLE();

  /* 1. Cấu hình PE3: Chip Select (CS) cho Gyro Onboard */
  HAL_GPIO_WritePin(GPIOE, GPIO_PIN_3, GPIO_PIN_SET); // Mặc định HIGH (De-select)
  GPIO_InitStruct.Pin = GPIO_PIN_3;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

  /* 2. Cấu hình PD12: LED Green chỉ báo trạng thái */
  HAL_GPIO_WritePin(GPIOD, GPIO_PIN_12, GPIO_PIN_RESET);
  GPIO_InitStruct.Pin = GPIO_PIN_12;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

  /* 3. Cấu hình PA2 (TX), PA3 (RX): USART2 Alternate Function */
  GPIO_InitStruct.Pin = GPIO_PIN_2 | GPIO_PIN_3;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.Alternate = GPIO_AF7_USART2;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /* 4. Cấu hình PA5 (SCK), PA6 (MISO), PA7 (MOSI): SPI1 Alternate Function */
  GPIO_InitStruct.Pin = GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.Alternate = GPIO_AF5_SPI1;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
}


void Error_Handler(void)
{
  __disable_irq();
  while (1)
  {
  }
}
