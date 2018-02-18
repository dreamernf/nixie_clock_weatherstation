#include "main.h"
#include "defines.h"
#include "functions.h"
#include "stm32_ub_freertos.h"
#include "stm32_ub_led.h"
#include "stm32_ub_pwm_tim3.h"
#include "stm32_ub_i2c2.h"
#include "stm32_ub_uart.h"
#include "stm32_ub_spi1.h"
#include "stm32_ub_spi_send_mbyte.h"
#include "stm32_ub_button.h"
#include "stm32_ub_encoder_tim2.h"
#include "eeprom.h"

uint16_t counterwert = 0;
uint16_t k =0;

int32_t P = 0;
int16_t T = 0;
int16_t L = 0;


volatile int8_t napravlenie = 0;

uint8_t sec = 0;
uint8_t min = 0;
uint8_t hour = 0;

uint8_t day = 0;
uint8_t month = 0;
uint8_t year = 0;

uint8_t  time_of_display = 0;

uint8_t  global_time_of_display_c = 0;
uint8_t  global_time_of_display_p = 0;

uint8_t flag_menu = 0;
uint8_t flag_one_read_m = 0;
uint8_t flag_one_read_h = 0;
uint8_t flag_one_read_tdc = 0;
uint8_t flag_one_read_tdp = 0;


void IsWorkedTask( void *pvParameters );
void PWMTask( void *pvParameters );
void GetTP( void *pvParameters );
void Out_Data_to_Nixie( void *pvParameters );
void ScanInput( void *pvParameters );


void hw_init(void)
{

  //инициализаци€ железа
  UB_Button_Init();
  UB_Led_Init();
  UB_Uart_Init();
  UB_Button_OnClick(BTN_MENU);
  UB_I2C2_Init();
  NIXIE_REG_Init();
  UB_PWM_TIM3_Init();

  UB_ENCODER_TIM2_Init(ENC_T2_MODE_2A, ENC_T2_TYP_NORMAL,  0xFFFF);

  //считывание настроек из ѕ«”
  taskENTER_CRITICAL();
  global_time_of_display_c= Read_24Cxx(CLOCK,M24512);
  taskEXIT_CRITICAL();

  taskENTER_CRITICAL();
  global_time_of_display_p= Read_24Cxx(PRESSURE,M24512);
  taskEXIT_CRITICAL();
}


//--------------------------------------------------------------
int main(void)
{
  SystemInit();

  //инициализаци€ железа
  hw_init();

  //создание тасков
  xTaskCreate( IsWorkedTask, ( signed char * ) "IsWorkedTask", configMINIMAL_STACK_SIZE, NULL, 1, NULL );
  xTaskCreate( PWMTask, ( signed char * ) "PWMTask", configMINIMAL_STACK_SIZE, NULL, 1, NULL );
  xTaskCreate( GetTP, ( signed char * ) "GetTP", configMINIMAL_STACK_SIZE, NULL, 1, NULL);
  xTaskCreate( Out_Data_to_Nixie, ( signed char * ) "Out_Data_to_Nixie", configMINIMAL_STACK_SIZE, NULL, 1, NULL);
  xTaskCreate( ScanInput, ( signed char * ) "ScanInput", configMINIMAL_STACK_SIZE, NULL, 1, NULL);

  //запустили планировщик
  vTaskStartScheduler();

  while(1)
  {

  }
}


//моргает светодиодом отправл€ет строку в USART  показыва€, что система не зависла
void IsWorkedTask( void *pvParameters )
{
  while(1)
  {

	  UB_Led_Toggle(LED_RED);
    UB_Uart_SendString(COM4,"I am live",LFCR);
    vTaskDelay(500/portTICK_RATE_MS);
  }
}


//сканирует кнопки, определ€ет и обратывает нажатие
void ScanInput( void *pvParameters )
{
  while(1)
  {
   if (UB_Button_OnClick(BTN_MENU)==true)  {

	   UB_Led_Toggle(LED_ORANGE);

	   	  //если находимс€ не в меню
	      if	(flag_menu == NO_MENU)
	      {
	    	  //установить пункт меню - Ќастройка времени - минут
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
   	      }

	      //если находимс€ в последнем пункте меню
	      if  (flag_menu == MENU_LAST)
	      {
	    	  //выйти из меню
	    	  flag_menu = NO_MENU;
	      }
    }

    vTaskDelay(100/portTICK_RATE_MS);
  }
}

