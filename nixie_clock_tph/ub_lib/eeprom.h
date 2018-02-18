#ifndef __EEPROM_H
#define __EEPROM_H


#include <stm32f4xx.h>
#include <misc.h>			 // I recommend you have a look at these in the ST firmware folder
#include <stm32f4xx_i2c.h> // under Libraries/STM32F4xx_StdPeriph_Driver/inc and src
#include <defines.h>


#define I2C_TIMEOUT_MAX 10000
#define MEM_DEVICE_WRITE_ADDR EEPROM
#define MEM_DEVICE_READ_ADDR MEM_DEVICE_WRITE_ADDR+1


enum  eepromtype  {M2401,M2402,M2404,M2408,M2416,M2432,M2464,M24128,M24256,M24512};

uint8_t Write_24Cxx(uint16_t, uint8_t, uint8_t);
uint8_t Read_24Cxx(uint16_t, uint8_t);

#endif
