/*
 * atDeviceSetup.h
 *
 *  Created on: 25 янв. 2024 г.
 *      Author: Andrianov Vitaly
 */

#ifndef INC_ATDEVICESETUP_H_
#define INC_ATDEVICESETUP_H_

#include "at_commands.h"
#include "stm32l1xx_hal.h"
#include <string.h>

extern UART_HandleTypeDef huart4;


void factoryResetHM10(HM_10_AT_COMMANDS at_command);
void resetHM10(HM_10_AT_COMMANDS at_command);
void notifyOnHM10(HM_10_AT_COMMANDS at_command);
void setRoleHM10(HM_10_AT_COMMANDS at_command);
void setNameHM10(HM_10_AT_COMMANDS at_command, char* name);
void setupSlaveModule(void);

#endif /* INC_ATDEVICESETUP_H_ */
