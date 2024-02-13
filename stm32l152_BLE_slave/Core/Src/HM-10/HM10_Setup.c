/*
 * HM10_Setup.c
 *
 *  Created on: Feb 12, 2024
 *      Author: kayaton
 */

#include "HM-10/HM10_Setup.h"

static char dma_res[30];

setup_result setupSlave(UART_HandleTypeDef *huart) {

	setup_result connection = checkConnection(huart);
	if (connection != OK) {
		return HM10_ERROR;
	}

	usDelay(delayUs);

	if (getRole(huart) == MASTER) {
		setRole(huart, SLAVE);
	}

	usDelay(delayUs);

	if (getImme(huart) == ONLY_AT) {
		setImme(huart, BASE);
	}

	usDelay(delayUs);

	setName(huart, "HM-10_Slave2");

	return OK;
}

setup_result checkConnection(UART_HandleTypeDef *huart) {
	clearingBuf();

	HAL_UART_Receive_DMA(huart, (uint8_t *) dma_res, getResLength(AT));
	HAL_UART_Transmit(huart, getCommand(AT), strlen((char *) getCommand(AT)), 0xFFFF);

	usDelay(delayUs);

	if (strcmp (dma_res, "OK") != 0) return LOST_AT;

	return OK;
}

setup_result setBaudRate(UART_HandleTypeDef *huart, hm10_baud baudrate) {
	clearingBuf();

	char tx_baud = baudrate + '0';
	char* tx_cmd = concat_str((char*) getCommand(BAUD_SET), tx_baud);

	HAL_UART_Receive_DMA(huart, (uint8_t *) dma_res, getResLength(BAUD_SET));
	HAL_UART_Transmit(huart, (uint8_t *) tx_cmd, strlen(tx_cmd), 0xFFFF);

	usDelay(delayUs);

	free(tx_cmd);
	int tx_res = dma_res[getResLength(BAUD_SET) - 1] - '0';
	if (tx_res != 0 && tx_res != 1 && tx_res != 2 && tx_res != 3 && tx_res != 4 &&
		tx_res != 5 && tx_res != 6 && tx_res != 7 && tx_res != 8) return HM10_ERROR;

	return OK;
}

setup_result setRole(UART_HandleTypeDef *huart, hm10_role role) {
	clearingBuf();

	char tx_role = role + '0';
	char* tx_cmd = concat_str((char*) getCommand(ROLE_SET), tx_role);

	HAL_UART_Receive_DMA(huart, (uint8_t *) dma_res, getResLength(ROLE_SET));
	HAL_UART_Transmit(huart, (uint8_t *) tx_cmd, strlen(tx_cmd), 0xFFFF);

	usDelay(delayUs);

	free(tx_cmd);

	if (strcmp (dma_res, "OK+Set:1") != 0 && strcmp (dma_res, "OK+Set:0") != 0) return HM10_ERROR;

	return OK;
}

setup_result setImme(UART_HandleTypeDef *huart, hm10_imme imme) {
	clearingBuf();

	char tx_imme = imme + '0';
	char* tx_cmd = concat_str((char*) getCommand(IMME_SET), tx_imme);

	HAL_UART_Receive_DMA(huart, (uint8_t *) dma_res, getResLength(IMME_SET));
	HAL_UART_Transmit(huart, (uint8_t *) tx_cmd, strlen(tx_cmd), 0xFFFF);

	usDelay(delayUs);

	free(tx_cmd);

	if (strcmp (dma_res, "OK+Set:1") != 0 && strcmp (dma_res, "OK+Set:0") != 0) return HM10_ERROR;

	return OK;
}

setup_result setName(UART_HandleTypeDef *huart, char * name) {
	clearingBuf();

	char* tx_cmd = concat_cmd_str((char *) getCommand(NAME_SET), name);

	HAL_UART_Receive_DMA(huart, (uint8_t *) dma_res, getResLength(NAME_SET) + strlen(name));
	HAL_UART_Transmit(huart, (uint8_t *) tx_cmd, strlen(tx_cmd), 0xFFFF);

	usDelay(delayUs);

	free(tx_cmd);

	if (strcmp (dma_res, concat_cmd_str("OK+Set", name)) != 0) return HM10_ERROR;

	return OK;
}

