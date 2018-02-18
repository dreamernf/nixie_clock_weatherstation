//--------------------------------------------------------------
// File     : functions.h
//--------------------------------------------------------------

//--------------------------------------------------------------
#ifndef __STM32F4_FUNCTIONS_H
#define __STM32F4_FUNCTIONS_H

//--------------------------------------------------------------
// Includes
//--------------------------------------------------------------
#include "stm32f4xx.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_rcc.h"
#include "stm32_ub_spi1.h"
#include "stm32_ub_spi_send_mbyte.h"
#include "defines.h"


int16_t   enc_GetRelativeMove(void);
uint8_t   convert_code_id1_in14(uint8_t data);
uint8_t   convert_code_id1_in18(uint8_t data);
uint8_t   bcd2int(uint8_t b);
uint8_t   bcd2int(uint8_t b);

void symbol_minus (uint8_t status);
void symbol_dot2 (uint8_t status);
void symbol_plus (uint8_t status);
void symbol_smalldot (uint8_t status);
void symbols1_off (void);
void symbol_gradus (uint8_t status);
void symbol_percent (uint8_t status);
void symbol_mm (uint8_t status);
void symbol_m1 (uint8_t status);
void symbols2_off (void);
void process_data_for_display_tph(uint8_t  type_of_ind , uint16_t data);
void process_data_for_display_clock(uint8_t  hh, uint8_t mm);
void out_data_to_indicators (void);

void  IN18_On_and_dot(void);
void  IN18_Off(void);

void  IN18_On_Only_Minutes(void);
void  IN18_On_Only_Hours(void);


#endif // __STM32F4_UB_BUTTON_H
