/*
 * recieveData.h
 *
 *  Created on: 25 янв. 2024 г.
 *      Author: Andrianov Vitaly
 */

#ifndef INC_RECIEVEDATA_H_
#define INC_RECIEVEDATA_H_

#include "stm32l1xx_hal.h"
#include "main.h"

extern UART_HandleTypeDef huart4;

typedef struct {
  uint8_t usart_buf[60];
  uint8_t usart_cnt;
} USART_buf;

void string_parse(char* buf_str);
void UART4_RxCpltCallback(void);

#endif /* INC_RECIEVEDATA_H_ */
