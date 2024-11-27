/*
 * callBack.c
 *
 *  Created on: Nov 22, 2024
 *      Author: romingo
 */

#ifndef CALLBACK_C_
#define CALLBACK_C_

#include "callBack.h"

extern int isADC;
extern uint32_t pulseGoal_1;
extern uint32_t pulseGoal_2;

void CUSTOM_HAL_TIM_PeriodElapsedCallBack(TIM_HandleTypeDef* htim){

	if(htim->Instance== TIM2){ // Timer for smooth transition
		uint32_t pulse_1 = htim1.Instance->CCR1;
		uint32_t pulse_2 = htim1.Instance->CCR2;

		if(pulse_1!= pulseGoal_1){
			int8_t sign_1 = pulse_1 > pulseGoal_1?
					-1:1;
			__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, pulse_1 + sign_1);
		}
		if(pulse_2!=pulseGoal_2){
			int8_t sign_2 = pulse_2 > pulseGoal_2?
					-1:1;
			__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_2, pulse_2 + sign_2);
		}
	}
}


void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc){
	if (hadc->Instance == ADC1){ //TIM1 Trigger the start of conversion
		HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_6);
		isADC = 1;
	}
}

#endif /* CALLBACK_C_ */
