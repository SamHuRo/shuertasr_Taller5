/*
 * PLLDrivers.h
 *
 *  Created on: May 16, 2023
 *      Author: samuel
 */

#ifndef PLLDRIVER_H_
#define PLLDRIVER_H_

#include <stm32f4xx.h>
#include "GPIOxDriver.h"

#define PLLP_2		0
#define	PLLP_4		1
#define PLLP_6		3
#define PLLP_8		4

#define PLL_MCO1PRE_2	0
#define PLL_MCO1PRE_3	1
#define PLL_MCO1PRE_4	2
#define PLL_MCO1PRE_5	3

//Definicion del PLL
typedef struct
{
	uint8_t 	PLL_PLLM; //El valor que va a tomar el registro PLLM
	uint16_t	PLL_PLLN; //El valor que va a tomar el registro PLLN
	uint8_t		PLL_PLLP; //El valor que va a tomar el registro PLLP
	uint8_t		PLL_MCO1PRE; //MCO1 prescaler
}PLL_Config_t;

//Funcion para la configuracion del PLL
void ConfigPLL(PLL_Config_t *pPLLHandler);
//Funcion para entregar el estado de la configuracion del equipo
uint16_t getConfigPLL(void);

#endif /* PLLDRIVER_H_ */
