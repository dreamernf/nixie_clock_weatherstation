//--------------------------------------------------------------
// File     : defines.h
//--------------------------------------------------------------

//--------------------------------------------------------------
#ifndef __STM32F4_DEFINES_H
#define __STM32F4_DEFINES_H


//--------------------------------------------------------------
// Includes
//--------------------------------------------------------------
#include "stm32f4xx.h"

//определения пунктов меню
#define  NO_MENU 0
#define  MENU_SET_MINUTE 									1
#define  MENU_SET_HOUR 										2
#define  MENU_SET_DAY 										3
#define  MENU_SET_MONTH	 									4
#define  MENU_SET_TIME_DISPLAY_CLOCK 				5
#define  MENU_SET_TIME_DISPLAY_PRESSURE 		6
#define  MENU_SET_TIME_DISPLAY_TEMP 				7
#define  MENU_SET_TIME_DISPLAY_DM 					8
#define  MENU_SET_TIME_DISPLAY_HUM 				9
#define  MENU_LAST  												MENU_SET_TIME_DISPLAY_HUM+1

//определения адресов устройств i2c
#define   BMP085    						0xEF  //датчик давления + температуры
#define   EEPROM    					0xAE  // ПЗУ для хранения настроек
#define   DS3231_I2C_ADDR   		0xD0  // i2c  адрес DS3231
#define   HTU21D  						0x80  // i2c  адрес HTU21D
#define   TLS2561  						0x29  // i2c  адрес TLS2561


// timekeeping registers
#define   DS3231_TIME_CAL_ADDR              0x00
#define   DS3231_CAL_ADDR         	            0x03
#define   DS3231_ALARM1_ADDR          		0x07
#define   DS3231_ALARM2_ADDR          		0x0B
#define   DS3231_CONTROL_ADDR         		0x0E
#define   DS3231_STATUS_ADDR          		0x0F
#define   DS3231_AGING_OFFSET_ADDR    	0x10
#define   DS3231_TEMPERATURE_ADDR     	0x11

// control register bits
#define   DS3231_A1IE     							0x01
#define   DS3231_A2IE     							0x02
#define   DS3231_INTCN    							0x04

// status register bits
#define   DS3231_A1F      							0x01
#define   DS3231_A2F      							0x02
#define   DS3231_OSF      							0x80

//символы знаком для дисплея погодной станции
#define  SYMBOL_PLUS    3 // знак плюс
#define  SYMBOL_MINUS  1 //знак минус
#define  SYMBOL_DOT2   2 // точка

#define  SYMBOL_GRADUS   6 // знак градус
#define  SYMBOL_PERCENT  5 //знак процента
#define  SYMBOL_MM1  4  // знак буква м1
#define  SYMBOL_MM2  7 // знак буква м2

#define  SYMBOL_DOTSMALL  0 // точка разделитель

//определения состояний включено - выключено
#define  ON  1
#define  OFF 0

//определения байтов данных
#define  DATA_SYMBOLS   				byte1  //байт символов дисплея погодной станции
#define  DATA_TPH1   					byte2  //
#define  DATA_TPH2   					byte3
#define  DATA_CLOCK_MINUTES   	byte4  // байт минут дисплея часов
#define  DATA_CLOCK_HOURS   		byte5  //байт часов дисплея часов
#define  DATA_CLOCK_ANODES  		byte6  //байт состояний ламп часов-минут + разделительная точка

//определения  для режимов вывода информации на дисплей
#define  TEMP_P 				0
#define  TEMP_N 				1
#define  HUMIDITY  		2
#define  PRESSURE  		3
#define  TIME_SECONDS 	4
#define  TIME_SET 			5
#define  TIME_TDC 			6
#define  TIME_TDP 			7
#define  TIME_TDT 			8
#define  TIME_TDH 			9
#define  DAYMONTH 		10
#define  TIME_TDD 			11
#define  DEMO 				12
#define  ERROR_RF    		13


//биты ключей анодов ламп дисплея погодной станции
#define  IN14_PART 0x08
#define  IN14_EDIN 0x04
#define  IN14_DEC   0x02

//биты ключей анодов ламп дисплея часов
#define  M1     0x40
#define  M2     0x80
#define  H1     0x08
#define  H2      0x10
#define  DOT   0x20

//максимальный уровень яркости подсветки
#define    RGBLED_BRIGN  64
#define    RGBLED_BRIGN_DAY  128
#define    RGBLED_BRIGN_NIGHT 16
#define    RGBLED_BRIGN_MORNING 16

//часы работы подсветки

#define MORNING1  9
#define MORNING2  10
#define DAY1  11
#define DAY2  20
#define NIGHT1  20
#define NIGHT2  22
#define DEEPNIGHT1  0
#define DEEPNIGHT2  9




//адреса ячеек ПЗУ
#define   CLOCK_EEPROM  		 	0x2234
#define   PRESSURE_EEPROM    	0x2235
#define   TEMPER_EEPROM  		 	0x2236
#define   DAYMONTH_EEPROM  	0x2237
#define   HUMV_EEPROM    			0x2238



//время отображения информации на дисплее
#define TIME_BASE  100
#define KTD    1000 / TIME_BASE
#define TIME_DISPLAY_MAX 99


//--------------------------------------------------------------
#endif // __STM32F4_DEFINES_H
