/*
 * HM10_Setup.h
 *
 *  Created on: Feb 12, 2024
 *      Author: kayaton
 */

#ifndef INC_HM_10_HM10_SETUP_H_
#define INC_HM_10_HM10_SETUP_H_

#include "stm32l1xx_hal.h"
#include "HM-10/AT_Command.h"
#include "HM-10/HM10_Setup.h"
#include "utils/delay.h"
#include <string.h>
#include "stdlib.h"

#define delayUs 0x0186A0

typedef enum {
	OK,
	RENEW_OK,
	RESET_OK,
	LOST_AT,
	HM10_ERROR
} setup_result;

setup_result checkConnection(UART_HandleTypeDef *huart);
setup_result setBaudRate(UART_HandleTypeDef *huart, hm10_baud baudrate);
setup_result setRole(UART_HandleTypeDef *huart, hm10_role role);
setup_result setImme(UART_HandleTypeDef *huart, hm10_imme imme);
setup_result setName(UART_HandleTypeDef *huart, char * name);

hm10_baud getBaudRate(UART_HandleTypeDef *huart);
hm10_role getRole(UART_HandleTypeDef *huart);
hm10_imme getImme(UART_HandleTypeDef *huart);

setup_result renewDevice(UART_HandleTypeDef *huart);
setup_result resetDevice(UART_HandleTypeDef *huart);
setup_result startHM10(UART_HandleTypeDef *huart);

setup_result setupSlave(UART_HandleTypeDef *huart);

char* concat_str(char * cmd, char mode);
char* concat_cmd_str(char * cmd, char * str);
void clearingBuf();

#endif /* INC_HM_10_HM10_SETUP_H_ */
