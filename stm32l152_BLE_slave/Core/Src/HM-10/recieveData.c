/*
 * recieveData.c
 *
 *  Created on: 25 СЏРЅРІ. 2024 Рі.
 *      Author: Andrianov Vitaly
 */

#include "HM-10/recieveData.h"

//---------------------------------------------------------------------------------------------------//
	uint8_t rxBuf[rxBuf_SIZE];				// receive data buffer
	uint8_t subBuf[subBuf_SIZE];			// sub data buffer
//---------------------------------------------------------------------------------------------------//
	uint16_t oldPos = 0;					// old data position
	uint16_t newPos = 0;					// new data position
//---------------------------------------------------------------------------------------------------//
	char minTemp[10] = "50.50";				// min temperature buffer
	char maxTemp[10] = "-50.50";			// max temperature buffer
//---------------------------------------------------------------------------------------------------//
	char minRSSI[10] = "999";				// min RSSI buffer
	char maxRSSI[10] = "-250";				// max RSSI buffer
//---------------------------------------------------------------------------------------------------//
	uint8_t currPongTx = -1;				// current num of transmitted data
	uint8_t currPingRx = -1;				// current num of received data
	uint16_t countOfRxData;					// amount of received data
	uint16_t countOfTxData;					// amount of transmitted data
	uint16_t loss = 0;						// current num of lost data
//---------------------------------------------------------------------------------------------------//
	const char * pong = "pong";				// const start of transmit massage
	const char * ping = "ping";				// const start of receive massage
//---------------------------------------------------------------------------------------------------//
	char currPongTxStr[20];					// sub buffer for prost proc transmit massage
	char currPingRxStr[20];					// sub buffer for post proc received massage
	char currLossStr[20] = "0";				// sub buffer to post proc lost data
//---------------------------------------------------------------------------------------------------//
	char currTxBuf[50];						// sub buffer for check current massage to transmit
	char currRxBuf[50];						// sub buffer for check current massage to receive
	char stats[50];							// RX/TX/LOSS stats buffer
	uint8_t rxDataControlList[255] = {0};	// received Data control list
//---------------------------------------------------------------------------------------------------//
	char rxBufNumStr[20];					// sub buffer to control expected receved num and actual
	uint16_t rxBufNum = 0;					// int of actual received num
//---------------------------------------------------------------------------------------------------//

/**
  * @brief  Receive data post proc
  * @note   Function from unknown size of received data
  * @param  huart - current HM10 huart
  * @param  size - receive data size
  * @retval void
 */
void UART4_RxCpltCallback(UART_HandleTypeDef *huart, uint16_t size) {
	oldPos = newPos;
	/* Check available buffer size */
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
	/* if get successful connection message from HM10 */
	if (strcmp ((char *) rxBuf, (char *) "OK+CONN\r\n") == 0) {
		connEvent();
	}
	/* if get lost connection message from HM10 */
	else if (strcmp ((char *) rxBuf, (char *) "OK+LOST\r\n") == 0) {
		connLostEvent();
	}
	/* if get temperature or RSSI request from connected device */
	else if (rxBuf[0] == 'O' && rxBuf[1] == 'K' && rxBuf[2] == '+') {
		getTempRssiEvent();
	}
	/* if get common message text */
	else {
		getMsgEvent(huart);
	}
	/* Start new DMA Idle interrupt */
	HAL_UARTEx_ReceiveToIdle_DMA(huart, (uint8_t *) rxBuf, rxBuf_SIZE);
	__HAL_DMA_DISABLE_IT(&hdma_uart4_rx, DMA_IT_HT);
}

/**
  * @brief  Clearing receive data buffer
  * @retval void
 */
void clearingRXBuf() {
	memset(rxBuf, 0, rxBuf_SIZE);
	memset(subBuf, 0, subBuf_SIZE);
}

/**
  * @brief  Successful connection event
  * @retval void
 */
void connEvent() {
	/* Display successful connection to user */
	st7789_FillRect(170, 10, 150, 20, WHITE_st7789);
	/* Clear previous received data stats */
	st7789_FillRect(160, 165, 150, 20, WHITE_st7789);
	/* Change connection status */
	st7789_PrintString(220, 10, BLACK_st7789, GREEN_st7789, 1, &font_11x18, 1, "Сопряжен");
	isConnected = 1;
	/* Prepare variables to receive data */
	clearingRXBuf();
}

/**
  * @brief  Lost connection event
  * @retval void
 */
void connLostEvent() {
	/* Display lost connection to user */
	st7789_FillRect(185, 10, 150, 20, WHITE_st7789);
	/* Change connection status */
	st7789_PrintString(185, 10, BLACK_st7789, RED_st7789, 1, &font_11x18, 1, "Не сопряжен");
	isConnected = 0;
	/* Prepare variables to receive data */
	clearingRXBuf();
	memset(stats, 0, 50);
	memset(currTxBuf, 0, 50);
	memset(currRxBuf, 0, 50);
	memset(currPongTxStr, 0, 20);
	currPongTx = -1;
	currPingRx = -1;
	loss = 0;
	rxBufNum = 0;
}

/**
  * @brief  Get Temperature or RSSI post proc
  * @retval void
 */
