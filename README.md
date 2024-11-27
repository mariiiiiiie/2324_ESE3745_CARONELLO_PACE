# 2324_ESE3745_CARONELLO_PACE

## Séance 1 - Commande MCC Basique
_Objectifs :_

- Générer 4 PWM en complémentaire décalée pour contrôler en boucle ouverte le moteur en respectant le cahier des charges,
-  Inclure le temps mort,
- Vérifier les signaux de commande à l'oscilloscope,
- Prendre en main le hacheur,
- Faire un premier essai de commande moteur.

### Génération 4 PWM
_Cahier des charges :_
- Fréquence de la PWM : 20kHz
- Temps mort minimum : à voir selon la datasheet des transistors
- Résolution minimum : 10bits.

Le temps mort minimum pour les transistors est définit dans la datasheet. S'il l'on choisisait d'optimiser au maximum on ferait..

Cependant, par mesure de précautions, on choisit un temp mort identique pour le temps de montée et de descente à ...

[photo]

[oscilloscope]

### Commande de vitesse

Pour associer une commande à un mot clé entrée dans le terminal on référence le mot clé à une fonction dans une structure `MAPPER` définit si dessous :
```c
typedef struct{
	char* name;
	char* resume;
	char* params;
	void (*funct)();
}MAPPER;
```
On implément ensuite dans un variabel globale le mapping. De cette facon ile st très simple d'ajouter une fonction appellable par le shell.
```c
MAPPER mapping[] = {
		{"help", "Print every function available","None", subfunct_help},
		{"WhereisBrian", "If you don't know where brian is","None", subfunct_WherisBrian},
		{"speed", "Change the speed of the DC motor", "str:RPM", subfunct_speed},
		{"start", "Start PWM, DC Motor set up with the minimum speed","None", subfunct_start},
		{"stop", "Stop PWM","None",subfunct_stop},
		{"adc","Défini l'incrément  pour la transition de a consigne moteur","int:dPulse",subfunct_printADC},
		{"asservI","None","int: mA",subfunct_asservI},
};
```
Pour chaque caractère traité, on va vérifier si c'est le caractère `Entrée`. Si ce n'est pas le cas alors on ajoute le caractère au buffer `uartRxBuffer`, autrement on termine l'écoute de l'uart en séparant tous les arguments séparés par un espace dans `argv[]`.
```c
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
    [...Suite du code...]   
}
```
Lorsque le shell à finit de reçevoir une commande. Il faut qu'il parcourt l'entièreté du `MAPPER mapping` pour vérifier que la fonction est repertoriée. Si elle est repertoriée alors utilise la strucutre pour trouver la fonction associée.
```c
if(newCmdReady){
		for (int i=0;i<sizeof(mapping) / sizeof(mapping[0]);i++){
			if (strcmp(argv[0],mapping[i].name)==0){
				mapping[i].funct();
				isFind=1;
			}
		}
    [...Suite du code...]
}
```

De cette manière on est capable de traiter une commande `speed xxxx`.
Nous choisissons que l'argument de `speed` sera le rapport cyclique de la PWM.

La focntion qui traite l'entrée `speed xxxx` est la fonction `subfunct_start` elle sera décrite dans une premier temps comme ci dessous: 
```c
void subfunct_speed(void){
	uint32_t alpha = atoi(argv[1]);

	pulseGoal_1 = alpha * htim1.Instance->ARR /100;
	pulseGoal_2 = htim1.Instance->ARR - pulseGoal_1;
	__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, pulseGoal_1);
    __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_2, pulseGoal_2);
}
```
### Premiers tests
Cette facon de faire n'est pas optimale car l'appel en courant est excessivement important lorsque la consigne de vitesse passe de 0% à ±100%.

On décide donc de créer une consigne de vitesse en rampe. Pour faire cela, on créer un timer `TIM2` synchonisé sur toute les 1ms. Plutôt que de mettre à jour le pulse dans la fonction `subfunct_speed`, on va le faire progressivement dans le callBack du `TIM2`. 
```c
void subfunct_speed(void){
	uint32_t alpha = atoi(argv[1]);

	pulseGoal_1 = alpha * htim1.Instance->ARR /100;
	pulseGoal_2 = htim1.Instance->ARR - pulseGoal_1;
	/*
	 * htim2 is going to actualise pulses every 1ms: CHECK Interruptions for more details
	 */
}
```
```c
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef* htim){

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
    [...Suite du code...]
}
```

De cette facon on obtiens un vitesse progressive qui limite l'appel en courant par le moteur.
## Séance 2 - Commande En Boucle Ouverte, Mesure De Vitesse Et De Courant
_Objectifs :_

- Commander en boucle ouverte le moteur avec une accélération limitée,
- Mesurer le courant aux endroits adéquat dans le montage,
- Mesurer la vitesse à partir du codeur présent sur chaque moteur.
### Commande de la vitesse
Dans cette partie on met un point d'honneur sur la lisibilité du code afin que tout les appels des fonctions `HAL_XXX_()` soit surveiller, pour qu'en cas d'échec l'utilisateur du shell soit averti.

### Mesure du courant

[kicad]
Pour mesurer correctement 
[config_.ioc]
[photo_oscillo_output_findeADC]
## Séance 3 - Asservissement 

## Génération de Doxyfile
- Installer doxyfile
    - MacOs : 
    ````
    brew install doxygen
    ````
    - Linux : 
    ```
    sudo apt install doxygen
    ```
- Créer un doxyfile dans votre répertoire 
```bash
doxygen -g
open Doxyfile
```
- Pour que le doxyfile ne documente que des inputs particulier:
````bash
INPUT                  = NUCLEO-G474RET6-Inverter_Pinout/Core/
````
- Compiler le doxygen 
````bash
doxygen doxyfile
open html/index.html
```` 
