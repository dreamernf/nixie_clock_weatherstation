//--------------------------------------------------------------
// Includes
//--------------------------------------------------------------
#include "functions.h"

uint8_t   byte1 = 0;
uint8_t   byte2 = 0;
uint8_t   byte3 = 0;
uint8_t   byte4 = 0;
uint8_t   byte5 = 0;
uint8_t   byte6 = 0;


uint8_t  convert_code_id1_in14(uint8_t data)
{
uint8_t temp = 0;
	switch (data) {
	case 0: temp = 9;
	break;
	case 1: temp =1;
	break;
	case 2: temp =3;
	break;
	case 3: temp = 11;
	break;
	case 4: temp = 10;
	break;
	case 5: temp = 2;
	break;
	case 6: temp = 12;
	break;
	case 7: temp =4;
	break;
	case 8: temp = 0;
	break;
	case 9: temp = 8;
	break;
	return temp;
}
}

uint8_t  convert_code_id1_in18(uint8_t data)
{
uint8_t temp = 0;
	switch (data) {
	case 0: temp = 3; ////
	break;
	case 1: temp =12; ////;
	break;
	case 2: temp =4;////
	break;
	case 3: temp = 8; ///
	break;
	case 4: temp = 2;/////
	break;
	case 5: temp = 10;//////
	break;
	case 6: temp = 9;/////
	break;
	case 7: temp =1; /////
	break;
	case 8: temp = 0;////
	break;
	case 9: temp = 11;///
	break;
	return temp;
}
}

uint8_t   bcd2int(uint8_t b)
{
	 return (b & 0x0f) + ((b >> 4) * 10);
}



uint8_t   int2bcd(uint8_t b)
{
	 return (b%10)  | ((b/10)<<4);
}


void symbol_minus (uint8_t status)
{
	if (status == ON)
	{
		DATA_SYMBOLS = DATA_SYMBOLS|=1<<SYMBOL_MINUS;
	}
	if (status == OFF)
	{
		DATA_SYMBOLS&=~(1<<SYMBOL_MINUS);
	}
	DATA_SYMBOLS&=~(1<<SYMBOL_PLUS);
	DATA_SYMBOLS&=~(1<<SYMBOL_DOT2);

}

void symbol_dot2 (uint8_t status)
{
	if (status == ON)
	{
		DATA_SYMBOLS = DATA_SYMBOLS|=1<<SYMBOL_DOT2;
	}
	if (status == OFF)
	{
		DATA_SYMBOLS&=~(1<<SYMBOL_DOT2);
	}
	DATA_SYMBOLS&=~(1<<SYMBOL_PLUS);
	DATA_SYMBOLS&=~(1<<SYMBOL_MINUS);
}


void symbol_plus (uint8_t status)
{
	if (status == ON)
	{
		DATA_SYMBOLS = DATA_SYMBOLS|=1<<SYMBOL_PLUS;
	}
	if (status == OFF)
	{
		DATA_SYMBOLS&=~(1<<SYMBOL_PLUS);
	}
	DATA_SYMBOLS&=~(1<<SYMBOL_MINUS);
	DATA_SYMBOLS&=~(1<<SYMBOL_DOT2);
}

void symbol_smalldot (uint8_t status)
{
	if (status == ON)
	{
		DATA_SYMBOLS = DATA_SYMBOLS|=1<<SYMBOL_DOTSMALL;
	}
	if (status == OFF)
	{
		DATA_SYMBOLS&=~(1<<SYMBOL_DOTSMALL);
	}
}

void symbols1_off (void)
{
	DATA_SYMBOLS&=~(1<<SYMBOL_PLUS);
	DATA_SYMBOLS&=~(1<<SYMBOL_MINUS);
	DATA_SYMBOLS&=~(1<<SYMBOL_DOT2);
}

void symbol_gradus (uint8_t status)
{
	if (status == ON)
	{
		DATA_SYMBOLS = DATA_SYMBOLS|=1<<SYMBOL_GRADUS;
	}
	if (status == OFF)
	{
		DATA_SYMBOLS&=~(1<<SYMBOL_GRADUS);
	}
	DATA_SYMBOLS&=~(1<<SYMBOL_PERCENT);
	DATA_SYMBOLS&=~(1<<SYMBOL_MM1);
	DATA_SYMBOLS&=~(1<<SYMBOL_MM2);
}

