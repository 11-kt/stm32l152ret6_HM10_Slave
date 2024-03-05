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

char minTemp[10] = "50.50";
char maxTemp[10] = "-50.50";

char minRSSI[10] = "999";
char maxRSSI[10] = "-250";

uint8_t currPongTx = -1;
uint16_t currPingRx = -1;
uint8_t loss = 0;
const char * pong = "pong";
const char * ping = "ping";
char currTxBuf[50];
char currRxBuf[50];
char stats[50];
char currPongTxStr[20];
char currPingRxStr[20];
char currLossStr[20] = "0";

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
		st7789_FillRect(170, 10, 150, 20, WHITE_st7789);
		st7789_PrintString(220, 10, BLACK_st7789, GREEN_st7789, 1, &font_11x18, 1, "Сопряжен");
		isConnected = 1;
		clearingRXBuf();
	}
	else if (strcmp ((char *) rxBuf, (char *) "OK+LOST\r\n") == 0) {
		st7789_FillRect(185, 10, 150, 20, WHITE_st7789);
		st7789_PrintString(185, 10, BLACK_st7789, RED_st7789, 1, &font_11x18, 1, "Не сопряжен");
		clearingRXBuf();
	}
	else if (rxBuf[0] == 'O' && rxBuf[1] == 'K' && rxBuf[2] == '+') {
		uint8_t dotFlag = 0;
		uint8_t currentChar = 0;
		char res_str[10];
		for (uint8_t i=0; i < rxBuf_SIZE; i++) {
			if (i > 0 && rxBuf[i - 1] == ':') dotFlag = 1;

			if (dotFlag) {
				if (rxBuf[i] == '0' && currentChar == 0) {

				}
				else {
					res_str[currentChar] = rxBuf[i];
					currentChar++;
				}
			}
		}
		if (isTemp == 1) {
			if (strcmp( maxTemp, res_str ) < 0) {
				strncpy(maxTemp, res_str, 10);
				st7789_PrintString(250, 75, BLACK_st7789, WHITE_st7789, 1, &font_11x18, 1, maxTemp);
			}
			if (strcmp( minTemp, res_str ) > 0) {
				strncpy(minTemp, res_str, 10);
				st7789_PrintString(250, 55, BLACK_st7789, WHITE_st7789, 1, &font_11x18, 1, minTemp);
			}
			st7789_PrintString(250, 35, BLACK_st7789, WHITE_st7789, 1, &font_11x18, 1, res_str);
			isTemp = 0;
		}
		else {
			if (strcmp( maxRSSI, res_str ) < 0) {
				strncpy(maxRSSI, res_str, 10);
				st7789_PrintString(270, 140, BLACK_st7789, WHITE_st7789, 1, &font_11x18, 1, maxRSSI);
			}
			if (strcmp( minRSSI, res_str ) > 0) {
				strncpy(minRSSI, res_str, 10);
				st7789_PrintString(270, 120, BLACK_st7789, WHITE_st7789, 1, &font_11x18, 1, minRSSI);
			}
			st7789_PrintString(270, 100, BLACK_st7789, WHITE_st7789, 1, &font_11x18, 1, res_str);
			isTemp = 1;
		}
		clearingRXBuf();
	}
	else {
		currPingRx++;
		snprintf(currPingRxStr, sizeof(currPingRxStr), "%d", currPingRx);
		strcat(currRxBuf, ping);
		strcat(currRxBuf, currPingRxStr);
		if (strstr (currRxBuf, (char *) rxBuf) == NULL) {
			loss++;
			snprintf(currLossStr, sizeof(currLossStr), "%d", loss);
		}

		currPongTx++;
		snprintf(currPongTxStr, sizeof(currPongTxStr), "%d", currPongTx);
		strcat(currTxBuf, pong);
		strcat(currTxBuf, currPongTxStr);
		HAL_UART_Transmit(huart, (uint8_t *) currTxBuf, 50, 0xFFFF);

		st7789_FillRect(0, 210, 320, 20, WHITE_st7789);
		st7789_PrintString(20, 210, BLACK_st7789, WHITE_st7789, 1, &font_11x18, 1, (char *) rxBuf);
		clearingRXBuf();

		strcat(stats, currPingRxStr);
		strcat(stats, "/");
		strcat(stats, currPongTxStr);
		strcat(stats, "/");
		strcat(stats, currLossStr);
		st7789_PrintString(160, 165, BLACK_st7789, WHITE_st7789, 1, &font_11x18, 1, stats);
		memset(stats, 0, 50);
		memset(currTxBuf, 0, 50);
		memset(currRxBuf, 0, 50);
		memset(currPongTxStr, 0, 20);
	}
	HAL_UARTEx_ReceiveToIdle_DMA(huart, (uint8_t *) rxBuf, rxBuf_SIZE);
	__HAL_DMA_DISABLE_IT(&hdma_uart4_rx, DMA_IT_HT);
}

void clearingRXBuf() {
	memset(rxBuf, 0, rxBuf_SIZE);
	memset(subBuf, 0, subBuf_SIZE);
}
