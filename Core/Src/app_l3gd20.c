#include "main.h"



void Read_Gyro_WHO_AM_I() {
  /* Config */  
  
  ty_l3gd20 l3gd20;
  l3gd20.protocol = SPI;
  config_l3gd20(&l3gd20);

  uint8_t data[2];
  uint8_t retry = 1;
  do
  {
    uint8_t count = 0;
    printf("----------Readding Gyro WHO_AM_I: [%d] ----------\n", count++);
    HAL_StatusTypeDef status = read_l3gd20(0x0F, 0, 1, data);
    if (status == HAL_OK)
    {
      if(data[0] == 0xD3) {
        printf("Correct ^-^ Gyro WHO_AM_I: 0x%x\n", data[0]);
        retry = 0;
        break;
      } else {
        printf("In Correct -_- Gyro WHO_AM_I: 0x%x\n", data);
      }
    } else {
      UART_PrintStr("Fail read l3gd20\n");
    }
    HAL_Delay(5000);
  } while (retry);
}


void TC_Write_Single_mode_CTRL_REG3_22H(void) {
  // Step1: Read register 22h
  // Write 
  // Read again to confirm
  printf("Gyro CTRL_REG3 = %x\n", read1byte_l3gd20(0x22));


}