void symbol_percent (uint8_t status)
{
	if (status == ON)
	{
		DATA_SYMBOLS = DATA_SYMBOLS|=1<<SYMBOL_PERCENT;
	}
	if (status == OFF)
	{
		DATA_SYMBOLS&=~(1<<SYMBOL_PERCENT);
	}
	DATA_SYMBOLS&=~(1<<SYMBOL_GRADUS);
	DATA_SYMBOLS&=~(1<<SYMBOL_MM1);
	DATA_SYMBOLS&=~(1<<SYMBOL_MM2);
}

void symbol_mm (uint8_t status)
{
	if (status == ON)
	{
		DATA_SYMBOLS = DATA_SYMBOLS|=1<<SYMBOL_MM1;
		DATA_SYMBOLS = DATA_SYMBOLS|=1<<SYMBOL_MM2;
	}
	if (status == OFF)
	{
		DATA_SYMBOLS&=~(1<<SYMBOL_MM1);
		DATA_SYMBOLS&=~(1<<SYMBOL_MM2);
	}
	DATA_SYMBOLS&=~(1<<SYMBOL_PERCENT);
	DATA_SYMBOLS&=~(1<<SYMBOL_GRADUS);
}

void symbol_m1 (uint8_t status)
{
	if (status == ON)
	{
		DATA_SYMBOLS = DATA_SYMBOLS|=1<<SYMBOL_MM1;
	}
	if (status == OFF)
	{
		DATA_SYMBOLS&=~(1<<SYMBOL_MM1);
		DATA_SYMBOLS&=~(1<<SYMBOL_MM2);
	}
	DATA_SYMBOLS&=~(1<<SYMBOL_PERCENT);
	DATA_SYMBOLS&=~(1<<SYMBOL_GRADUS);
}

void symbol_m2 (uint8_t status)
{
	if (status == ON)
	{
		DATA_SYMBOLS = DATA_SYMBOLS|=1<<SYMBOL_MM2;
	}
	if (status == OFF)
	{
		DATA_SYMBOLS&=~(1<<SYMBOL_MM1);
		DATA_SYMBOLS&=~(1<<SYMBOL_MM2);
	}
	DATA_SYMBOLS&=~(1<<SYMBOL_PERCENT);
	DATA_SYMBOLS&=~(1<<SYMBOL_GRADUS);
}

void symbols2_off (void)
{
	DATA_SYMBOLS&=~(1<<SYMBOL_PERCENT);
	DATA_SYMBOLS&=~(1<<SYMBOL_GRADUS);
	DATA_SYMBOLS&=~(1<<SYMBOL_MM2);
	DATA_SYMBOLS&=~(1<<SYMBOL_MM1);
}


