/*
 * asserv.c
 *
 *  Created on: Nov 22, 2024
 *      Author: romingo
 */

#include "mylibs/asserv.h"
extern int16_t I_CONS;

extern uint32_t pulseGoal_1;
extern uint32_t pulseGoal_2;

uint16_t K_CAPT = 1;
int32_t KI = 1;
int32_t KP = 1;
int32_t erreur[2] = {0,0}; //{ t, t-1 ,t-2...} On stocke les anciennes erreurs


void asservCourant(uint32_t* ADC_VAL){
	// Valeur de courant I avec ADC
	int16_t I_MES_U = K_CAPT* (int16_t)ADC_VAL[0]; // Courant U
	int16_t I_MES_V = K_CAPT* (int16_t)ADC_VAL[1]; // Courant V
	int16_t I_MES = abs(I_MES_U) > abs(I_MES_V) ? I_MES_U:I_MES_V;

	// Fonction de transfertn U=f(I)
	erreur[0] = (int32_t)I_CONS - I_MES;
	int32_t U_COMMANDE = (int32_t)KP * erreur[0] + (int32_t)KI * erreur[1];
	erreur[1]=erreur[0];
	// Transformation de U_COMMANDE en pulseGoal_1/2
	// A Verifier
	if(U_COMMANDE > 0){
		pulseGoal_1 =  (uint32_t)((MAX_U_MOTOR*100 / U_COMMANDE) * htim1.Instance->ARR /100);
		pulseGoal_2 =  (uint32_t)(htim1.Instance->ARR - pulseGoal_1);
	}
	else
	{
		pulseGoal_2 =  (uint32_t)((MAX_U_MOTOR*100 / abs(U_COMMANDE)) * htim1.Instance->ARR /100);
		pulseGoal_1 =  (uint32_t)(htim1.Instance->ARR - pulseGoal_2);
	}


}
