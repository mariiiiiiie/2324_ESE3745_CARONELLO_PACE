/*
 * shellv2.c
 *
 *  Created on: Nov 8, 2024
 *      Author: romingo
 */
#define V1

#include "mylibs/shellv2.h"
#define ESC 0x1B

// États pour la machine à états de la séquence d’échappement
typedef enum {
	STATE_WAIT_ESC,
	STATE_WAIT_BRACKET,
	STATE_WAIT_FINAL_CHAR,
	ARROW_UP,
	ARROW_DOWN,
} EscapeState;

EscapeState escState = STATE_WAIT_ESC;
uint8_t uart_data;



uint8_t prompt[]="user@Nucleo-STM32G474RET6>>";
uint8_t started[]=
		"\r\n*-----------------------------*"
		"\r\n| Welcome on Nucleo-STM32G474 |"
		"\r\n*-----------------------------*"
		"\r\n";
uint8_t newline[]="\r\n";
uint8_t backspace[]="\b \b";
uint8_t cmdNotFound[]="Command not found\r\n";
char brian[]="Brian is in the kitchen\r\n";
uint8_t uartRxReceived;
uint8_t uartRxBuffer[UART_RX_BUFFER_SIZE];
uint8_t uartTxBuffer[UART_TX_BUFFER_SIZE];

/****** NEW ARGS ******/
extern uint32_t dPulse;

uint8_t cmdHistory[CMD_HISTORY_SIZE][CMD_BUFFER_SIZE];
uint8_t cmdHistory_index_save=0;
uint8_t cmdHistory_index_nav=0;
uint32_t rpmMax = 3000;
uint32_t pulseGoal_1;// Partagé avec IT.h
uint32_t pulseGoal_2;// Partagé avec IT.h

char* error=				"AN ISSUE OCCURED";
char* info_PWMs=			"Timer PWM ";
char* info_TIMERs=			"Timer Updater Pulse";
char* info_PWM_1=		 	"PWM CHANNEL 1";
char* info_PWMN_1=	 		"PWMN CHANNEL 1";
char* info_PWMN_2=			"PWM CHANNEL 2";
char* info_PWM_2=			"PWWN CHANNEL 2";
char* stopMessage=			"PWM and Timer Updater Pulse";
char* speedNotAvailable= 	"Too high value, max speed instead";
char* alreadyStarted= 		"PWM & Timer Updater Pulse are already started";
char* alreadyStop=			"PWM & Timer Updater Pulse are already stopped";
char* changement_dPulse= 	"Changement de dPulse";
char	 	cmdBuffer[CMD_BUFFER_SIZE];
int 		idx_cmd;
char* 		argv[MAX_ARGS];
int		 	argc = 0;
char*		token;
int 		newCmdReady = 0;
int 		isFind=0;
int 		isStarted=0;

MAPPER mapping[] = {
		{"help", "Print every function available","None", subfunct_help},
		{"WhereisBrian", "If you don't know where brian is","None", subfunct_WherisBrian},
		{"speed", "Change the speed of the DC motor", "str:RPM", subfunct_speed},
		{"start", "Start PWM, DC Motor set up with the minimum speed","None", subfunct_start},
		{"stop", "Stop PWM","None",subfunct_stop},
		{"printADC","Défini l'incrément  pour la transition de a consigne moteur","int:dPulse",subfunct_printADC},
};


void Shell_Init(void){
	memset(argv, 0, MAX_ARGS*sizeof(char*));
	memset(cmdBuffer, 0, CMD_BUFFER_SIZE*sizeof(char));
	memset(uartRxBuffer, 0, UART_RX_BUFFER_SIZE*sizeof(char));
	memset(uartTxBuffer, 0, UART_TX_BUFFER_SIZE*sizeof(char));

	HAL_UART_Receive_IT(&huart2, uartRxBuffer, UART_RX_BUFFER_SIZE);
	HAL_UART_Transmit(&huart2, started, strlen((char *)started), HAL_MAX_DELAY);
	HAL_UART_Transmit(&huart2, prompt, strlen((char *)prompt), HAL_MAX_DELAY);
	subfunct_start();
}

