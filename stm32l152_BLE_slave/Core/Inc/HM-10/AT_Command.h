/*
 * AT_Command.h
 *
 *  Created on: Feb 12, 2024
 *      Author: kayaton
 */

#ifndef INC_HM_10_AT_COMMAND_H_
#define INC_HM_10_AT_COMMAND_H_

#include "stm32l1xx_hal.h"

typedef enum {
	AT,
	RESET1,
	RENEW,
	BAUD_GET,
	BAUD_SET,
	CONN,
	IMME_GET,
	IMME_SET,
	NAME_GET,
	NAME_SET,
	ROLE_GET,
	ROLE_SET,
	START
} AT_commands;

typedef enum {
	BAUD_9600,
	BAUD_19200,
	BAUD_38400,
	BAUD_57600,
	BAUD_115200
} hm10_baud;

typedef enum {
	SLAVE,
	MASTER
} hm10_role;

typedef enum {
	BASE,
	ONLY_AT
} hm10_imme;

struct AT_command {
	char * command;
	int res_len;
};

uint8_t * getCommand(AT_commands command);
uint8_t getResLength(AT_commands command);

#endif /* INC_HM_10_AT_COMMAND_H_ */