void getTempRssiEvent() {
	/* Сolon flag in temp/rssi value string */
	uint8_t dotFlag = 0;
	/* Current char position */
	uint8_t currentChar = 0;
	/* PostProc result string */
	char res_str[10];
	/* Character search */
	for (uint8_t i=0; i < rxBuf_SIZE; i++) {
		/* A colon was found */
		if (i > 0 && rxBuf[i - 1] == ':') dotFlag = 1;
		/* Beginning of temp/rssi values */
		if (dotFlag) {
			/* Skipping an insignificant 0 in temp value */
			if (rxBuf[i] == '0' && currentChar == 0) {}
			else {
				res_str[currentChar] = rxBuf[i];
				currentChar++;
			}
		}
	}
	/* 0 - temp, 1,2 - data, 3 - rssi, 4,5 - data */
	/* Current data - temperature value */
	if (msgType == 0) {
		/* Current temp value is higher than maxTemp */
		if (strcmp( maxTemp, res_str ) < 0) {
			strncpy(maxTemp, res_str, 10);
			/* Display actual max value */
			st7789_PrintString(250, 75, BLACK_st7789, WHITE_st7789, 1, &font_11x18, 1, maxTemp);
		}
		/* Current temp value is lower than minTemp */
		if (strcmp( minTemp, res_str ) > 0) {
			strncpy(minTemp, res_str, 10);
			/* Display actual min value */
			st7789_PrintString(250, 55, BLACK_st7789, WHITE_st7789, 1, &font_11x18, 1, minTemp);
		}
		/* Display actual temp value */
		st7789_PrintString(250, 35, BLACK_st7789, WHITE_st7789, 1, &font_11x18, 1, res_str);
		msgType++;
	}
	/* Current data - RSSI value */
	else if (msgType == 3) {
		/* Current RSSI value is higher than maxRSSI */
		if (strcmp( maxRSSI, res_str ) < 0) {
			strncpy(maxRSSI, res_str, 10);
			/* Display actual max value */
			st7789_PrintString(270, 140, BLACK_st7789, WHITE_st7789, 1, &font_11x18, 1, maxRSSI);
		}
		/* Current RSSI value is higher than minRSSI */
		if (strcmp( minRSSI, res_str ) > 0) {
			strncpy(minRSSI, res_str, 10);
			/* Display actual min value */
			st7789_PrintString(270, 120, BLACK_st7789, WHITE_st7789, 1, &font_11x18, 1, minRSSI);
		}
		/* Display actual RSSI value */
		st7789_PrintString(270, 100, BLACK_st7789, WHITE_st7789, 1, &font_11x18, 1, res_str);
		msgType++;
	}
	/* Clear working memory */
	clearingRXBuf();
}

/**
  * @brief  Get message event
  * @param  huart - current HM10 huart
  * @retval void
 */
void getMsgEvent(UART_HandleTypeDef *huart) {
	/* Check current data queue */
	msgType++;
	/* 0 - temp, 1,2 - data, 3 - rssi, 4,5 - data */
	if (msgType == 5) msgType = 0;
	/* Create current expected received string */
	currPingRx++;
	snprintf(currPingRxStr, sizeof(currPingRxStr), "%d", currPingRx);
	strcat(currRxBuf, ping);
	strcat(currRxBuf, currPingRxStr);
	/* Actual received string num to int */
	for (uint8_t i=4; i < strlen((char*) rxBuf); i++) {
		rxBufNumStr[i-4] = rxBuf[i];
	}
	rxBufNum = atoi(rxBufNumStr);
	/* Mark received string in rxDataControlList*/
	rxDataControlList[rxBufNum] = 1;
	uint8_t lostPocket = 0;
	for (uint8_t j=0; j < currPingRx; j++) {
		if (rxDataControlList[j] == 0) lostPocket++;
	}
	snprintf(currLossStr, sizeof(currLossStr), "%d", lostPocket);
	memset(rxBufNumStr, 0, 50);
	/* Create current transmit data */
	currPongTx++;
	snprintf(currPongTxStr, sizeof(currPongTxStr), "%d", currPongTx);
	strcat(currTxBuf, pong);
	strcat(currTxBuf, currPongTxStr);
	/* Transmit created data */
	HAL_UART_Transmit(huart, (uint8_t *) currTxBuf, 50, 0xFFFF);
	/* Display current received data to user */
	st7789_FillRect(0, 210, 320, 20, WHITE_st7789);
	st7789_PrintString(20, 210, BLACK_st7789, WHITE_st7789, 1, &font_11x18, 1, (char *) rxBuf);
	clearingRXBuf();
	/* Display RX/TX/LOSS stats to user */
	strcat(stats, currPingRxStr);
	strcat(stats, "/");
	strcat(stats, currPongTxStr);
	strcat(stats, "/");
	strcat(stats, currLossStr);
	st7789_PrintString(160, 165, BLACK_st7789, WHITE_st7789, 1, &font_11x18, 1, stats);
	/* Clearing working memory */
	memset(stats, 0, 50);
	memset(currTxBuf, 0, 50);
	memset(currRxBuf, 0, 50);
	memset(currPongTxStr, 0, 20);
}
