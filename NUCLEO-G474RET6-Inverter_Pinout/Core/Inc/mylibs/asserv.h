/*
 * asserv.h
 *
 *  Created on: Nov 22, 2024
 *      Author: romingo
 */

#ifndef INC_MYLIBS_ASSERV_H_
#define INC_MYLIBS_ASSERV_H_

#define MAX_U_MOTOR 48 //Volts

#include "stdio.h"
#include "stdlib.h"
#include "stdint.h"
#include "tim.h"

extern TIM_HandleTypeDef htim1;

void asservCourant(uint32_t* ADC_VAL);

#endif /* INC_MYLIBS_ASSERV_H_ */
