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

#define PRESC_APB1_2	0
#define PRESC_APB1_4	1
#define PRESC_APB1_8	2
#define PRESC_APB1_16	3
#define PRESC_APB1_NONE	4

#define PRESC_APB2_2	0
#define PRESC_APB2_4	1
#define PRESC_APB2_8	2
#define PRESC_APB2_16	3
#define PRESC_APB2_NONE	4

#define PLL_DISABLE		0
#define PLL_ENABLE		1

//Definicion del PLL
typedef struct
{
	uint8_t 	PLL_PLLM; 		//El valor que va a tomar el registro PLLM
	uint16_t	PLL_PLLN; 		//El valor que va a tomar el registro PLLN
	uint8_t		PLL_PLLP; 		//El valor que va a tomar el registro PLLP
	uint8_t		PLL_MCO1PRE; 	//MCO1 prescaler
	uint8_t 	PLL_ON;			//Si el PLL se va a activar o no
	uint8_t 	APB1_PRESC;		//El prescaler que va a tener el APB1
	uint8_t 	APB2_PRESC;		//El prescaler que va a tener el APB2
}PLL_Config_t;

//Funcion para la configuracion del PLL
void ConfigPLL(PLL_Config_t *pPLLHandler);
//Funcion para entregar el estado de la configuracion del equipo
uint16_t getConfigPLL(void);

#endif /* PLLDRIVER_H_ */
