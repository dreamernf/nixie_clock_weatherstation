#include "main.h"
#include "defines.h"
#include "functions.h"
#include "stm32_ub_freertos.h"
#include "stm32_ub_led.h"
#include "stm32_ub_pwm_tim3.h"
#include "stm32_ub_i2c2.h"
#include "stm32_ub_spi1.h"
#include "stm32_ub_spi_send_mbyte.h"
#include "stm32_ub_button.h"
#include "tm_stm32f4_nrf24l01.h"
#include "eeprom.h"
#include "math.h"

/* Receiver address */
uint8_t TxAddress[] = {
    0xE7,
    0xE7,
    0xE7,
    0xE7,
    0xE7
};
/* My address */
uint8_t MyAddress[] = {
    0x7E,
    0x7E,
    0x7E,
    0x7E,
    0x7E
};

uint8_t dataOut[4], dataCOut[4], dataIn[4], dataCIn[4];


uint16_t counterwert = 0;
uint16_t k =0;

int32_t P = 0;
int16_t T = 0;
int16_t L = 0;
int16_t HUMV = 0;
uint16_t LUX=0;

int16_t SS = 0;


uint16_t  hum=0;

volatile int8_t napravlenie = 0;
volatile int8_t napravlenie1 = 0;
volatile int8_t napravlenie2 = 0;

uint8_t sec = 0;
uint8_t min = 0;
uint8_t hour = 0;



uint8_t day = 0;
uint8_t w_day = 0;
uint8_t month = 0;
uint8_t year = 0;

uint8_t  time_of_display = 0;
uint8_t  time_of_display_scan = 0;

uint8_t  global_time_of_display_c = 0;
uint8_t  global_time_of_display_p = 0;
uint8_t  global_time_of_display_t = 0;
uint8_t  global_time_of_display_d = 0;
uint8_t  global_time_of_display_h = 0;
uint8_t  global_time_of_display_t2 = 0;
uint8_t  global_display_rgbl_status = 0;


uint8_t flag_menu = 0;
uint8_t flag_one_read_m = 0;
uint8_t flag_one_read_h = 0;
uint8_t flag_one_read_tdc = 0;
uint8_t flag_one_read_tdp = 0;
uint8_t flag_one_read_tdt = 0;
uint8_t flag_one_read_tdd = 0;
uint8_t flag_one_read_d = 0;
uint8_t flag_one_read_mm = 0;
uint8_t flag_one_read_th = 0;

uint8_t flag_one_read_rgbl = 0;


uint8_t flag_rec_ok =0;

uint8_t flag_btn_plus =0;
uint8_t flag_btn_minus =0;


uint8_t  TC = 0;
uint8_t  TD = 0;
uint8_t  TT1 = 0;
uint8_t  TT2 = 0;
uint8_t  TP = 0;
uint8_t  TH = 0;


uint8_t flag_scan_inlamps = 0;


void IsWorkedTask( void *pvParameters );
void PWMTask( void *pvParameters );
void GetTP();
void GetTime_and_Date( void *pvParameters );
void GetTempOut( void *pvParameters );
void Out_Data_to_Nixie( void *pvParameters );
void ScanInput( void *pvParameters );
void Dots_Clock( void *pvParameters );


void hw_init(void)
{

  //инициализация железа
  UB_Button_Init();
  UB_Led_Init();
  UB_Button_OnClick(BTN_MENU);
  UB_I2C2_Init();
  NIXIE_REG_Init();
  UB_PWM_TIM3_Init();

  /* Initialize NRF24L01+ on channel 15 and 32bytes of payload */
  /* By default 2Mbps data rate and 0dBm output power */
  /* NRF24L01 goes to RX mode by default */
  TM_NRF24L01_Init(16, 4);

  /* Set RF settings, Data rate to 2Mbps, Output power to -18dBm */
  TM_NRF24L01_SetRF(TM_NRF24L01_DataRate_2M, TM_NRF24L01_OutputPower_M18dBm);

  /* Set my address, 5 bytes */
  TM_NRF24L01_SetMyAddress(MyAddress);
  /* Set TX address, 5 bytes */
  TM_NRF24L01_SetTxAddress(TxAddress);

 I2C2_DATA[0] = 0x01;
 UB_I2C2_WriteMultiByte(HTU21D, 0xE6, 1);


  //считывание настроек из ПЗУ
 global_time_of_display_c= Read_24Cxx(CLOCK_EEPROM,M24512);
 global_time_of_display_p= Read_24Cxx(PRESSURE_EEPROM,M24512);
 global_time_of_display_t= Read_24Cxx(TEMPER_EEPROM,M24512);
 global_time_of_display_d= Read_24Cxx( DAYMONTH_EEPROM,M24512);
 global_time_of_display_h= Read_24Cxx( HUMV_EEPROM ,M24512);
 global_display_rgbl_status = Read_24Cxx(STATUSLIGHT_EEPROM  ,M24512);
 global_time_of_display_t2 = global_time_of_display_t;

  TC=global_time_of_display_c;
  TP=TC+global_time_of_display_p;
  TT1=TP+global_time_of_display_t;
  TT2=TT1+global_time_of_display_t2;
  TH=TT2+global_time_of_display_h;
  TD=TH+ global_time_of_display_d;



  flag_scan_inlamps = 0;

  UB_Led_On(BEEPER);
  Delay(5000000);
  UB_Led_Off(BEEPER);

}

