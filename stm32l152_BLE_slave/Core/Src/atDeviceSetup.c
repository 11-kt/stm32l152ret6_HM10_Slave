/*
 * atDeviceSetup.c
 *
 *  Created on: 25 янв. 2024 г.
 *      Author: Andrianov Vitaly
 */

#include "atDeviceSetup.h"

void factoryResetHM10(HM_10_AT_COMMANDS at_command) {
	HAL_UART_Transmit(&huart4, (uint8_t *) AT_commands[at_command], strlen(AT_commands[at_command]), 0xFFFF);
}

void resetHM10(HM_10_AT_COMMANDS at_command) {
	HAL_UART_Transmit(&huart4, (uint8_t *) AT_commands[at_command], strlen(AT_commands[at_command]), 0xFFFF);
}

void notifyOnHM10(HM_10_AT_COMMANDS at_command) {
	HAL_UART_Transmit(&huart4, (uint8_t *) AT_commands[at_command], strlen(AT_commands[at_command]), 0xFFFF);
}

void setRoleHM10(HM_10_AT_COMMANDS at_command) {
	HAL_UART_Transmit(&huart4, (uint8_t *) AT_commands[at_command], strlen(AT_commands[at_command]), 0xFFFF);
}

void setNameHM10(HM_10_AT_COMMANDS at_command, char* name) {
	char name_command[60];
	strcpy(name_command, (char *) AT_commands[at_command]);
	strcat(name_command, name);
	strcat(name_command, "\r\n");
	HAL_UART_Transmit(
			&huart4,
			(uint8_t *) name_command,
			strlen(name_command),
			0xFFFF
	);
}

void setupSlaveModule(void) {
	resetHM10(RESET1);

	notifyOnHM10(NOTI1);

	setRoleHM10(NOTI1);

	setNameHM10(NAME, "HM-10_dSLAVE");
}
