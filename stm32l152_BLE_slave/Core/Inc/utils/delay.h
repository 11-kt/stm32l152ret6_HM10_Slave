/*
 * delay.h
 *
 *  Created on: Feb 13, 2024
 *      Author: Andrianov Vitaly
 */

#ifndef INC_UTILS_DELAY_H_
#define INC_UTILS_DELAY_H_

#include "main.h"

extern TIM_HandleTypeDef htim2;

void usDelay(uint32_t us);

#endif /* INC_UTILS_DELAY_H_ */