//--------------------------------------------------------------
int main(void)
{
  SystemInit();

  //инициализация железа
  hw_init();

  //создание тасков
  xTaskCreate(IsWorkedTask, ( signed char * ) "IsWorkedTask", configMINIMAL_STACK_SIZE, NULL, 1, NULL );
  xTaskCreate(PWMTask, ( signed char * ) "PWMTask", configMINIMAL_STACK_SIZE, NULL, 1, NULL );
  xTaskCreate(GetTempOut, ( signed char * ) "GetTempOut", configMINIMAL_STACK_SIZE, NULL, 1, NULL);
  xTaskCreate(Out_Data_to_Nixie, ( signed char * ) "Out_Data_to_Nixie", configMINIMAL_STACK_SIZE, NULL, 1, NULL);
  xTaskCreate(ScanInput, ( signed char * ) "ScanInput", configMINIMAL_STACK_SIZE, NULL, 1, NULL);
  xTaskCreate(GetTime_and_Date, ( signed char * ) "GetTime_and_Date", configMINIMAL_STACK_SIZE, NULL, 1, NULL);
  xTaskCreate(Dots_Clock, ( signed char * ) "Dots_Clock", configMINIMAL_STACK_SIZE, NULL, 1, NULL);

  //запустили планировщик
  vTaskStartScheduler();

  while(1)
  {

  }
}

void Dots_Clock( void *pvParameters )
  {
    while(1)
    {
    	 if ((flag_menu == NO_MENU) && (flag_scan_inlamps == 0))
    	      {
    	        if (time_of_display<global_time_of_display_c*KTD)
    			 {
    	    		  if (hour<10)
    	    		  {
    	    			  IN18_On_hour09();
    	    			  symbol_dot2(ON);
    	    		  }
    	    		  else
    	    	      	  {
    	    			     IN18_On_and_dot();
    	    			     symbol_dot2(ON);
    	    		      }

    				 vTaskDelay(360/portTICK_RATE_MS);

   	    		  if (hour<10)
   	    		  {
   	    			IN18_On_hour09_nodot();
   	    			 symbol_dot2(OFF);
   	    		  }
   	    		  else
   	    	      	  {
   	    			     IN18_On_and_nodot();
   	    			     symbol_dot2(OFF);
   	    		      }

					 vTaskDelay(360/portTICK_RATE_MS);

    			 }

              }
    }
  }

void GetTime_and_Date( void *pvParameters )
  {
	uint8_t  status_time = 0;
    while(1)
    {
	//считываем значение часов-даты
           	taskENTER_CRITICAL();
           	status_time = UB_I2C2_ReadMultiByte(DS3231_I2C_ADDR, DS3231_TIME_CAL_ADDR, 6);
       	 if (status_time == 0){
       		 sec = bcd2int(I2C2_DATA[0]);
       		 min = bcd2int(I2C2_DATA[1]);
       		 hour = bcd2int(I2C2_DATA[2]);
       		 w_day = bcd2int(I2C2_DATA[3]);
       		 day = bcd2int(I2C2_DATA[4]);
       		 month = bcd2int(I2C2_DATA[5]);
       		 year = bcd2int(I2C2_DATA[6]);
       	 }

	 taskEXIT_CRITICAL();
	 vTaskDelay(100/portTICK_RATE_MS);
    }
  }

void GetTempOut( void *pvParameters )
  {


	while(1)
    {
	 	taskENTER_CRITICAL();
	 	 /* If data is ready on NRF24L01+ */
	 	if ((TM_NRF24L01_DataReady()==1)) {
	 TM_NRF24L01_GetData(dataIn);
	 	     flag_rec_ok=0;
	 	    UB_Led_Off(LED_ORANGE);
	 	        }
	 	else
	 	{
	 	  	UB_Led_On(LED_ORANGE);
	 	 		 flag_rec_ok ++;
	 			 	if (flag_rec_ok>=5)
	 	 			 	{
	 	 			 		dataIn[0]=9;
	 	 			 		dataIn[1]=9;
	 	 			 		dataIn[2]=9;
	 	 			 		dataIn[3]=9;
	 	 			 		flag_rec_ok = 1;
	 	 			 	}

	 	}

        taskEXIT_CRITICAL();
	    vTaskDelay(5000/portTICK_RATE_MS);

    }
  }

//моргает светодиодом показывая, что система не зависла
void IsWorkedTask( void *pvParameters )
{

 while(1)
  {

	  UB_Led_Toggle(LED_RED);
	  time_of_display++;
      vTaskDelay(1000/portTICK_RATE_MS);
  }
}

//сканирует кнопки, определяет и обратывает нажатие
void ScanInput( void *pvParameters )
{
  while(1)
  {

	if (
		  (
			(hour == 0)
            ||
			(hour == 4)
			||
			(hour == 13)
			||
			(hour == 17)
			||
			(hour == 21)
		  )
			&&
			(
					(min == 10)
					||
					(min == 20)
					||
					(min == 35)
					||
					(min == 50)
			)
		)
	{
		flag_scan_inlamps = 1;
	}
	else
	{
		flag_scan_inlamps = 0;
	}


   if (UB_Button_OnClick(BTN_MENU)==true)  {
	    Beep();
	   	  //если находимся не в меню
	      if	(flag_menu == NO_MENU)
	      {
	    	  //установить пункт меню - Настройка времени - минут
	    	  flag_menu = MENU_SET_MINUTE;
	      }
	      else
	      {
   	    	 //если другой пункт меню - то перейти на след пункт
	    	 flag_menu = flag_menu +1;

	    	 //и сбросить флаги одноразовых чтений параметров
   	    	 flag_one_read_m = 0;
   	    	 flag_one_read_h = 0;
   	    	 flag_one_read_tdc = 0;
   	    	 flag_one_read_tdp = 0;
   	    	 flag_one_read_tdt = 0;
   	    	 flag_one_read_tdd = 0;
   	    	 flag_one_read_d = 0;
   	    	 flag_one_read_mm = 0;
   	    	 flag_one_read_rgbl = 0;
   	      }

	      //если находимся в последнем пункте меню
	      if  (flag_menu == MENU_LAST)
	      {
	    	  //выйти из меню
	    	  flag_menu = NO_MENU;
	      }
      }

if ((UB_Button_OnClick(BTN_PLUS)==true))
   {
		flag_btn_plus =1;
		Beep();
   }

if (UB_Button_OnClick(BTN_MINUS)==true)
   {
	   flag_btn_minus =1;
	   Beep();
   }

    vTaskDelay(2/portTICK_RATE_MS);
  }
}

