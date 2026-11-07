#include "main.h"


/* Pass */
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

/* Pass */
void TC_Write_Single_mode_CTRL_REG3_22H(void) {
  // Step1: Read register 22h
  // Write 
  // Read again to confirm
  printf("%s = 0x%x\n", l3gd20_find_name(L3GD20_REG_CTRL_REG3), read1byte_l3gd20(L3GD20_REG_CTRL_REG3));
  write_l3gd20(L3GD20_REG_CTRL_REG3, SINGLE_MODE, 1,(uint8_t[]){0x80}); //C99 Upper
  printf("%s = 0x%x\n", l3gd20_find_name(L3GD20_REG_CTRL_REG3), read1byte_l3gd20(L3GD20_REG_CTRL_REG3));

}

/* Checkpoint : Test mutiple write 
   Describle  : Write to two consecutive registers, 
                and then read them back to verify that the values were written correctly.
   Status     : PASS
*/
void TC_Write_Mul_Mode_CTRL_REG1_20H_21H(void) {
  printf("TC_Write_Mul_Mode_CTRL_REG1_20H_21H\n");
// Write DR1-DR0 = 01: Select the output data rate (ODR).
// Write BW1-BW0 = 01: Select the bandwidth.
// Corresponding settings:
// ODR = 190 Hz
// Cut-off frequency = 70 Hz
// 0b01010000 = 0x50

// REG_CTRL_REG2
// HPCF3-0 = 0011 => 0.9 cutoff 
// 0x03

  printf("%s = 0x%x\n", l3gd20_find_name(L3GD20_REG_CTRL_REG1), read1byte_l3gd20(L3GD20_REG_CTRL_REG1));
  printf("%s = 0x%x\n", l3gd20_find_name(L3GD20_REG_CTRL_REG2), read1byte_l3gd20(L3GD20_REG_CTRL_REG2));
  write_l3gd20(L3GD20_REG_CTRL_REG1, MUTILPLE_MODE, 2,(uint8_t[]){0x50,0x03});
  printf("%s = 0x%x\n", l3gd20_find_name(L3GD20_REG_CTRL_REG1), read1byte_l3gd20(L3GD20_REG_CTRL_REG1));
  printf("%s = 0x%x\n", l3gd20_find_name(L3GD20_REG_CTRL_REG2), read1byte_l3gd20(L3GD20_REG_CTRL_REG2));
}



/* Checkpoint : Test mutiple read 
   Describle  : Write to two consecutive registers, 
                and then read them back to verify that the values were written correctly.
   Status     : PASS
*/
void TC_Read_Mul_Mode_CTRL_REG1_20H_21H(void) {
  printf("TC_Read_Mul_Mode_CTRL_REG1_20H_21H\n");
// Write DR1-DR0 = 01: Select the output data rate (ODR).
// Write BW1-BW0 = 01: Select the bandwidth.
// Corresponding settings:
// ODR = 190 Hz
// Cut-off frequency = 70 Hz
// 0b01010000 = 0x50

// REG_CTRL_REG2
// HPCF3-0 = 0011 => 0.9 cutoff 
// 0x03
  uint8_t buff[3] = {0};
  write_l3gd20(L3GD20_REG_CTRL_REG1, MUTILPLE_MODE, 2,(uint8_t[]){0x55,0x13});
  printf("%s = 0x%x\n", l3gd20_find_name(L3GD20_REG_CTRL_REG1), read1byte_l3gd20(L3GD20_REG_CTRL_REG1));
  printf("%s = 0x%x\n", l3gd20_find_name(L3GD20_REG_CTRL_REG2), read1byte_l3gd20(L3GD20_REG_CTRL_REG2));
  read_l3gd20(L3GD20_REG_CTRL_REG1, MUTILPLE_MODE, 3,buff);
  printf("%s = 0x%x\n", l3gd20_find_name(L3GD20_REG_CTRL_REG1), buff[0]);
  printf("%s = 0x%x\n", l3gd20_find_name(L3GD20_REG_CTRL_REG2), buff[1]);
  printf("%s = 0x%x\n", l3gd20_find_name(L3GD20_REG_CTRL_REG2), buff[2]);
}
