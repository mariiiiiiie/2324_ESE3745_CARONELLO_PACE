/*
 * shell.c
 *
 *  Created on: Oct 1, 2023
 *      Author: nicolas
 */
#include "usart.h"
#include "mylibs/shell.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

uint8_t prompt[]="user@Nucleo-STM32G474RET6>>";
uint8_t started[]=
		"\r\n*-----------------------------*"
		"\r\n| Welcome on Nucleo-STM32G474 |"
		"\r\n*-----------------------------*"
		"\r\n";
uint8_t newline[]="\r\n";
uint8_t backspace[]="\b \b";
uint8_t cmdNotFound[]="Command not found\r\n";
uint8_t brian[]="Brian is in the kitchen\r\n";
uint8_t speedNotAvailable[]="This speed is not available -> Speed max instead\r\n";
uint8_t uartRxReceived;
uint8_t uartRxBuffer[UART_RX_BUFFER_SIZE];
uint8_t uartTxBuffer[UART_TX_BUFFER_SIZE];
/****** NEW ARGS ******/
uint32_t rpmMax = 3000;
uint32_t pulseGoal_1;// Partagé avec IT.h
uint32_t pulseGoal_2;// Partagé avec IT.h
uint8_t startMessage[] = "Start PWM and Transition Timer: OK\r\n";
uint8_t stopMessage[]="Stop PWM and Transition Timer: OK\r\n";


char	 	cmdBuffer[CMD_BUFFER_SIZE];
int 		idx_cmd;
char* 		argv[MAX_ARGS];
int		 	argc = 0;
char*		token;
int 		newCmdReady = 0;

void Shell_Init(void){
	memset(argv, 0, MAX_ARGS*sizeof(char*));
	memset(cmdBuffer, 0, CMD_BUFFER_SIZE*sizeof(char));
	memset(uartRxBuffer, 0, UART_RX_BUFFER_SIZE*sizeof(char));
	memset(uartTxBuffer, 0, UART_TX_BUFFER_SIZE*sizeof(char));

	HAL_UART_Receive_IT(&huart2, uartRxBuffer, UART_RX_BUFFER_SIZE);
	HAL_UART_Transmit(&huart2, started, strlen((char *)started), HAL_MAX_DELAY);
	HAL_UART_Transmit(&huart2, prompt, strlen((char *)prompt), HAL_MAX_DELAY);
}

void Shell_Loop(void){
	if(uartRxReceived){
		switch(uartRxBuffer[0]){
		case ASCII_CR: // Nouvelle ligne, instruction à traiter
			HAL_UART_Transmit(&huart2, newline, sizeof(newline), HAL_MAX_DELAY);
			cmdBuffer[idx_cmd] = '\0';
			argc = 0;
			token = strtok(cmdBuffer, " ");
			while(token!=NULL){
				argv[argc++] = token;
				token = strtok(NULL, " ");
			}
			idx_cmd = 0;
			newCmdReady = 1;
			break;
		case ASCII_BACK: // Suppression du dernier caractère
			cmdBuffer[idx_cmd--] = '\0';
			HAL_UART_Transmit(&huart2, backspace, sizeof(backspace), HAL_MAX_DELAY);
			break;

		default: // Nouveau caractère
			cmdBuffer[idx_cmd++] = uartRxBuffer[0];
			HAL_UART_Transmit(&huart2, uartRxBuffer, UART_RX_BUFFER_SIZE, HAL_MAX_DELAY);
		}
		uartRxReceived = 0;
	}

	if(newCmdReady){
		if(strcmp(argv[0],"WhereisBrian?")==0){
			HAL_UART_Transmit(&huart2, brian, sizeof(brian), HAL_MAX_DELAY);
		}
		else if(strcmp(argv[0],"help")==0){
			int uartTxStringLength = snprintf((char *)uartTxBuffer, UART_TX_BUFFER_SIZE, "Print all available functions here\r\n");
			HAL_UART_Transmit(&huart2, uartTxBuffer, uartTxStringLength, HAL_MAX_DELAY);
		}
		else if (strcmp(argv[0], "speed")==0){
			uint32_t speed = atoi(argv[1]);
			if (speed > 3000){
				speed = 3000;
				HAL_UART_Transmit(&huart2,speedNotAvailable , sizeof(speedNotAvailable), HAL_MAX_DELAY);
			}
			pulseGoal_1= ((uint32_t)htim1.Instance->ARR * (uint32_t)(100*speed/rpmMax))/100;
			pulseGoal_2 = ((uint32_t)htim1.Instance->ARR * (uint32_t)(100-100*speed/rpmMax))/100;
			// htim2 is going to actualise pulses every 1ms, smoothly: CHECK Interruptions for more details
		}
		else if (strcmp(argv[0], "start")==0){
			HAL_TIM_Base_Start(&htim1);
			HAL_TIM_Base_Start_IT(&htim2); //Timer for smooth transition pulses

			__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, htim1.Instance->ARR/2);
			__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_2, htim1.Instance->ARR/2);

			//Channel 1
			HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);
			HAL_TIMEx_PWMN_Start(&htim1, TIM_CHANNEL_1);

			//Channel 2
			HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_2);
			HAL_TIMEx_PWMN_Start(&htim1, TIM_CHANNEL_2);

			HAL_UART_Transmit(&huart2,startMessage , sizeof(startMessage), HAL_MAX_DELAY);
		}
		else if (strcmp(argv[0], "stop")==0){
			HAL_TIM_Base_Stop(&htim1);
			HAL_TIM_Base_Stop_IT(&htim2);
			HAL_UART_Transmit(&huart2,stopMessage , sizeof(stopMessage), HAL_MAX_DELAY);

		}
		else{
			HAL_UART_Transmit(&huart2, cmdNotFound, sizeof(cmdNotFound), HAL_MAX_DELAY);
		}

		HAL_UART_Transmit(&huart2, prompt, sizeof(prompt), HAL_MAX_DELAY);
		newCmdReady = 0;
	}
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef * huart){
	uartRxReceived = 1;
	HAL_UART_Receive_IT(&huart2, uartRxBuffer, UART_RX_BUFFER_SIZE);
}
