/*
 * shellv2.h
 *
 *  Created on: Nov 8, 2024
 *      Author: romingo
 */

#ifndef INC_MYLIBS_SHELLV2_H_
#define INC_MYLIBS_SHELLV2_H_
#include "usart.h"
#include "main.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define UART_RX_BUFFER_SIZE 1
#define UART_TX_BUFFER_SIZE 64
#define CMD_BUFFER_SIZE 64
#define CMD_HISTORY_SIZE 10

#define MAX_ARGS 9
#define ASCII_LF 0x0A			// LF = line feed, saut de ligne
#define ASCII_CR 0x0D			// CR = carriage return, retour chariot
#define ASCII_BACK 0x08			// BACK = Backspace
#define ASCII_BACK_BIS 0x7F
#define ASCII_ESC_1 0x1B
#define ASCII_ESC_2 0x5B
#define ASCII_ARROW_UP 0x41
#define ASCII_ARROW_DOWN 0x42

extern TIM_HandleTypeDef htim1;
extern TIM_HandleTypeDef htim2;
extern ADC_HandleTypeDef hadc1;


void Shell_Init(void);
void Shell_Loop(void);
void subfunct_WherisBrian(void);
void subfunct_help(void);
void subfunct_speed(void);
void subfunct_start(void);
void subfunct_stop(void);
void subfunct_printADC(void);
void subfunct_asservI(void);
void debug(char* debugType,char* message);
typedef struct{
	char* name;
	char* resume;
	char* params;
	void (*funct)();
}MAPPER;

/************************************************************************************************
 * 										DEBUG
 *************************************************************************************************/#define separator "--------------------------------------------------------------------------\r\n"
#define INFORMATION 	"\033[35mINFORMATION"   // Code couleur Magenta
#define WARNING    		"\033[33mWARNING"    // Code couleur Jaune
#define START 			"\033[32mSTART"
#define OK 				"\033[32mOK"
#define STOP   			"\033[31mSTOP"    // Code couleur Rouge
#define ERROR   		"\033[31mERROR"    // Code couleur Rouge
#define GRAS 			"\033[1m"
#define FUNCTION 		"\033[35m"


#define RESET 			"\033[0m"         // Réinitialise la couleur

#endif /* INC_MYLIBS_SHELLV2_H_ */