//таск управлени€ RGB подстветкой
void PWMTask( void *pvParameters )
{
uint8_t step = 0;
uint8_t  r,g,b;
uint8_t  maxx = RGBLED_BRIGN;
  while(1)
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
	             if(r>=maxx)
	                       {
	            	          step=2;
	                      }
	        }
	 if(step==2)
	        {
	           g++;
	              if(g>=maxx)
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
	             if(b>=maxx)
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
	                  if(r>=maxx)
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

//таск опроса датчиков давлени€, температуры, влажности, часов
void GetTP( void *pvParameters )
  {

	uint8_t  status = 0;
    while(1)
    {
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

           	//считываем значение часов-даты
           	taskENTER_CRITICAL();
           	status = UB_I2C2_ReadMultiByte(DS3231_I2C_ADDR, DS3231_TIME_CAL_ADDR, 6);
           	taskEXIT_CRITICAL();

       	 if (status == 0){
       		 sec = bcd2int(I2C2_DATA[0]);
       		 min = bcd2int(I2C2_DATA[1]);
       		 hour = bcd2int(I2C2_DATA[2]);
       		 day = bcd2int(I2C2_DATA[3]);
       		 month = bcd2int(I2C2_DATA[4]);
       		 year = bcd2int(I2C2_DATA[5]);
       	 }

           	vTaskDelay(100/portTICK_RATE_MS);

    }
  }

//таск расчета и вывода данных на индикаторы, индикаци€ меню
void Out_Data_to_Nixie( void *pvParameters )
  {
	portTickType xLastWakeTime;
	int8_t tmp_hour = 0;
	int8_t tmp_min = 0;
	int8_t tmp_tdc = 0;
	int8_t tmp_tdp = 0;

	uint8_t byte_eeprom = 0;

	/* ѕеременна€ xLastWakeTime нуждаетс€ в инициализации текущим
	     значением счетчика тиков. »мейте в виду, переменна€ записываетс€
	     €вно только в этот момент. «атем xLastWakeTime обновл€етс€
	     автоматически внутри функции vTaskDelayUntil(). */
	  xLastWakeTime = xTaskGetTickCount();


	while(1)
    {

if (flag_menu == NO_MENU)
     {
        time_of_display++;
		if (time_of_display<global_time_of_display_c*KOEFF_TIME_DISPLAY)
		{
		   IN18_On_and_dot();
		   process_data_for_display_tph(TIME_SECONDS,sec);
		   process_data_for_display_clock(hour,min);
		   out_data_to_indicators ();
		}

		if ((time_of_display>global_time_of_display_c*KOEFF_TIME_DISPLAY) &&
				(time_of_display<global_time_of_display_c*KOEFF_TIME_DISPLAY+global_time_of_display_p*KOEFF_TIME_DISPLAY))
		{

			IN18_Off();
			process_data_for_display_tph(PRESSURE,P);
			out_data_to_indicators ();
		}

		if (time_of_display>global_time_of_display_c*KOEFF_TIME_DISPLAY+global_time_of_display_p*KOEFF_TIME_DISPLAY)
		{

			IN18_Off();
			process_data_for_display_tph(TEMP_P,T);
			out_data_to_indicators ();
		}

		if (time_of_display==global_time_of_display_c*KOEFF_TIME_DISPLAY+global_time_of_display_p*KOEFF_TIME_DISPLAY+TIME_DISPLAY_PRESSURE)
			{
				time_of_display = 0;
			}

		vTaskDelayUntil( &xLastWakeTime, (TIME_BASE / portTICK_RATE_MS ) );
    }

if (flag_menu == MENU_SET_MINUTE)
	{

	  	  	  if (flag_one_read_m == 0)
	  	  	  	  {
	  	  		  	  tmp_min = min;
	  	  		  	  flag_one_read_m = 1;
	  	  	  	  }

	  	  	  	 taskENTER_CRITICAL();
	  	  	  	 napravlenie = enc_GetRelativeMove();
	  	  	  	 taskEXIT_CRITICAL();

	  	  	  if (napravlenie >0)
	  	  	  	  {
	  	  		  	  tmp_min++;
	  	  		  	  	  if (tmp_min>59)
	  	  		  	  	     {
	  	  		  	  		  	  tmp_min=0;
	  	  		  	  	      }
	  	  	  	  }
	  	  	 if (napravlenie <0)
	  	  	 	 {
	  	  		 	 tmp_min--;
	  	  		 	 	 if (tmp_min<0)
	  	  		 	 	 	 {
	  	  		 	 		 	 tmp_min=59;
	  	  		 	 	 	 }
	  	  	 	 }

	  	  	 	 IN18_On_Only_Minutes();
	  	  	 	 process_data_for_display_tph(TIME_SET,sec);
	  	  	 	 process_data_for_display_clock(hour,tmp_min);
	  	  	 	 out_data_to_indicators ();

	  	  	 	 vTaskDelayUntil( &xLastWakeTime, (300 / portTICK_RATE_MS ) );

	  	  	 	 IN18_On_and_dot();
	  	  	 	 process_data_for_display_tph(TIME_SET,sec);
	  	  	 	 process_data_for_display_clock(hour,tmp_min);
	  	  	 	 out_data_to_indicators ();


	  	  	 	 I2C2_DATA[0] = int2bcd(30);
	  	  	 	 I2C2_DATA[1] = int2bcd(tmp_min);
	  	  	 	 I2C2_DATA[2] = int2bcd(hour);

	  	  	 	 taskENTER_CRITICAL();
	  	  	 	 UB_I2C2_WriteMultiByte(DS3231_I2C_ADDR,DS3231_TIME_CAL_ADDR, 3);
	  	  	 	 taskEXIT_CRITICAL();

	  	  	 	 vTaskDelayUntil( &xLastWakeTime, (200 / portTICK_RATE_MS ) );
      	  }
if (flag_menu == MENU_SET_HOUR)
	{

					if (flag_one_read_h == 0)
						{
							tmp_hour = hour;
							flag_one_read_h = 1;
						}

					taskENTER_CRITICAL();
					napravlenie = enc_GetRelativeMove();
					taskEXIT_CRITICAL();

					if (napravlenie >0)
						{
							tmp_hour++;
								if (tmp_hour>23)
									{
										tmp_hour=0;
									}
						}
					if (napravlenie <0)
						{
							tmp_hour--;
								if (tmp_hour<0)
								{
									tmp_hour=23;
								}
						}

						IN18_On_Only_Hours();
						process_data_for_display_tph(TIME_SET,sec);
						process_data_for_display_clock(tmp_hour,min);
						out_data_to_indicators ();

						vTaskDelayUntil( &xLastWakeTime, (300 / portTICK_RATE_MS ) );

						IN18_On_and_dot();
						process_data_for_display_tph(TIME_SET,sec);
						process_data_for_display_clock(tmp_hour, min);
						out_data_to_indicators ();


						I2C2_DATA[0] = int2bcd(30);
						I2C2_DATA[1] = int2bcd(min);
						I2C2_DATA[2] = int2bcd(tmp_hour);

						taskENTER_CRITICAL();
						UB_I2C2_WriteMultiByte(DS3231_I2C_ADDR,DS3231_TIME_CAL_ADDR, 3);
						taskEXIT_CRITICAL();

						vTaskDelayUntil( &xLastWakeTime, (200 / portTICK_RATE_MS ) );

		}
if (flag_menu == MENU_SET_TIME_DISPLAY_CLOCK)
	{
						IN18_Off();
						if (flag_one_read_tdc == 0)
							{
								taskENTER_CRITICAL();
								tmp_tdc = Read_24Cxx(CLOCK,M24512);
								taskEXIT_CRITICAL();
								flag_one_read_tdc = 1;
							}

						taskENTER_CRITICAL();
						napravlenie = enc_GetRelativeMove();
						taskEXIT_CRITICAL();

						if (napravlenie >0)
							{
								tmp_tdc++;
									if (tmp_tdc>254)
										{
											tmp_tdc=0;
										}
							}
						if (napravlenie <0)
		{
			tmp_tdc--;
			if (tmp_tdc<0){
			tmp_tdc=254;
			}
		}

		global_time_of_display_c = tmp_tdc;

	   taskENTER_CRITICAL();
	   Write_24Cxx(0x2234,tmp_tdc,M24512);
	   taskEXIT_CRITICAL();

       process_data_for_display_tph(TIME_TDC,tmp_tdc);
       out_data_to_indicators ();
       vTaskDelayUntil( &xLastWakeTime, (200 / portTICK_RATE_MS ) );
}
if (flag_menu ==  MENU_SET_TIME_DISPLAY_PRESSURE)
	{
			IN18_Off();
				if (flag_one_read_tdp == 0)
					{
						taskENTER_CRITICAL();
						tmp_tdp = Read_24Cxx(PRESSURE,M24512);
						taskEXIT_CRITICAL();
						flag_one_read_tdp = 1;
					}

				taskENTER_CRITICAL();
				napravlenie = enc_GetRelativeMove();
				taskEXIT_CRITICAL();

					if (napravlenie >0)
						{
							tmp_tdp++;
								if (tmp_tdp > 254)
									{
										tmp_tdp = 0;
									}

						}
					if (napravlenie <0)
						{
							tmp_tdp--;
								if (tmp_tdp <0)
								{
									tmp_tdp = 254;
								}
						}


					global_time_of_display_p = tmp_tdp;

					taskENTER_CRITICAL();
					Write_24Cxx(PRESSURE,tmp_tdp,M24512);
					taskEXIT_CRITICAL();

					process_data_for_display_tph(TIME_TDP,tmp_tdp);
					out_data_to_indicators ();
					vTaskDelayUntil( &xLastWakeTime, (100 / portTICK_RATE_MS ) );
		}

    }
 }
