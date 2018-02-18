/*
 * DHT22.c
 *
 *  Created on: Feb 6, 2013
 *      Author: alamer
 */
#include "dht22.h"
#include "stm32f4xx.h"
#include "stm32f4xx_gpio.h"
#include "stm32_ub_freertos.h"


// Настраиваем пин на выход и возвращаем высокий уровень шины
void restorePin(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin, GPIO_InitTypeDef* out){
	GPIO_Init(GPIOx, out);
	GPIO_SetBits(GPIOx, GPIO_Pin);
}

// Все задержки для waitCounter вычислены для частоты 168 MHz
uint8_t getDHT22Data(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin, float *hum, float *temp)
{
	// Некоторые некорректные начальные данные
	*hum = -100;
	*temp = -100;

	// Структура для настройки пина на вход
	GPIO_InitTypeDef dht22_in;
	dht22_in.GPIO_Pin = GPIO_Pin;
	dht22_in.GPIO_Mode = GPIO_Mode_IN;
	dht22_in.GPIO_Speed = GPIO_Speed_2MHz;
	dht22_in.GPIO_PuPd = GPIO_PuPd_UP;

	// Структура для настройки пина на выход
	GPIO_InitTypeDef dht22_out;
	dht22_out.GPIO_Pin = GPIO_Pin;
	dht22_out.GPIO_Mode = GPIO_Mode_OUT;
	dht22_out.GPIO_Speed = GPIO_Speed_25MHz;
	dht22_out.GPIO_OType = GPIO_OType_PP;
	dht22_out.GPIO_PuPd = GPIO_PuPd_NOPULL;

	// Выставляем 1 и немного ждём
	restorePin(GPIOx, GPIO_Pin, &dht22_out);
	//delay_us(20);
	//vTaskDelayUntil( &xLastWakeTime, (20 / portTICK_RATE_MS ) );
	vTaskDelay(20/portTICK_RATE_MS);

	uint16_t waitCounter;

	// Выставляем 0, ждём, выставляем 1 и ждём сообщения датчика о готовности к передаче
	GPIO_ResetBits(GPIOx, GPIO_Pin);
	//delay_us(500);
	//vTaskDelayUntil( &xLastWakeTime, (500 / portTICK_RATE_MS ) );
	vTaskDelay(500/portTICK_RATE_MS);
	GPIO_SetBits(GPIOx, GPIO_Pin);
	//delay_us(20);
	vTaskDelay(20/portTICK_RATE_MS);
	//vTaskDelayUntil( &xLastWakeTime, (20 / portTICK_RATE_MS ) );
	GPIO_Init(GPIOx, &dht22_in);

	//todo: Вставить таймаут и выходить с ошибкой при его превышении
	// < 40 us
	waitCounter = 0;
	while (GPIO_ReadInputDataBit(GPIOx, GPIO_Pin) == 1){
		waitCounter++;
		if (waitCounter > 150){
			restorePin(GPIOx, GPIO_Pin, &dht22_out);
			return dht22Res_not_ready;
		}
	};
	// 80 us
	waitCounter = 0;
	while (GPIO_ReadInputDataBit(GPIOx, GPIO_Pin) == 0){
		waitCounter++;
		if (waitCounter > 300){
			restorePin(GPIOx, GPIO_Pin, &dht22_out);
			return dht22Res_not_ready;
		}
	};
	// 80 us
	waitCounter = 0;
	while (GPIO_ReadInputDataBit(GPIOx, GPIO_Pin) == 1){
		waitCounter++;
		if (waitCounter > 300){
			restorePin(GPIOx, GPIO_Pin, &dht22_out);
			return dht22Res_not_ready;
		}
	};

	// Обнуляем буфер
	uint8_t i;
	uint8_t data[5];
	uint16_t counter;
	//uint32_t cnt[40];
	for (i = 0; i < 5; i++){
		data[i] = 0;
	}

	// Считываем 40 бит(5 байт) данных:
	// Первые 2 байта - влажность
	// Следующие 2 - температура
	// Последний - контрольная сумма
	for (i = 0; i < 40; i++){
		counter = 0;
		uint8_t dI;
		uint8_t bit;
		dI = i / 8;
		bit = i % 8;
		bit = 7 - bit;

		//todo: Вставить таймаут и выходить с ошибкой при его превышении
		// 50 us
		waitCounter = 0;
		while (GPIO_ReadInputDataBit(GPIOx, GPIO_Pin) == 0){
			waitCounter++;
			if (waitCounter > 300){
				restorePin(GPIOx, GPIO_Pin, &dht22_out);
				return dht22Res_data_not_received;
			}
		};
		// 70 us
		waitCounter = 0;
		while (GPIO_ReadInputDataBit(GPIOx, GPIO_Pin) == 1){
			counter++;
			waitCounter++;
			if (waitCounter > 300){
				restorePin(GPIOx, GPIO_Pin, &dht22_out);
				return dht22Res_data_not_received;
			}
		};

		// Значение счетчика зависит от частоты МК
		// Для 168 MHz  пороговое значение = 170
		if (counter > 170){
			data[dI] = data[dI] + (uint8_t)(1 << bit);
		}
		//cnt[i] = counter;
	}

	// Настраиваем пин на выход и возвращаем высокий уровень шины
	restorePin(GPIOx, GPIO_Pin, &dht22_out);

	if ((uint8_t)(data[0]+data[1]+data[2]+data[3]) != data[4]){
		return dht22Res_not_valid_crc;
	}

	// Считаем результат
	*hum = (data[0] * 256 + data[1]);
	*hum /= 10;

	*temp = (data[2] & 0x7F)* 256 + data[3];
	*temp /= 10.0;
	if (data[2] & 0x80){
		*temp *= -1;
	}

	return dht22Res_no_error;
}