hm10_baud getBaudRate(UART_HandleTypeDef *huart) {
	clearingBuf();

	HAL_UART_Receive_DMA(huart, (uint8_t *) dma_res, getResLength(ROLE_GET));
	HAL_UART_Transmit(huart, getCommand(BAUD_GET), strlen((char *) getCommand(BAUD_GET)), 0xFFFF);

	usDelay(delayUs);

	switch(dma_res[getResLength(BAUD_GET) - 1]) {
		case BAUD_9600 + '0':
			return BAUD_9600;
		case BAUD_19200 + '0':
			return BAUD_19200;
		case BAUD_38400 + '0':
			return BAUD_38400;
		case BAUD_57600 + '0':
			return BAUD_57600;
		default:
			return BAUD_115200;
	}

	return BAUD_115200;
}

hm10_role getRole(UART_HandleTypeDef *huart) {
	clearingBuf();

	HAL_UART_Receive_DMA(huart, (uint8_t *) dma_res, getResLength(ROLE_GET));
	HAL_UART_Transmit(huart, getCommand(ROLE_GET), strlen((char *) getCommand(ROLE_GET)), 0xFFFF);

	usDelay(delayUs);

	if (dma_res[getResLength(ROLE_GET) - 1] == SLAVE + '0') return SLAVE;

	return MASTER;
}

hm10_imme getImme(UART_HandleTypeDef *huart) {
	clearingBuf();

	HAL_UART_Receive_DMA(huart, (uint8_t *) dma_res, getResLength(IMME_GET));
	HAL_UART_Transmit(huart, getCommand(IMME_GET), strlen((char *) getCommand(IMME_GET)), 0xFFFF);

	usDelay(delayUs);

	if (dma_res[getResLength(IMME_GET) - 1] == BASE + '0') return BASE;

	return ONLY_AT;

}

setup_result renewDevice(UART_HandleTypeDef *huart) {
	clearingBuf();

	HAL_UART_Receive_DMA(huart, (uint8_t *) dma_res, getResLength(RENEW));
	HAL_UART_Transmit(huart, getCommand(RENEW), strlen((char*) getCommand(RENEW)), 0xFFFF);

	usDelay(delayUs);

	if (strcmp (dma_res, "OK+RENEW") != 0) return HM10_ERROR;

	return OK;
}

setup_result resetDevice(UART_HandleTypeDef *huart) {
	clearingBuf();

	HAL_UART_Receive_DMA(huart, (uint8_t *) dma_res, getResLength(RESET));
	HAL_UART_Transmit(huart, getCommand(RESET), strlen((char*) getCommand(RESET)), 0xFFFF);

	usDelay(delayUs);

	if (strcmp (dma_res, "OK") != 0) return HM10_ERROR;

	return OK;
}

setup_result startHM10(UART_HandleTypeDef *huart) {
	clearingBuf();

	HAL_UART_Receive_DMA(huart, (uint8_t *) dma_res, getResLength(START));
	HAL_UART_Transmit(huart, getCommand(START), strlen((char*) getCommand(START)), 0xFFFF);

	usDelay(delayUs);

	if (strcmp (dma_res, "OK") != 0) return HM10_ERROR;

	return OK;
}

char* concat_str(char * cmd, char mode) {
	const size_t len1 = strlen(cmd);

	char *result = malloc(len1 + 4);

	strcpy(result, cmd);
	result[len1] = mode;
	strcat(result, "\r");
	strcat(result, "\n");

	return result;
}

char* concat_cmd_str(char * cmd, char * str) {
	const size_t len_cmd = strlen(cmd);
	const size_t len_str = strlen(str);

	char *result = malloc(len_cmd + len_str + 4);

	strcpy(result, cmd);
	strcat(result, str);
	strcat(result, "\r");
	strcat(result, "\n");

	return result;
}

void clearingBuf() {
	memset(dma_res, 0, strlen(dma_res));
}
