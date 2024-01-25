/*
 * recieveData.c
 *
 *  Created on: 25 янв. 2024 г.
 *      Author: Andrianov Vitaly
 */

#include "recieveData.h"
#include <string.h>

char recievedData[60]={0};
USART_buf usartBuffer;

void string_parse(char* buf_str) {
  HAL_UART_Transmit(&huart4,(uint8_t*) buf_str, strlen(buf_str), 0x1000);
  HAL_GPIO_TogglePin(led_GPIO_Port, led_Pin);
}

void UART4_RxCpltCallback(void) {
  uint8_t sub = recievedData[0];
  if (usartBuffer.usart_cnt > 59) {
	  usartBuffer.usart_cnt = 0;
	  HAL_UART_Receive_IT(&huart4, (uint8_t*) recievedData, 1);
	  return;
  }
  usartBuffer.usart_buf[usartBuffer.usart_cnt] = sub;
  if(sub == 0x0A) {
	  usartBuffer.usart_buf[usartBuffer.usart_cnt + 1] = 0;
	  string_parse((char *) usartBuffer.usart_buf);
	  usartBuffer.usart_cnt = 0;
	  HAL_UART_Receive_IT(&huart4, (uint8_t *) recievedData, 1);
	  return;
  }
  usartBuffer.usart_cnt++;
  HAL_UART_Receive_IT(&huart4, (uint8_t *) recievedData, 1);
}
