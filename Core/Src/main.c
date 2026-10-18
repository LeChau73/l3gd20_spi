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

/* Macro điều khiển chân Chip Select (CS) của Gyro onboard (PE3) */


/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_SPI1_Init(void);
static void MX_USART2_UART_Init(void);

void UART_PrintChar(char c);
void UART_PrintStr(const char *str);
void UART_PrintHex(uint8_t num);

/* Private user code ---------------------------------------------------------*/

uint8_t flag_exti0 = 0;
volatile uint16_t exti_pin = 0;
uint8_t  flag_exti = 0;

uint8_t Gyro_ReadRegister(uint8_t reg)
{
  uint8_t txData[2];
  uint8_t rxData[2] = {0, 0};

  /* 
   * Cấu hình byte lệnh:
   * - Bit 7 = 1: Lệnh READ
   * - Bit 6 = 0: Không tự động tăng địa chỉ (Single byte)
   */
  txData[0] = reg | 0x80; 
  txData[1] = 0xFF; // Dummy byte gửi đi để tạo xung Clock nhận dữ liệu về

  GYRO_CS_LOW();
  
  // Gửi 2 bytes và nhận đồng thời 2 bytes qua SPI
  HAL_SPI_TransmitReceive(&hspi1, txData, rxData, 2, HAL_MAX_DELAY);
  
  GYRO_CS_HIGH();

  return rxData[1]; // Byte thứ 2 là dữ liệu cảm biến phản hồi
}

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


void SPI1_IRQHandler(void) {
  HAL_SPI_IRQHandler(&hspi1);
}

void HAL_SPI_ErrorCallback(SPI_HandleTypeDef *hspi) {
  UART_PrintStr("\r\n=======SPI ERROR=======\r\n");
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
  HAL_Delay(200);
  Init_EXTI0_ButtonUser();


  UART_PrintStr("\033[2J\033[H"); // Clear terminal
  //TODO: In ra tan so hien tai, va delay
  UART_PrintStr("\r\n==================================\r\n");
  UART_PrintStr("  STM32F411 DISCOVERY SPI TEST    \r\n");
  UART_PrintStr("==================================\r\n");


  ty_l3gd20 l3gd20;
  l3gd20.protocol = SPI;

  config_l3gd20(&l3gd20);

  uint8_t data[2];

    /* Đọc thanh ghi WHO_AM_I (0x0F) của L3GD20 / I3G4250D */

  //0b11010100 ~ 
  uint8_t retry = 1;
  do
  {
    HAL_StatusTypeDef status = read_l3gd20(0x0F, 0, 1, data);
    if (status == HAL_OK)
    {
      if(data[0] == 0b11010100) {
        UART_PrintStr("Correct ^-^ Gyro WHO_AM_I: 0x");
        UART_PrintStr(data);
        UART_PrintStr("\r\n");
        retry = 0;
      } else {
        UART_PrintStr("InCorrect -_- Gyro WHO_AM_I 0x");
        UART_PrintHex(data[0]);
        UART_PrintStr("\r\n");
      }
    } else {
      UART_PrintStr("Fail read l3gd20\n");
    }
    HAL_Delay(5000);
  } while (retry);
  

  /* Vòng lặp chính */
  int count = 10U;
  while (1)
  {
    

  }
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
  hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_16; // Baudrate ~1MHz
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