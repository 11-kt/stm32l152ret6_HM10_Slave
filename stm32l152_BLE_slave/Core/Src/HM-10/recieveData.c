/*
 * recieveData.c
 *
 *  Created on: 25 СЏРЅРІ. 2024 Рі.
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

	if (strcmp ((char *) rxBuf, (char *) "OK+CONN\r\n") == 0) {
		st7789_FillRect(140, 20,  100, 20, WHITE_st7789);
		st7789_PrintString(140, 20, BLACK_st7789, GREEN_st7789, 1, &font_11x18, 1, "Связан");
		isConnected = 1;
		clearingRXBuf();
	}
	else if (strcmp ((char *) rxBuf, (char *) "OK+LOST\r\n") == 0) {
		st7789_FillRect(140, 20,  100, 20, WHITE_st7789);
		st7789_PrintString(140, 20, BLACK_st7789, RED_st7789, 1, &font_11x18, 1, "Не связан");
		clearingRXBuf();
	}
	else if (isTemp && rxBuf[0] == 'O' && rxBuf[1] == 'K' && rxBuf[2] == '+') {
		uint8_t dotFlag = 0;
		uint8_t currentChar = 0;
		char temp_str[10];
		for (uint8_t i=0; i < rxBuf_SIZE; i++) {
			if (i > 0 && rxBuf[i - 1] == ':') dotFlag = 1;

			if (dotFlag) {
				if (rxBuf[i] == '0' && currentChar == 0) {

				}
				else {
					temp_str[currentChar] = rxBuf[i];
					currentChar++;
				}
			}
		}
		st7789_PrintString(190, 50, BLACK_st7789, WHITE_st7789, 1, &font_11x18, 1, temp_str);
		clearingRXBuf();
		isTemp = 0;
		isRSSI = 1;
	}
	else if (isRSSI && rxBuf[0] == 'O' && rxBuf[1] == 'K' && rxBuf[2] == '+') {
		uint8_t dotFlag = 0;
		uint8_t currentChar = 0;
		char rssi_str[10];
		for (uint8_t i=0; i < rxBuf_SIZE; i++) {
			if (i > 0 && rxBuf[i - 1] == ':') dotFlag = 1;

			if (dotFlag) {
				rssi_str[currentChar] = rxBuf[i];
				currentChar++;
			}
		}
		st7789_PrintString(210, 80, BLACK_st7789, WHITE_st7789, 1, &font_11x18, 1, rssi_str);
		clearingRXBuf();
		isRSSI = 0;
		isTemp = 1;
	}
	else {
		st7789_FillRect(0, 170,  320, 20, WHITE_st7789);
		st7789_PrintString(20, 170, BLACK_st7789, WHITE_st7789, 1, &font_11x18, 1, (char *) rxBuf);
		isConnected = 0;
		clearingRXBuf();
		isTemp = 0;
		isRSSI = 1;
	}
	HAL_UARTEx_ReceiveToIdle_DMA(huart, (uint8_t *) rxBuf, rxBuf_SIZE);
	__HAL_DMA_DISABLE_IT(&hdma_uart4_rx, DMA_IT_HT);
}

void clearingRXBuf() {
	memset(rxBuf, 0, rxBuf_SIZE);
	memset(subBuf, 0, subBuf_SIZE);
}