void process_data_for_display_tph(uint8_t  type_of_ind , uint16_t data)
{
	uint8_t tmp = 0;
	uint8_t status_in14 = 0;

	uint8_t tmp_sec = 0;

status_in14 = IN14_DEC + IN14_EDIN + IN14_PART;

if (type_of_ind == STATUS_RGB)
	{
	 DATA_TPH2 = 0;
	 symbols2_off();
	 symbol_minus(ON);
	 symbol_mm(ON);
	 //symbol_dot2(OFF);
	 symbol_smalldot(OFF);
	 status_in14 = IN14_PART;


    tmp = data%100;
    DATA_TPH1 = convert_code_id1_in14(tmp%10);
    DATA_TPH1  =DATA_TPH1  <<4;
    DATA_TPH1  =DATA_TPH1  + convert_code_id1_in14(data/100);
    DATA_TPH2 =  convert_code_id1_in14(tmp/10);
    DATA_TPH2 = DATA_TPH2 << 4;

    DATA_TPH2 =DATA_TPH2+ status_in14;
	}

if (type_of_ind == PRESSURE)
	{
		symbols1_off();
		symbol_mm(ON);
		symbol_smalldot(OFF);
		 status_in14 = IN14_DEC + IN14_EDIN + IN14_PART;
	}



if (type_of_ind == DEMO)
	{
		 status_in14 = IN14_DEC + IN14_EDIN + IN14_PART;
	}

if (type_of_ind == DAYMONTH)
	{
		symbols1_off();
		symbol_mm(OFF);
		symbol_smalldot(OFF);
		 status_in14 = 0;
	}

if (type_of_ind == HUMIDITY)
	{
		symbols1_off();
		symbol_percent(ON);
		symbol_smalldot(OFF);
		// status_in14 = IN14_DEC + IN14_EDIN + IN14_PART;
		 status_in14 =  IN14_EDIN + IN14_PART;
	}

if (type_of_ind == TEMP_P)
	{
		symbol_plus(ON);
		symbol_gradus(ON);
		symbol_smalldot(ON);
		 status_in14 = IN14_DEC + IN14_EDIN + IN14_PART;
	}

if (type_of_ind == TEMP_N)
	{
		symbol_minus(ON);
		symbol_gradus(ON);
		symbol_smalldot(ON);
		 status_in14 = IN14_DEC + IN14_EDIN + IN14_PART;
	}

if (type_of_ind == ERROR_RF)
	{
	    symbols1_off();
		symbol_gradus(ON);
		symbol_smalldot(OFF);
		 status_in14 = 0;
	}

    tmp = data%100;
    DATA_TPH1 = convert_code_id1_in14(tmp%10);
    DATA_TPH1  =DATA_TPH1  <<4;
    DATA_TPH1  =DATA_TPH1  + convert_code_id1_in14(data/100);
    DATA_TPH2 =  convert_code_id1_in14(tmp/10);
    DATA_TPH2 = DATA_TPH2 << 4;

/*
if (data < 99) // если есть ноль в сотнях  - то его не выводим
    {
    	status_in14 =  IN14_EDIN + IN14_PART;
    }

    if (data <9)  // если есть ноль в десятках и сотнях то эти ноли не выводим
    {
    	status_in14 = IN14_PART;
    }

    if (type_of_ind == TIME_SECONDS)
    {
    	status_in14 = IN14_DEC + IN14_EDIN;
    }
*/

    DATA_TPH2 =DATA_TPH2+ status_in14;

if (type_of_ind == TIME_SECONDS)
{
	DATA_TPH2 = 0;
	symbols2_off();
	symbol_smalldot(OFF);
	status_in14 = IN14_DEC + IN14_EDIN;


	tmp_sec = convert_code_id1_in14(data/10);

    DATA_TPH1 = tmp_sec;

    DATA_TPH2 =  convert_code_id1_in14(data%10);
    DATA_TPH2 = DATA_TPH2 << 4;

    DATA_TPH2 =DATA_TPH2+ status_in14;

}

if (type_of_ind == TIME_SET)
{
	DATA_TPH2 = 0;
	symbols2_off();
	symbol_dot2(OFF);
	symbol_smalldot(OFF);
	status_in14 = 0;


	tmp_sec = convert_code_id1_in14(data/10);

    DATA_TPH1 = tmp_sec;

    DATA_TPH2 =  convert_code_id1_in14(data%10);
    DATA_TPH2 = DATA_TPH2 << 4;

    DATA_TPH2 =DATA_TPH2+ status_in14;

}
if (type_of_ind == TIME_TDC)
{
	DATA_TPH2 = 0;
	IN18_Only_dot();
	symbols2_off();
	symbol_gradus(OFF);
	symbol_dot2(ON);
	symbol_smalldot(OFF);
	status_in14 = IN14_DEC + IN14_EDIN + IN14_PART;


    tmp = data%100;
    DATA_TPH1 = convert_code_id1_in14(tmp%10);
    DATA_TPH1  =DATA_TPH1  <<4;
    DATA_TPH1  =DATA_TPH1  + convert_code_id1_in14(data/100);
    DATA_TPH2 =  convert_code_id1_in14(tmp/10);
    DATA_TPH2 = DATA_TPH2 << 4;

    DATA_TPH2 =DATA_TPH2+ status_in14;
}

if (type_of_ind == TIME_TDP)
{
	DATA_TPH2 = 0;
	symbols2_off();
	symbol_m1(ON);
	symbol_m2(ON);
	symbol_dot2(OFF);
	symbol_smalldot(OFF);
	status_in14 = IN14_DEC + IN14_EDIN + IN14_PART;


    tmp = data%100;
    DATA_TPH1 = convert_code_id1_in14(tmp%10);
    DATA_TPH1  =DATA_TPH1  <<4;
    DATA_TPH1  =DATA_TPH1  + convert_code_id1_in14(data/100);
    DATA_TPH2 =  convert_code_id1_in14(tmp/10);
    DATA_TPH2 = DATA_TPH2 << 4;

    DATA_TPH2 =DATA_TPH2+ status_in14;

}

if (type_of_ind == TIME_TDT)
{
	DATA_TPH2 = 0;
	symbols2_off();
	symbol_m1(OFF);
	symbol_dot2(OFF);
	symbol_smalldot(OFF);
	symbol_gradus(ON);
	status_in14 = IN14_DEC + IN14_EDIN + IN14_PART;


    tmp = data%100;
    DATA_TPH1 = convert_code_id1_in14(tmp%10);
    DATA_TPH1  =DATA_TPH1  <<4;
    DATA_TPH1  =DATA_TPH1  + convert_code_id1_in14(data/100);
    DATA_TPH2 =  convert_code_id1_in14(tmp/10);
    DATA_TPH2 = DATA_TPH2 << 4;

    DATA_TPH2 =DATA_TPH2+ status_in14;

}

if (type_of_ind == TIME_TDD)
{
	DATA_TPH2 = 0;
	symbols2_off();
	symbol_m2(ON);
	symbol_dot2(ON);
	symbol_smalldot(OFF);
	status_in14 = IN14_DEC + IN14_EDIN + IN14_PART;


    tmp = data%100;
    DATA_TPH1 = convert_code_id1_in14(tmp%10);
    DATA_TPH1  =DATA_TPH1  <<4;
    DATA_TPH1  =DATA_TPH1  + convert_code_id1_in14(data/100);
    DATA_TPH2 =  convert_code_id1_in14(tmp/10);
    DATA_TPH2 = DATA_TPH2 << 4;

    DATA_TPH2 =DATA_TPH2+ status_in14;
}


if (type_of_ind == TIME_TDH)
{
	DATA_TPH2 = 0;
	symbols2_off();
	symbol_m1(OFF);
	symbol_percent(ON);
	symbol_dot2(OFF);
	symbol_smalldot(OFF);
	status_in14 = IN14_DEC + IN14_EDIN + IN14_PART;


    tmp = data%100;
    DATA_TPH1 = convert_code_id1_in14(tmp%10);
    DATA_TPH1  =DATA_TPH1  <<4;
    DATA_TPH1  =DATA_TPH1  + convert_code_id1_in14(data/100);
    DATA_TPH2 =  convert_code_id1_in14(tmp/10);
    DATA_TPH2 = DATA_TPH2 << 4;

    DATA_TPH2 =DATA_TPH2+ status_in14;

}



}

