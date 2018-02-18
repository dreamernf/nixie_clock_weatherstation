//--------------------------------------------------------------
// File     : stm32_ub_encoder_tim2.c
// Datum    : 21.07.2013
// Version  : 1.0
// Autor    : UB
// EMail    : mc-4u(@)t-online.de
// Web      : www.mikrocontroller-4u.de
// CPU      : STM32F4
// IDE      : CooCox CoIDE 1.7.0
// Module   : GPIO, TIM
// Funktion : Drehgeber Messung (Rotary Encoder) per Timer2
//
// Hinweis  : mцgliche Pinbelegungen
//            CH1 : [PA0, PA5]
//            CH2 : [PA1, PB3]
//
//--------------------------------------------------------------


//--------------------------------------------------------------
// Includes
//--------------------------------------------------------------
#include "stm32_ub_encoder_tim2.h"

volatile Direction captured_direction = FORWARD;



//--------------------------------------------------------------
// interne Funktionen
//--------------------------------------------------------------
void P_ENCODER_InitIO(void);
void P_ENCODER_InitTIM(ENC_TIM2_MODE_t mode, ENC_TIM2_TYP_t typ, uint16_t maxwert);


//--------------------------------------------------------------
// Definition der zwei Rotary-Encoder Pins
//
// Channel : [1 und 2]
//--------------------------------------------------------------
ENC_TIM2_t ENC_TIM2[] = {
  // Name   ,Channel, PORT , PIN       , CLOCK               , Source
  {ENC_T2_A ,1      ,GPIOA ,GPIO_Pin_0 ,RCC_AHB1Periph_GPIOA ,GPIO_PinSource0},
  {ENC_T2_B ,2      ,GPIOA ,GPIO_Pin_1 ,RCC_AHB1Periph_GPIOA ,GPIO_PinSource1},
};


//--------------------------------------------------------------
// init und start vom Encoder (Drehgeber-Messung) mit Timer2
//
// mode    : [ENC_MODE_2A, ENC_MODE_2B, ENC_MODE_4AB]
// typ     : [ENC_TYP_NORMAL, ENC_TYP_REVERS]
// maxwert : [0 bis 65535]
//-------------------------------------------------------------- 
void UB_ENCODER_TIM2_Init(ENC_TIM2_MODE_t mode, ENC_TIM2_TYP_t typ, uint16_t maxwert)
{
  // init der Funktionen
  P_ENCODER_InitIO();
  P_ENCODER_InitTIM(mode, typ, maxwert);
}


//--------------------------------------------------------------
// auslesen der aktuellen Position vom Drehgeber
//
// Return_Wert : [0 bis eingestelltem Maxwert]
//--------------------------------------------------------------
uint16_t UB_ENCODER_TIM2_ReadPos(void)
{
  uint16_t ret_wert=0;

  ret_wert=TIM_GetCounter(TIM2);


  return(ret_wert);
}

void UB_ENCODER_TIM2_WritePos(uint16_t data)
{

  TIM_SetCounter(TIM2,data);

}

Direction UB_ENCODER_TIM2_GetDirection(void)
{
 // uint8_t  captured_direction=0;

  /* В бите TIM_CR1_DIR регистра TIM3_CR1 хранится
       направление вращения энкодера, запоминаем его. */
    captured_direction = (TIM2->CR1 & TIM_CR1_DIR ? FORWARD : BACKWARD);

  return(captured_direction);
}



//--------------------------------------------------------------
// interne Funktion
// Init aller IO-Pins (mit PullUp)
//--------------------------------------------------------------
void P_ENCODER_InitIO(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  ENC_TIM2_NAME_t vname;

  for(vname=0;vname<2;vname++) {
    // Clock Enable
    RCC_AHB1PeriphClockCmd(ENC_TIM2[vname].ENC_CLK, ENABLE);

    // Config des Pins als AF-Input
    GPIO_InitStructure.GPIO_Pin = ENC_TIM2[vname].ENC_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_Init(ENC_TIM2[vname].ENC_PORT, &GPIO_InitStructure);

    // Alternative-Funktion mit dem IO-Pin verbinden
    GPIO_PinAFConfig(ENC_TIM2[vname].ENC_PORT, ENC_TIM2[vname].ENC_SOURCE, GPIO_AF_TIM2);
  }
}



//--------------------------------------------------------------
// interne Funktion
// Init vom Timer fьr den Rotary-Encoder
//--------------------------------------------------------------
void P_ENCODER_InitTIM(ENC_TIM2_MODE_t mode, ENC_TIM2_TYP_t typ, uint16_t maxwert)
{
  uint16_t enc_mode, enc_edge;

  // Clock enable
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE); 

  // Encoder Mode
  if(mode==ENC_T2_MODE_2A) {
    enc_mode=TIM_EncoderMode_TI1;
  }
  else if(mode==ENC_T2_MODE_2B){
    enc_mode=TIM_EncoderMode_TI2;
  }
  else {
    enc_mode=TIM_EncoderMode_TI12;
  }

  // Encoder Typ
  if(typ==ENC_T2_TYP_NORMAL) {
    enc_edge=TIM_ICPolarity_Rising;
  }
  else {
    enc_edge=TIM_ICPolarity_Falling;
  }

  // Modus einstellen
  TIM_EncoderInterfaceConfig(TIM2, enc_mode, enc_edge, TIM_ICPolarity_Rising);

  // Maxwert setzen
  TIM_SetAutoreload(TIM2, maxwert);

  // Timer enable
  TIM_Cmd(TIM2, ENABLE);
}

