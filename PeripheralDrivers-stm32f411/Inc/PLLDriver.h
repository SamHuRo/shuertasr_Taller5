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

//Definicion del PLL
typedef struct
{
	uint8_t 	PLL_PLLM; //El valor que va a tomar el registro PLLM
	uint32_t	PLL_PLLN; //El valor que va a tomar el registro PLLN
	uint8_t		PLL_PLLP; //El valor que va a tomar el registro PLLP
}PLL_Config_t;


void ConfigPLL(PLL_Config_t *pPLLHandler);
void getConfigPLL(void);

#endif /* PLLDRIVER_H_ */
