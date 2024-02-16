/*
 * recieveData.c
 *
 *  Created on: 25 янв. 2024 г.
 *      Author: Andrianov Vitaly
 */

#include "HM-10/recieveData.h"

uint8_t rxBuf[rxBuf_SIZE];
uint8_t subBuf[subBuf_SIZE];
uint16_t oldPos = 0;
uint16_t newPos = 0;

void UART4_RxCpltCallback(UART_HandleTypeDef *huart, uint16_t size) {
	oldPos = newPos;
	if (oldPos + size > subBuf_SIZE) {
		uint16_t dataToCopy = subBuf_SIZE - oldPos;

		memcpy((uint8_t *) subBuf + oldPos, rxBuf, dataToCopy);

		oldPos = 0;

		memcpy((uint8_t *) subBuf, (uint8_t *) rxBuf + dataToCopy, (size-dataToCopy));

		newPos = size - dataToCopy;
	}
	else {
		memcpy((uint8_t *) subBuf + oldPos, rxBuf, size);
		newPos = size + oldPos;
	}

	st7789_SetWindow(20, 20, 240, 320);
	st7789_PrintString(20, 20, GREEN_st7789, BLACK_st7789, 1, &font_7x9, 1, rxBuf);

	HAL_UARTEx_ReceiveToIdle_DMA(huart, (uint8_t *) rxBuf, rxBuf_SIZE);
	__HAL_DMA_DISABLE_IT(&hdma_uart4_rx, DMA_IT_HT);
}
