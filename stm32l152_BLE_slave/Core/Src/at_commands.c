/*
 * at_commands.c
 *
 *  Created on: 25 янв. 2024 г.
 *      Author: Andrianov Vitaly
 */

#include "at_commands.h"

const char *AT_commands[] = {
		"AT\r\n",
		"AT+INQ\r\n",
		"AT+INQM",
		"AT+ROLE0",
		"AT+VERSION?",
		"AT+BAUD\r\n",
		"AT+SHOW",
		"AT+NAME",
		"AT+DISI?",
		"AT+CONN1\r\n",
		"AT+RSSI?",
		"AT+IBEA0\r\n",
		"AT+HELP\r\n",
		"AT+DEFAULT\r\n",
		"AT+RENEW\r\n",
		"AT+IMME1",
		"AT+RESET\r\n",
		"AT+DISC?",
		"AT+MODE?",
		"AT+SCAN1",
		"AT+CONNL",
		"AT+CONNL",
		"AT+MEA",
		"AT+NOTI1\r\n",
		"AT+ROLE0\r\n"
};
