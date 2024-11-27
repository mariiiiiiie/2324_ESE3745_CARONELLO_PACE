/*
 * callBack.h
 *
 *  Created on: Nov 22, 2024
 *      Author: romingo
 */

#ifndef CALLBACK_H_
#define CALLBACK_H_

#include "adc.h"
#include "tim.h"

void CUSTOM_HAL_TIM_PeriodElapsedCallBack(TIM_HandleTypeDef* htim);

#endif /* CALLBACK_H_ */