void process_data_for_display_clock(uint8_t  hh, uint8_t mm)
{
	uint8_t tmp_hour = 0;
	uint8_t tmp_min = 0;

    tmp_min =convert_code_id1_in18(mm%10);
	tmp_min =tmp_min <<4;
	tmp_min = tmp_min +convert_code_id1_in18(mm/10);

	tmp_hour =convert_code_id1_in18(hh%10);
	tmp_hour =tmp_hour <<4;
	tmp_hour = tmp_hour +convert_code_id1_in18(hh/10);

	DATA_CLOCK_MINUTES = tmp_min;
	DATA_CLOCK_HOURS = 	 tmp_hour;
}


void out_data_to_indicators (void)
{
	  P_NIXIE_REG_CS(Bit_RESET);
	  UB_SPI1_SendByte(DATA_SYMBOLS);
	  UB_SPI1_SendByte(DATA_TPH1);
	  UB_SPI1_SendByte(DATA_TPH2);
	  UB_SPI1_SendByte(DATA_CLOCK_MINUTES);
	  UB_SPI1_SendByte(DATA_CLOCK_HOURS);
	  UB_SPI1_SendByte(DATA_CLOCK_ANODES);
	  P_NIXIE_REG_CS(Bit_SET);
}

void  IN18_On_and_dot(void)
{
	DATA_CLOCK_ANODES  =H1 + H2 + DOT + M1 +M2;
}

void  IN18_Only_dot(void)
{
	DATA_CLOCK_ANODES  = DOT;
}

void  IN18_On_and_nodot(void)
{
	DATA_CLOCK_ANODES  =H1 + H2 + M1 +M2;
}

void  IN18_On_hour09(void)
{
	DATA_CLOCK_ANODES  = H2 + DOT + M1 +M2;
}

void  IN18_On_hour09_nodot(void)
{
	DATA_CLOCK_ANODES  = H2 + M1 +M2;
}

void  IN18_Off(void)
{
	DATA_CLOCK_ANODES  = 0x00;
}

void  IN18_On_Only_Minutes(void)
{
	DATA_CLOCK_ANODES  =H1 + H2 +DOT  ;
}

void  IN18_On_Only_EdMinutes(void)
{
	DATA_CLOCK_ANODES  =M1  ;
}

void  IN18_On_Only_Hours(void)
{
	DATA_CLOCK_ANODES  =M1 + M2 +DOT  ;
}


int what_day(uint8_t day , uint8_t month, uint8_t year)
{
    int a = (14 - month) / 12;
    int y = 2000+year - a;
    int m = month + 12 * a - 2;
    return (7000 + (day + y + y / 4 - y / 100 + y / 400 + (31 * m) / 12)) % 7;
}

void Delay(volatile uint32_t nCount) {
  while(nCount--) {
  }
}

void Beep() {
	  UB_Led_On(BEEPER);
	  vTaskDelay(100/portTICK_RATE_MS);
	  UB_Led_Off(BEEPER);
}