void Shell_Loop(void){

	if(uartRxReceived==1){
		switch(uartRxBuffer[0]){
		case ASCII_CR: // Nouvelle ligne, instruction à traiter
			HAL_UART_Transmit(&huart2, newline, sizeof(newline), HAL_MAX_DELAY);
			cmdBuffer[idx_cmd] = '\0';
			//On copie la commande dans l'historique
			strcpy(cmdHistory[cmdHistory_index_save % CMD_HISTORY_SIZE], cmdBuffer);
			cmdHistory_index_save++;
			argc = 0;
			token = strtok(cmdBuffer, " ");
			while(token!=NULL){
				argv[argc++] = token;
				token = strtok(NULL, " ");
			}
			idx_cmd = 0;
			newCmdReady = 1;
			break;
		case ASCII_BACK: case ASCII_BACK_BIS: // Suppression du dernier caractère
			if (idx_cmd==0){break;}
			cmdBuffer[idx_cmd--] = '\0';
			HAL_UART_Transmit(&huart2, backspace, sizeof(backspace), HAL_MAX_DELAY);
			break;
		default:
			cmdBuffer[idx_cmd++] = uartRxBuffer[0];
			HAL_UART_Transmit(&huart2, uartRxBuffer, UART_RX_BUFFER_SIZE, HAL_MAX_DELAY);
		}
		uartRxReceived = 0;
	}


	else if(uartRxReceived==2){
		uartRxReceived=1;
		switch (escState) {

		case ARROW_DOWN:
			if(cmdHistory_index_nav==0){break;}
			strcpy(uartRxBuffer,cmdHistory[cmdHistory_index_nav]);			//On recupere le messsage sauvegarde et on l'associe au buffer actuel
			idx_cmd = sizeof(cmdHistory[cmdHistory_index_nav])/sizeof(cmdHistory[cmdHistory_index_nav][0]);			//On actualise le idx_cmd pour pouvoir ecrire ou modifier le msg
			cmdHistory_index_nav--;
			HAL_UART_Transmit(&huart2, uartRxBuffer, UART_RX_BUFFER_SIZE, HAL_MAX_DELAY);
			break;
		case ARROW_UP:
			if(cmdHistory_index_nav==CMD_HISTORY_SIZE){break;}
			strcpy(uartRxBuffer,cmdHistory[cmdHistory_index_nav]);			//On recupere le messsage sauvegarde et on l'associe au buffer actuel
			idx_cmd = sizeof(cmdHistory[cmdHistory_index_nav])/sizeof(cmdHistory[cmdHistory_index_nav][0]);			//On actualise le idx_cmd pour pouvoir ecrire ou modifier le msg
			cmdHistory_index_nav++;
			HAL_UART_Transmit(&huart2, uartRxBuffer, UART_RX_BUFFER_SIZE, HAL_MAX_DELAY);
			break;
		}
	}




	if(newCmdReady){
		for (int i=0;i<sizeof(mapping) / sizeof(mapping[0]);i++){
			if (strcmp(argv[0],mapping[i].name)==0){
				mapping[i].funct();
				isFind=1;
			}
		}
		if(!isFind){
			HAL_UART_Transmit(&huart2, cmdNotFound, sizeof(cmdNotFound), HAL_MAX_DELAY);
		}

		HAL_UART_Transmit(&huart2, prompt, sizeof(prompt), HAL_MAX_DELAY);
		newCmdReady = 0;
		isFind=0;
	}
}
#ifdef V2
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
	switch (escState) {
	case STATE_WAIT_ESC:
		if (uartRxBuffer[0] == ESC) {
			escState = STATE_WAIT_BRACKET;
			uartRxReceived=0;
		}else
		{
			uartRxReceived=1;
		}
		break;

	case STATE_WAIT_BRACKET:
		if (uartRxBuffer[0] == '[') {
			escState = STATE_WAIT_FINAL_CHAR;
		} else {
			escState = STATE_WAIT_ESC; // Si incorrect, reset
			uartRxReceived=1;
		}
		break;

	case STATE_WAIT_FINAL_CHAR:
		uartRxReceived=2;
		switch (uartRxBuffer[0]) {
		case 'A': // Flèche Haut
			// Action flèche haut
			escState =ARROW_UP;
			break;
		case 'B': // Flèche Bas
			// Action flèche bas
			escState =ARROW_DOWN;
			break;
		case 'C': // Flèche Droite
			break;
		case 'D': // Flèche Gauche
			break;
		default:
			uartRxReceived=1;
			escState = STATE_WAIT_ESC; // Réinitialiser l'état
			break;
		}
	}

	// Redémarrer la réception
	HAL_UART_Receive_IT(&huart2, uartRxBuffer, UART_RX_BUFFER_SIZE);
}
#endif