//таск управления RGB подстветкой
void PWMTask( void *pvParameters )
{
	uint8_t step = 0;
	uint8_t  r,g,b;
  while(1)
  {


	  if	(flag_menu != NO_MENU)
		      {
					UB_PWM_TIM3_SetPWM(PWM_T3_PB4_BLUE,0);
					UB_PWM_TIM3_SetPWM(PWM_T3_PB5_RED,RGBLED_BRIGN);
					UB_PWM_TIM3_SetPWM(PWM_T3_PB0_GREEN,0);

		      } else
		         {

	  if ( global_display_rgbl_status == 1 )
	  {

	  if ((hour>=DAY1)&&(hour<DAY2))
	  {
		  if(step==0)
			       {
			          //  step = rand() % 6 + 1;
			         step=1;
			       }
			 if(step==1)
			       {
			          r++;
			          g=0;
			          b=0;
			             if(r>=RGBLED_BRIGN_DAY)
			                       {
			            	          step=2;
			                      }
			        }
			 if(step==2)
			        {
			           g++;
			              if(g>=RGBLED_BRIGN_DAY)
			               {
			            	  step=3;
			               }
			        }
			 if(step==3)
			        {
			            r--;
			              if(r<=0)
			                 {
			            	  step=4;
			                 }
			        }
			 if(step==4)
			        {
			           b++;
			             if(b>=RGBLED_BRIGN_DAY)
			                 {
			            	    step=5;
			                 }
			        }
			 if(step==5)
			        {
			           g--;
			              if(g<=0)
			                 {
			            	   step=6;
			                 }
			        }
			 if(step==6)
			        {
			               r++;
			                  if(r>=RGBLED_BRIGN_DAY)
			                      {
			                	     step=7;
			                      }
			        }
			 if(step==7)
			        {
			             b--;
			                if(b<=0)
			                    {
			                	   step=0;
			                    }
			        }

			    UB_PWM_TIM3_SetPWM(PWM_T3_PB4_BLUE,b);
			    UB_PWM_TIM3_SetPWM(PWM_T3_PB5_RED,r);
			    UB_PWM_TIM3_SetPWM(PWM_T3_PB0_GREEN,g);

			    vTaskDelay(30/portTICK_RATE_MS);


	  }

	  if ((hour>=NIGHT1)&&(hour<=NIGHT2))
	  {
		  if(step==0)
					       {
					          //  step = rand() % 6 + 1;
					         step=1;
					       }
					 if(step==1)
					       {
					          r++;
					          g=0;
					          b=0;
					             if(r>=RGBLED_BRIGN_NIGHT)
					                       {
					            	          step=2;
					                      }
					        }
					 if(step==2)
					        {
					           g++;
					              if(g>=RGBLED_BRIGN_NIGHT)
					               {
					            	  step=3;
					               }
					        }
					 if(step==3)
					        {
					            r--;
					              if(r<=0)
					                 {
					            	  step=4;
					                 }
					        }
					 if(step==4)
					        {
					           b++;
					             if(b>=RGBLED_BRIGN_NIGHT)
					                 {
					            	    step=5;
					                 }
					        }
					 if(step==5)
					        {
					           g--;
					              if(g<=0)
					                 {
					            	   step=6;
					                 }
					        }
					 if(step==6)
					        {
					               r++;
					                  if(r>=RGBLED_BRIGN_NIGHT)
					                      {
					                	     step=7;
					                      }
					        }
					 if(step==7)
					        {
					             b--;
					                if(b<=0)
					                    {
					                	   step=0;
					                    }
					        }

					    UB_PWM_TIM3_SetPWM(PWM_T3_PB4_BLUE,b);
					    UB_PWM_TIM3_SetPWM(PWM_T3_PB5_RED,r);
					    UB_PWM_TIM3_SetPWM(PWM_T3_PB0_GREEN,g);

					    vTaskDelay(40/portTICK_RATE_MS);
	  }

	  if ((hour>= DEEPNIGHT1)&&(hour< DEEPNIGHT2))
	  {

		 UB_PWM_TIM3_SetPWM(PWM_T3_PB4_BLUE,0);
		 UB_PWM_TIM3_SetPWM(PWM_T3_PB5_RED,0);
		 UB_PWM_TIM3_SetPWM(PWM_T3_PB0_GREEN,0);
	  }

	  if ((hour>=MORNING1)&&(hour<MORNING2))
	  {
		  if(step==0)
							       {
							          //  step = rand() % 6 + 1;
							         step=1;
							       }
							 if(step==1)
							       {
							          r++;
							          g=0;
							          b=0;
							             if(r>=RGBLED_BRIGN_MORNING)
							                       {
							            	          step=2;
							                      }
							        }
							 if(step==2)
							        {
							           g++;
							              if(g>=RGBLED_BRIGN_MORNING)
							               {
							            	  step=3;
							               }
							        }
							 if(step==3)
							        {
							            r--;
							              if(r<=0)
							                 {
							            	  step=4;
							                 }
							        }
							 if(step==4)
							        {
							           b++;
							             if(b>=RGBLED_BRIGN_MORNING)
							                 {
							            	    step=5;
							                 }
							        }
							 if(step==5)
							        {
							           g--;
							              if(g<=0)
							                 {
							            	   step=6;
							                 }
							        }
							 if(step==6)
							        {
							               r++;
							                  if(r>=RGBLED_BRIGN_MORNING)
							                      {
							                	     step=7;
							                      }
							        }
							 if(step==7)
							        {
							             b--;
							                if(b<=0)
							                    {
							                	   step=0;
							                    }
							        }

							    UB_PWM_TIM3_SetPWM(PWM_T3_PB4_BLUE,b);
							    UB_PWM_TIM3_SetPWM(PWM_T3_PB5_RED,r);
							    UB_PWM_TIM3_SetPWM(PWM_T3_PB0_GREEN,g);

							    vTaskDelay(40/portTICK_RATE_MS);
	             }
	  }
	  else
	  {
		  UB_PWM_TIM3_SetPWM(PWM_T3_PB4_BLUE,0);
		  UB_PWM_TIM3_SetPWM(PWM_T3_PB5_RED,0);
		  UB_PWM_TIM3_SetPWM(PWM_T3_PB0_GREEN,0);
	  }
  }

	    vTaskDelay(3/portTICK_RATE_MS);
   }
  }


