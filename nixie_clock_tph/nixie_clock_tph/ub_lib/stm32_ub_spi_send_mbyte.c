//--------------------------------------------------------------
// File     : stm32_ub_spi_send_mbyte.c
// Datum    : 04.03.2013
// Version  : 1.0
// Autor    : UB
// EMail    : mc-4u(@)t-online.de
// Web      : www.mikrocontroller-4u.de
// CPU      : STM32F4
// IDE      : CooCox CoIDE 1.7.0
// Module   : GPIO, STM32_UB_SPI2
//
// Hinweis  : Settings :
//            SPI-Mode = 0, FRQ-Max = 10MHz
//            SPI2 [CLK=PB13, MOSI=PB15, MISO=PB14]
//            Chip-Select an PB12
//--------------------------------------------------------------

//--------------------------------------------------------------
// Includes
//--------------------------------------------------------------
#include "stm32_ub_spi_send_mbyte.h"
#include "stm32_ub_spi1.h"



//--------------------------------------------------------------
// interne Funktionen
//--------------------------------------------------------------
void P_NIXIE_REG_initIO(void);
void P_NIXIE_REG_CS(BitAction wert);


//--------------------------------------------------------------
// Initialisierung
// Return_wert :
//  -> ERROR   , wenn Initialisierung fehlgeschlagen
//  -> SUCCESS , wenn Initialisierung ok war 
//--------------------------------------------------------------
ErrorStatus NIXIE_REG_Init(void)
{
  ErrorStatus ret_wert=ERROR;
  // Init der Chip-Select-Leitung
  P_NIXIE_REG_initIO();
  // ChipSelect auf Hi
  P_NIXIE_REG_CS(Bit_SET);
  // init vom SPI-2 im Mode-0
  ret_wert=UB_SPI1_Init(SPI_MODE_0);

  return(ret_wert);
}


//--------------------------------------------------------------
// interne Funktion
// Init der ChipSelect-Leitung
//--------------------------------------------------------------
void P_NIXIE_REG_initIO(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;

  // Init der Chip-Select-Leitung
  RCC_AHB1PeriphClockCmd(NIXIE_REG_CS_GPIO_CLK,ENABLE);

  GPIO_InitStructure.GPIO_Pin = NIXIE_REG_CS_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(NIXIE_REG_CS_GPIO_PORT, &GPIO_InitStructure);
 
}


//--------------------------------------------------------------
// interne Funktion
// Pegel von ChipSelect einstellen
//--------------------------------------------------------------
void P_NIXIE_REG_CS(BitAction wert) {
  if(wert==Bit_RESET) {
	  NIXIE_REG_CS_GPIO_PORT->BSRRH =NIXIE_REG_CS_PIN;
  }
  else {
	  NIXIE_REG_CS_GPIO_PORT->BSRRL = NIXIE_REG_CS_PIN;
  }
}