#ifdef V1
void HAL_UART_RxCpltCallback(UART_HandleTypeDef * huart){

	uartRxReceived=1;
	HAL_UART_Receive_IT(&huart2, uartRxBuffer, UART_RX_BUFFER_SIZE);

}
#endif
/************************************************************************************************
 * 										SUBFUNCTIONS
 *************************************************************************************************/
void subfunct_WherisBrian(void){
	printf(brian);

}
void subfunct_help(void){
	printf("%s%-15s | %-10s | %-50s\r\n%s%s", GRAS,"Fonctions", "Params", "Description",RESET,separator);
	for (int i=0;i<sizeof(mapping) / sizeof(mapping[0]);i++){
		printf("%s%-15s%s | %-10s | %-50s\r\n",FUNCTION, mapping[i].name,RESET, mapping[i].params, mapping[i].resume);
	}
}
void subfunct_speed(void){
	uint32_t alpha = atoi(argv[1]);

	pulseGoal_1 = alpha * htim1.Instance->ARR /100;
	pulseGoal_2 = htim1.Instance->ARR - pulseGoal_1;
	/*
	 * htim2 is going to actualise pulses every 1ms: CHECK Interruptions for more details
	 */
}
void subfunct_start(void){
	if (isStarted){debug(WARNING,alreadyStarted);return;}
	HAL_TIM_Base_Start(&htim1)!=HAL_OK? debug(ERROR,error):debug(START,info_PWMs);
	HAL_TIM_Base_Start_IT(&htim2)!=HAL_OK? debug(ERROR,error):debug(START,info_TIMERs);//Timer for smooth transition pulses

	pulseGoal_1 = htim1.Instance->ARR/2;
	pulseGoal_2 = htim1.Instance->ARR/2;
	__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, pulseGoal_1);
	__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_2, pulseGoal_2);
	//Channel 1
	HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1)!=HAL_OK? debug(ERROR,error):debug(START,info_PWM_1);
	HAL_TIMEx_PWMN_Start(&htim1, TIM_CHANNEL_1)!=HAL_OK? debug(ERROR,error):debug(START,info_PWMN_1);

	//Channel 2
	HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_2)!=HAL_OK? debug(ERROR,error):debug(START,info_PWM_2);
	HAL_TIMEx_PWMN_Start(&htim1, TIM_CHANNEL_2)!=HAL_OK? debug(ERROR,error):debug(START,info_PWMN_2);
	isStarted=1;
	pulseGoal_1= htim1.Instance->ARR;
	pulseGoal_2 = htim1.Instance->ARR - pulseGoal_1;
}
void subfunct_stop(void){
	if (!isStarted){debug(WARNING,alreadyStop);return;}
	HAL_TIM_Base_Stop(&htim1)!=HAL_OK? debug(ERROR,error):debug(STOP,info_PWMs);
	HAL_TIM_Base_Stop(&htim2)!=HAL_OK? debug(ERROR,error):debug(STOP,info_TIMERs);//Timer for smooth transition pulses

	HAL_TIM_PWM_Stop(&htim1, TIM_CHANNEL_1)!=HAL_OK? debug(ERROR,error):debug(STOP,info_PWM_1);
	HAL_TIMEx_PWMN_Stop(&htim1, TIM_CHANNEL_1)!=HAL_OK? debug(ERROR,error):debug(STOP,info_PWMN_1);
	HAL_TIM_PWM_Stop(&htim1, TIM_CHANNEL_2)!=HAL_OK? debug(ERROR,error):debug(STOP,info_PWM_2);
	HAL_TIMEx_PWMN_Stop(&htim1, TIM_CHANNEL_2)!=HAL_OK? debug(ERROR,error):debug(STOP,info_PWMN_2);
	isStarted=0;
}
void subfunct_printADC(void){

	return;
}
/************************************************************************************************
 * 										DEBUG
 *************************************************************************************************/


void debug(char* debugType,char* message){

	printf("%-15s%s| %s\r\n",debugType,RESET,message);

}
