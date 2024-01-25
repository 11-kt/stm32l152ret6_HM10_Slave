/*
 * at_commands.h
 *
 *  Created on: 25 янв. 2024 г.
 *      Author: Andrianov Vitaly
 */

#ifndef INC_AT_COMMANDS_H_
#define INC_AT_COMMANDS_H_

extern const char *AT_commands[];

typedef enum {
	AT,
	INQ,
	INQM,
	ROLE,
	VERSION,
	BAUD,
	SHOW,
	NAME,
	DISI,
	CONN,
	RSSI,
	IBEA,
	HELP,
	DEFAULT,
	RENEW,
	IMME,
	RESET1,
	DISC,
	DISA,
	MODE,
	SCAN,
	CONNL,
	MEA,
	NOTI1,
	SLAVE_ROLE
} HM_10_AT_COMMANDS;

#endif /* INC_AT_COMMANDS_H_ */