//таск опроса датчиков давления, температуры, влажности, часов
void GetTP()
  {
	uint16_t rawHumidity = 0;
  	///опрос DMP180
    	int16_t AC1 = 0, AC2 = 0, AC3 = 0;
    	uint16_t AC4 = 0, AC5 = 0, AC6 = 0;
    	int16_t B1 = 0, B2 = 0, MB = 0, MC = 0, MD = 0;

    	 	 	taskENTER_CRITICAL();
    			AC1 =UB_I2C2_ReadByte(BMP085,0xAA)*256 + UB_I2C2_ReadByte(BMP085,0xAB);
            	AC2 = UB_I2C2_ReadByte(BMP085,0xAC)*256 + UB_I2C2_ReadByte(BMP085,0xAD);
            	AC3 = UB_I2C2_ReadByte(BMP085,0xAE)*256 + UB_I2C2_ReadByte(BMP085,0xAF);
            	AC4 = UB_I2C2_ReadByte(BMP085,0xB0)*256 + UB_I2C2_ReadByte(BMP085,0xB1);
            	AC5 = UB_I2C2_ReadByte(BMP085,0xB2)*256 + UB_I2C2_ReadByte(BMP085,0xB3);
            	AC6 = UB_I2C2_ReadByte(BMP085,0xB4)*256 + UB_I2C2_ReadByte(BMP085,0xB5);
            	 B1 = UB_I2C2_ReadByte(BMP085,0xB6)*256 + UB_I2C2_ReadByte(BMP085,0xB7);
            	 B2 = UB_I2C2_ReadByte(BMP085,0xB8)*256 +UB_I2C2_ReadByte(BMP085,0xB9);
            	 MB =UB_I2C2_ReadByte(BMP085,0xBA)*256 +UB_I2C2_ReadByte(BMP085,0xBB);
            	 MC =UB_I2C2_ReadByte(BMP085,0xBC)*256 + UB_I2C2_ReadByte(BMP085,0xBD);
            	 MD = UB_I2C2_ReadByte(BMP085,0xBE)*256 + UB_I2C2_ReadByte(BMP085,0xBF);
            	 taskEXIT_CRITICAL();


            	 taskENTER_CRITICAL();
            	 //Start temperature measurement
            	 UB_I2C2_WriteByte(BMP085,0xF4, 0x2E);
            	 taskEXIT_CRITICAL();
           	     vTaskDelay(5/portTICK_RATE_MS);
           	     //Read uncompensated temperature value
           	     taskENTER_CRITICAL();
           	     uint32_t UT = UB_I2C2_ReadByte(BMP085,0xF6)*256 + UB_I2C2_ReadByte(BMP085,0xF7);
           	     taskEXIT_CRITICAL();

           	     taskENTER_CRITICAL();
           	     //Start pressure measurement
           	     UB_I2C2_WriteByte(BMP085,0xF4, 0x34);
           	     taskEXIT_CRITICAL();
           	     vTaskDelay(5/portTICK_RATE_MS);
           	     taskENTER_CRITICAL();
           	     //Read uncompensated pressure value
           	     uint32_t UP = UB_I2C2_ReadByte(BMP085,0xF6)*256 + UB_I2C2_ReadByte(BMP085,0xF7);
           	     taskEXIT_CRITICAL();

           	//temparature compensation
           	int32_t X1 = (UT - AC6) * AC5 / 32768;
           	int16_t X2 = MC * 2048 / (X1 + MD);
           	int16_t B5 = X1 + X2;
           	T = (B5 + 8) / 16;

           	//pressure compensation
           	int16_t B6 = B5 - 4000;
           	int16_t B3 = ((AC1*4 + (B2*B6 / 4096 + AC2)*B6 / 2048) + 2) / 4;
           	X1 = (AC3 + 2*B1*B6)*B6 / 8192 / 65536 + 2;
           	uint16_t B4 = AC4*(unsigned long)(X1/4 + 32768) / 32768;
           	uint32_t B7 = ((unsigned long)UP - B3) * 50000;
           	P = (B7<0x8000000 ? B7*2/B4 : B7/B4*2);
           	P = P + ((3038*(P/256)*(P/256) - 7357*P) / 65536 + 3791) / 16;
           	P = P*75/10000;


       	     taskENTER_CRITICAL();
       	     UB_I2C2_ReadMultiByte(HTU21D, 0xE5, 3);
       	     taskEXIT_CRITICAL();

       	  rawHumidity = I2C2_DATA[0];
       	  rawHumidity = rawHumidity << 8;
       	  rawHumidity = rawHumidity +  I2C2_DATA[1] ;

       	    //sensorStatus = rawHumidity & 0x0003; //Grab only the right two bits
       		rawHumidity &= 0xFFFC; //Zero out the status bits but keep them in place

       		//Given the raw humidity data, calculate the actual relative humidity
       		float tempRH = rawHumidity / (float) 65536; //2^16 = 65536
       		float tempRH1 =(float) -6 + ((float)125 * (float) tempRH); //From page 14

       		//HUMV =(float)tempRH1 * 10;
       		HUMV =(float)tempRH1;

    }


