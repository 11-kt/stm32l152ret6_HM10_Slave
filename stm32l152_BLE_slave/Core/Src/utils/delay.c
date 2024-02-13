/*
 * delay.c
 *
 *  Created on: Feb 13, 2024
 *      Author: Andrianov Vitaly
 */

#include "utils/delay.h"

void usDelay(uint32_t us) {
	__HAL_TIM_SET_COUNTER(&htim2, 0);
	__HAL_TIM_CLEAR_FLAG(&htim2, TIM_FLAG_UPDATE);
	while(__HAL_TIM_GET_COUNTER(&htim2) < us) {
		if (__HAL_TIM_GET_FLAG(&htim2, TIM_FLAG_UPDATE) != RESET) {
			break;
		}
	}
}
