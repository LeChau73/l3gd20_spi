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
uint8_t Gyro_ReadRegister(uint8_t reg);

/* Private user code ---------------------------------------------------------*/

/**
  * @brief  Hàm đọc 1 thanh ghi từ cảm biến L3GD20 qua SPI1
  * @param  reg Địa chỉ thanh ghi cần đọc (vd: 0x0F)
  * @retval Giá trị đọc được từ thanh ghi
  */
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

int* a;

int func2(int var1_f2, int* var2_f2) {
  
  int adding = 5;
  int sum = var1_f2 + adding;

  

  sum += *var2_f2;

  return sum;

}

void func1(int var1, int var2, char var3, int* var4) {

  int sum_cal;


  sum_cal = func2(var1, var4);

  sum_cal += 2;

}

uint8_t flag_exti0 = 0;
volatile uint16_t exti_pin = 0;
volatile uint8_t  flag_exti = 0;

void EXTI0_IRQHandler(void) {

   HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_0);
   UART_PrintStr("\r\n=======EXTI0_IRQHandler Complete=======\r\n");
   UART_PrintStr("\r\n=======Flag = {");
   UART_PrintChar(flag_exti0);
   UART_PrintStr("}---\r\n");

}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(GPIO_Pin);
  /* NOTE: This function Should not be modified, when the callback is needed,
           the HAL_GPIO_EXTI_Callback could be implemented in the user file
   */

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
  gpio.Mode = GPIO_MODE_IT_RISING_FALLING;
  gpio.Pull = GPIO_NOPULL;

  gpio.Pin = GPIO_PIN_0;                 // Discovery / Black Pill
  HAL_GPIO_Init(GPIOA, &gpio);

  HAL_NVIC_SetPriority(EXTI0_IRQn, 2, 0);
  HAL_NVIC_EnableIRQ(EXTI0_IRQn);
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

  HAL_StatusTypeDef status = read_l3gd20(0x0F, 0, 1, data);

  UART_PrintStr("Gyro WHO_AM_I: 0x");
  UART_PrintStr(data);
  UART_PrintStr("\r\n");



  /* Đọc thanh ghi WHO_AM_I (0x0F) của L3GD20 / I3G4250D */
  //uint8_t chip_id = Gyro_ReadRegister(0x0F);
//
  //UART_PrintStr("Gyro WHO_AM_I: 0x");
  //UART_PrintHex(chip_id);
  //UART_PrintStr("\r\n");

  /* Kiểm tra mã ID phản hồi */
  //if (chip_id == 0xD4 || chip_id == 0xD3 || chip_id == 0xD7)
  //{
  //  UART_PrintStr("[RESULT] -> SPI Communication SUCCESS!\r\n");
  //}
  //else
  //{
  //  UART_PrintStr("[RESULT] -> SPI Communication FAILED!\r\n");
  //  UART_PrintStr("(Check SPI clock/mode or CS pin config)\r\n");
  //}
//
  /* Vòng lặp chính */
  int count = 10U;
  while (1)
  {
    // Nháy LED Green (PD12) báo hiệu hệ thống hoạt động
      if (flag_exti) {
            flag_exti = 0;
      if (exti_pin == GPIO_PIN_0)  UART_PrintStr("PA0 changed\r\n");
      }
    
    if (flag_exti0)
    {
      count = 10;
    }

    if (count == 0)
    {
      flag_exti0 = 0;
    }
    

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