//таск расчета и вывода данных на индикаторы, индикация меню
void Out_Data_to_Nixie( void *pvParameters )
  {
	int8_t tmp_hour = 0;
	int8_t tmp_min = 0;
	int8_t tmp_tdc = 0;
	int8_t tmp_tdp = 0;
	int8_t tmp_tdt = 0;
	int8_t tmp_tdd = 0;
	int8_t tmp_th = 0;
	int8_t tmp_day = 0;
	int8_t tmp_mm = 0;

	uint8_t tmp_rgbl = 0;

	uint8_t byte_eeprom = 0;
	uint8_t tt = 0;

	while(1)
    {
		if ((flag_menu == NO_MENU) && (flag_scan_inlamps == 1))
		     {
			time_of_display_scan++;
			   IN18_On_and_dot();
			     for (tt=0; tt<=9;tt++)
			       {
				       if ((tt == 0)||(tt == 3)||(tt == 6))
				       {
				    		symbol_gradus(ON);
				    		 symbol_dot2(ON);
				       }

				       if ((tt == 1)||(tt == 4)||(tt == 7))
				       {
				    	   symbol_percent(ON);
				    	   symbol_minus(ON);
				       }

				       if ((tt == 2)||(tt == 5)||(tt == 8))
				       {
				    	   symbol_mm(ON);
				    	   symbol_plus(ON);
				    	   IN18_On_and_dot();
				       }
			    		symbol_smalldot(ON);

			    		process_data_for_display_clock(tt*11,tt*11);
			    		process_data_for_display_tph(DEMO,tt*111);
			    		out_data_to_indicators ();
			    		vTaskDelay(80/portTICK_RATE_MS);

			    		symbols2_off();
			    		symbol_gradus(OFF);
			    		symbol_dot2(OFF);
			    		IN18_On_and_nodot();
			    		symbol_mm(OFF);
			    		symbol_smalldot(OFF);

			    		vTaskDelay(200/portTICK_RATE_MS);

			       }
		      }

   if ((flag_menu == NO_MENU) && (flag_scan_inlamps == 0))
      {
	   GetTP();
	   	   	   	   if (time_of_display<TC)
								{
								   process_data_for_display_tph(TIME_SECONDS,sec);
								   process_data_for_display_clock(hour,min);
								   out_data_to_indicators ();
								}

					if ((time_of_display>TC) &&(time_of_display<TP))
								{

									IN18_Off();
									process_data_for_display_tph(PRESSURE,P);
									out_data_to_indicators ();
							}

					if ((time_of_display>TP) &&(time_of_display<TT1))
								{

									IN18_On_Only_EdMinutes();
									process_data_for_display_clock(0,10);
									process_data_for_display_tph(TEMP_P,T);
									out_data_to_indicators ();
								}

					if ((time_of_display>TT1) &&(time_of_display<TT2))
								{

											IN18_On_Only_EdMinutes();
											process_data_for_display_clock(0,20);
											SS = dataIn[0]+10*dataIn[1]+100*dataIn[2];
											if (dataIn[3]==9)
											{
												process_data_for_display_tph(ERROR_RF,SS);
												out_data_to_indicators ();

											}
											if (dataIn[3]=='+')
											{
												process_data_for_display_tph(TEMP_P,SS);
												out_data_to_indicators ();
											}
											if (dataIn[3]=='-')
											{
												process_data_for_display_tph(TEMP_N,SS);
												out_data_to_indicators ();
											}
								}

						if ((time_of_display>TT2) &&	(time_of_display<TH))
						{
											IN18_On_Only_EdMinutes();
											process_data_for_display_clock(0,10);
											process_data_for_display_tph(HUMIDITY,HUMV);
											out_data_to_indicators ();
							}

						if ((time_of_display>TH) &&(time_of_display<TD))
						{
							IN18_On_and_dot();
							process_data_for_display_clock(day,month);
							process_data_for_display_tph(DAYMONTH,0);
							out_data_to_indicators ();
							}

						if (time_of_display>TD)
									{
										time_of_display = 0;
									}

						vTaskDelay(50/portTICK_RATE_MS);
    }

if (flag_menu == MENU_SET_MINUTE)
	{
	  	  	  if (flag_one_read_m == 0)
	  	  	  	  {
	  	  		  	  tmp_min = min;
	  	  		  	  flag_one_read_m = 1;
	  	  	  	  }

		 if (flag_btn_plus ==1)
	  	  	  	  {
	  	  		  	  tmp_min++;
	  	  		  	  	  if (tmp_min>59)
	  	  		  	  	     {
	  	  		  	  		  	  tmp_min=0;
	  	  		  	  	      }
	  	  		  	flag_btn_plus =0;
	  	  	  	  }
		 if (flag_btn_minus ==1)
	  	  	 	 {
	  	  		 	 tmp_min--;
	  	  		 	 	 if (tmp_min<0)
	  	  		 	 	 	 {
	  	  		 	 		 	 tmp_min=59;
	  	  		 	 	 	 }
	  	  		 	flag_btn_minus =0;
	  	  	 	 }

	  	  	 	 IN18_On_Only_Minutes();
	  	  	 	 process_data_for_display_tph(TIME_SET,sec);
	  	  	 	 process_data_for_display_clock(hour,tmp_min);
	  	  	 	 out_data_to_indicators ();

	  	  	 vTaskDelay(100/portTICK_RATE_MS);

	  	  	 	 IN18_On_and_dot();
	  	  	 	 process_data_for_display_tph(TIME_SET,sec);
	  	  	 	 process_data_for_display_clock(hour,tmp_min);
	  	  	 	 out_data_to_indicators ();

	  	  	 if (flag_btn_plus ==1)
	  	  	  	  	  	  {
	  	  	  	  		  	  tmp_min++;
	  	  	  	  		  	  	  if (tmp_min>59)
	  	  	  	  		  	  	     {
	  	  	  	  		  	  		  	  tmp_min=0;
	  	  	  	  		  	  	      }
	  	  	  	  		  flag_btn_plus =0;
	  	  	  	  	  	  }
	  	   if (flag_btn_minus ==1)
	  	  	  	  	 	 {
	  	  	  	  		 	 tmp_min--;
	  	  	  	  		 	 	 if (tmp_min<0)
	  	  	  	  		 	 	 	 {
	  	  	  	  		 	 		 	 tmp_min=59;
	  	  	  	  		 	 	 	 }
	  	  	  	  		 flag_btn_minus ==0;
	  	  	  	  	 	 }


	  	  	 	 I2C2_DATA[0] = int2bcd(30);
	  	  	 	 I2C2_DATA[1] = int2bcd(tmp_min);
	  	  	 	 I2C2_DATA[2] = int2bcd(hour);

	  	  	 	 taskENTER_CRITICAL();
	  	  	 	 UB_I2C2_WriteMultiByte(DS3231_I2C_ADDR,DS3231_TIME_CAL_ADDR, 3);
	  	  	 	 taskEXIT_CRITICAL();

	  	  	 vTaskDelay(100/portTICK_RATE_MS);
      	  }
if (flag_menu == MENU_SET_HOUR)
	{
					if (flag_one_read_h == 0)
						{
							tmp_hour = hour;
							flag_one_read_h = 1;
						}

					 if (flag_btn_plus ==1)
						{
							tmp_hour++;
								if (tmp_hour>23)
									{
										tmp_hour=0;
									}
								flag_btn_plus ==0;
						}
					 if (flag_btn_minus ==1)
						{
							tmp_hour--;
								if (tmp_hour<0)
								{
									tmp_hour=23;
								}
							flag_btn_minus =0;
						}

						IN18_On_Only_Hours();
						process_data_for_display_tph(TIME_SET,sec);
						process_data_for_display_clock(tmp_hour,min);
						out_data_to_indicators ();

						 vTaskDelay(100/portTICK_RATE_MS);

						IN18_On_and_dot();
						process_data_for_display_tph(TIME_SET,sec);
						process_data_for_display_clock(tmp_hour, min);
						out_data_to_indicators ();

						 if (flag_btn_plus ==1)
										{
											tmp_hour++;
												if (tmp_hour>23)
													{
														tmp_hour=0;
													}
												flag_btn_plus =0;
										}
						 if (flag_btn_minus ==1)
										{
											tmp_hour--;
												if (tmp_hour<0)
												{
													tmp_hour=23;
												}
											flag_btn_minus =0;
										}


						I2C2_DATA[0] = int2bcd(30);
						I2C2_DATA[1] = int2bcd(min);
						I2C2_DATA[2] = int2bcd(tmp_hour);

						taskENTER_CRITICAL();
						UB_I2C2_WriteMultiByte(DS3231_I2C_ADDR,DS3231_TIME_CAL_ADDR, 3);
						taskEXIT_CRITICAL();

						 vTaskDelay(100/portTICK_RATE_MS);

		}

if (flag_menu == MENU_SET_DAY)
	{
					if (flag_one_read_d == 0)
						{
							tmp_day = day;
							flag_one_read_d = 1;
						}

					 if (flag_btn_plus ==1)
						{
							tmp_day++;
								if (tmp_day>31)
									{
										tmp_day=1;
									}
							flag_btn_plus =0;
						}
					 if (flag_btn_minus ==1)
						{
							tmp_day--;
								if (tmp_day<1)
								{
									tmp_day=31;
								}
							flag_btn_minus =0;
						}

						IN18_On_Only_Hours();
						process_data_for_display_tph(DAYMONTH,0);
						process_data_for_display_clock(tmp_day,month);
						out_data_to_indicators ();

						 vTaskDelay(100/portTICK_RATE_MS);

						IN18_On_and_dot();
						process_data_for_display_tph(DAYMONTH,0);
						process_data_for_display_clock(tmp_day, month);
						out_data_to_indicators ();

						 if (flag_btn_plus ==1)
										{
											tmp_day++;
												if (tmp_day>31)
													{
														tmp_day=1;
													}
											flag_btn_plus =0;
										}
						 if (flag_btn_minus ==1)
										{
											tmp_day--;
												if (tmp_day<1)
												{
													tmp_day=31;
												}
											flag_btn_minus =0;
										}


						I2C2_DATA[0] = what_day(tmp_day , month, year);
						I2C2_DATA[1] = int2bcd(tmp_day);
						I2C2_DATA[2] = int2bcd(month);
						I2C2_DATA[3] = int2bcd(year);


						taskENTER_CRITICAL();
						UB_I2C2_WriteMultiByte(DS3231_I2C_ADDR,DS3231_CAL_ADDR, 4);
						taskEXIT_CRITICAL();

						 vTaskDelay(100/portTICK_RATE_MS);

		}

if (flag_menu == MENU_SET_MONTH)
	{
	  	  	  if (flag_one_read_mm == 0)
	  	  	  	  {
	  	  		  	  tmp_mm = month;
	  	  		  	  flag_one_read_mm = 1;
	  	  	  	  }

	  	  	 if (flag_btn_plus ==1)
	  	  	  	  {
	  	  		  	  tmp_mm++;
	  	  		  	  	  if (tmp_mm>12)
	  	  		  	  	     {
	  	  		  	  		  	  tmp_mm=1;
	  	  		  	  	      }
	  	  		  	flag_btn_plus =0;
	  	  	  	  }
	  	   if (flag_btn_minus ==1)
	  	  	 	 {
	  	  		 	 tmp_mm--;
	  	  		 	 	 if (tmp_mm<0)
	  	  		 	 	 	 {
	  	  		 	 		 	 tmp_mm=12;
	  	  		 	 	 	 }
	  	  		 	flag_btn_minus =0;
	  	  	 	 }

	  	  	 	 IN18_On_Only_Minutes();
	  	  	 process_data_for_display_tph(DAYMONTH,0);
	  	  	 	 process_data_for_display_clock(day,tmp_mm);
	  	  	 	 out_data_to_indicators ();

	  	  	 vTaskDelay(100/portTICK_RATE_MS);

	  	  	 	 IN18_On_and_dot();
	  	  	 process_data_for_display_tph(DAYMONTH,0);
	  	  	 	 process_data_for_display_clock(day,tmp_mm);
	  	  	 	 out_data_to_indicators ();

	  	  	 if (flag_btn_plus ==1)
	  	  	  	  	  	  {
	  	  	  	  		  	  tmp_mm++;
	  	  	  	  		  	  	  if (tmp_mm>12)
	  	  	  	  		  	  	     {
	  	  	  	  		  	  		  	  tmp_mm=1;
	  	  	  	  		  	  	      }
	  	  	  	  		    flag_btn_plus =0;
	  	  	  	  	  	  }
	  	   if (flag_btn_minus ==1)
	  	  	  	  	 	 {
	  	  	  	  		 	 tmp_mm--;
	  	  	  	  		 	 	 if (tmp_mm<0)
	  	  	  	  		 	 	 	 {
	  	  	  	  		 	 		 	 tmp_mm=12;
	  	  	  	  		 	 	 	 }
	  	  	  	  		    flag_btn_minus =0;
	  	  	  	  	 	 }


						I2C2_DATA[0] = what_day(day , tmp_mm, year);
							I2C2_DATA[1] = int2bcd(day);
							I2C2_DATA[2] = int2bcd(tmp_mm);
							I2C2_DATA[3] = int2bcd(year);


							taskENTER_CRITICAL();
							UB_I2C2_WriteMultiByte(DS3231_I2C_ADDR,DS3231_CAL_ADDR, 4);
							taskEXIT_CRITICAL();

							 vTaskDelay(100/portTICK_RATE_MS);
      	  }

if (flag_menu == MENU_SET_TIME_DISPLAY_CLOCK)
	{

						IN18_Off();
						if (flag_one_read_tdc == 0)
							{
								taskENTER_CRITICAL();
								tmp_tdc = Read_24Cxx(CLOCK_EEPROM,M24512);
								taskEXIT_CRITICAL();
								flag_one_read_tdc = 1;
							}

						 if (flag_btn_plus ==1)
							{
								tmp_tdc++;
									if (tmp_tdc>TIME_DISPLAY_MAX)
										{
											tmp_tdc=0;
										}
									flag_btn_plus =0;
							}
						 if (flag_btn_minus ==1)
											{
												tmp_tdc--;
												if (tmp_tdc<0){
												tmp_tdc=TIME_DISPLAY_MAX;
												}
												flag_btn_minus =0;
											}

						   global_time_of_display_c = tmp_tdc;

						   taskENTER_CRITICAL();
						   Write_24Cxx(CLOCK_EEPROM,tmp_tdc,M24512);
						   taskEXIT_CRITICAL();

						   process_data_for_display_tph(TIME_TDC,tmp_tdc);
						   out_data_to_indicators ();
						   vTaskDelay(100/portTICK_RATE_MS);
}
if (flag_menu ==  MENU_SET_TIME_DISPLAY_PRESSURE)
	{

			IN18_Off();
				if (flag_one_read_tdp == 0)
					{
						taskENTER_CRITICAL();
						tmp_tdp = Read_24Cxx(PRESSURE_EEPROM,M24512);
						taskEXIT_CRITICAL();
						flag_one_read_tdp = 1;
					}

				 if (flag_btn_plus ==1)
						{
							tmp_tdp++;
								if (tmp_tdp > TIME_DISPLAY_MAX)
									{
										tmp_tdp = 3;
									}
								flag_btn_plus =0;
						}
				 if (flag_btn_minus ==1)
						{
							tmp_tdp--;
								if (tmp_tdp <3)
								{
									tmp_tdp = TIME_DISPLAY_MAX;
								}
							flag_btn_minus =0;
						}


					global_time_of_display_p = tmp_tdp;

					taskENTER_CRITICAL();
					Write_24Cxx(PRESSURE_EEPROM,tmp_tdp,M24512);
					taskEXIT_CRITICAL();

					process_data_for_display_tph(TIME_TDP,tmp_tdp);
					out_data_to_indicators ();
					 vTaskDelay(100/portTICK_RATE_MS);
		}

if (flag_menu ==  MENU_SET_TIME_DISPLAY_TEMP)
	{

			IN18_Off();
				if (flag_one_read_tdt == 0)
					{
						taskENTER_CRITICAL();
						tmp_tdt = Read_24Cxx(TEMPER_EEPROM,M24512);
						taskEXIT_CRITICAL();
						flag_one_read_tdt = 1;
					}

				 if (flag_btn_plus ==1)
						{
							tmp_tdt++;
								if (tmp_tdt > TIME_DISPLAY_MAX)
									{
										tmp_tdt = 3;
									}
								flag_btn_plus =0;
						}
				 if (flag_btn_minus ==1)
						{
							tmp_tdt--;
								if (tmp_tdt <3)
								{
									tmp_tdt = TIME_DISPLAY_MAX;
								}
							flag_btn_minus =0;
						}


					global_time_of_display_t = tmp_tdt;

					taskENTER_CRITICAL();
					Write_24Cxx(TEMPER_EEPROM,tmp_tdt,M24512);
					taskEXIT_CRITICAL();

					process_data_for_display_tph(TIME_TDT,tmp_tdt);
					out_data_to_indicators ();
					 vTaskDelay(100/portTICK_RATE_MS);
		}
if (flag_menu ==  MENU_SET_TIME_DISPLAY_DM)
	{

			IN18_Off();
				if (flag_one_read_tdd == 0)
					{
						taskENTER_CRITICAL();
						tmp_tdd = Read_24Cxx(DAYMONTH_EEPROM,M24512);
						taskEXIT_CRITICAL();
						flag_one_read_tdd = 1;
					}

				 if (flag_btn_plus ==1)
						{
							tmp_tdd++;
								if (tmp_tdd > TIME_DISPLAY_MAX)
									{
										tmp_tdd = 3;
									}
								flag_btn_plus =0;
						}
				 if (flag_btn_minus ==1)
						{
							tmp_tdd--;
								if (tmp_tdd < 3)
								{
									tmp_tdd = TIME_DISPLAY_MAX;
								}
							flag_btn_minus =0;
						}


					global_time_of_display_d = tmp_tdd;

					taskENTER_CRITICAL();
					Write_24Cxx(DAYMONTH_EEPROM,tmp_tdd,M24512);
					taskEXIT_CRITICAL();

					process_data_for_display_tph(TIME_TDD,tmp_tdd);
					out_data_to_indicators ();
					 vTaskDelay(100/portTICK_RATE_MS);
		}

if (flag_menu ==  MENU_SET_TIME_DISPLAY_HUM)
	{

			IN18_Off();
				if (flag_one_read_th== 0)
					{
						taskENTER_CRITICAL();
						tmp_th = Read_24Cxx(HUMV_EEPROM,M24512);
						taskEXIT_CRITICAL();
						flag_one_read_th = 1;
					}

				 if (flag_btn_plus ==1)
						{
					 	 	 tmp_th++;
								if (tmp_th > TIME_DISPLAY_MAX)
									{
									 tmp_th = 3;
									}
								flag_btn_plus = 0;
						}
				 if (flag_btn_minus ==1)
						{
					 	 tmp_th--;
								if (tmp_th <3)
								{
									tmp_th = TIME_DISPLAY_MAX;
								}
							flag_btn_minus =0;
						}


					global_time_of_display_h = tmp_th;

					taskENTER_CRITICAL();
					Write_24Cxx(HUMV_EEPROM,tmp_tdd,M24512);
					taskEXIT_CRITICAL();

					process_data_for_display_tph(TIME_TDH ,tmp_th);
					out_data_to_indicators ();
					 vTaskDelay(100/portTICK_RATE_MS);
		}

if (flag_menu ==  MENU_SET_TIME_DISPLAY_RGBLIGHT )
	{
	IN18_Off();
	if (flag_one_read_rgbl== 0)
		{
			taskENTER_CRITICAL();
			tmp_rgbl = Read_24Cxx(STATUSLIGHT_EEPROM,M24512);
			taskEXIT_CRITICAL();
			flag_one_read_rgbl = 1;
		}

	             if (flag_btn_plus ==1)
						{
		                  tmp_rgbl++;

		                  if (tmp_rgbl>1)
		                  {
		                	  tmp_rgbl = 0;
		                  }

 						  flag_btn_plus =0;
						}
				 if (flag_btn_minus == 1)
						{
					       tmp_rgbl++;

			               if (tmp_rgbl>1)
			                  {
			                	  tmp_rgbl = 0;
			                  }

					       flag_btn_minus = 0;

						}
					global_display_rgbl_status = tmp_rgbl;

					taskENTER_CRITICAL();
					Write_24Cxx(STATUSLIGHT_EEPROM,tmp_rgbl,M24512);
					taskEXIT_CRITICAL();

					process_data_for_display_tph(STATUS_RGB ,tmp_rgbl);
					out_data_to_indicators ();
					 vTaskDelay(100/portTICK_RATE_MS);

	}

						TC=global_time_of_display_c;
						TP=TC+global_time_of_display_p;
						TT1=TP+global_time_of_display_t;
						TT2=TT1+global_time_of_display_t2;
						TH=TT2+global_time_of_display_h;
						TD=TH+ global_time_of_display_d;

    }
   }

