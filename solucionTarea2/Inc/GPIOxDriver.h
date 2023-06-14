/*
 * GPIOxDriver.h
 *
 *  Created on: 9/03/2023
 *      Author: Samuel Huertas Rojas
 *  Este programa se encarga de las definiciones realcionadas únicamente con el debido manejo
 *  y control del periférico GPIOx.
 */

#ifndef GPIOXDRIVER_H_
#define GPIOXDRIVER_H_

//Incluir el archivo stm32f411xx_hal.h
#include "stm32f411xx_hal.h"

//Definición del pin
typedef struct
{
	uint8_t GPIO_PinNumber;			//Pin con el que deseamos trabajar
	uint8_t GPIO_PinMode;			//Modo de la configuración: entrada, salida, análoga, función alternativa
	uint8_t GPIO_PinSpeed;			//Velocidad de respuestadel Pin
	uint8_t GPIO_PinPuPdControl;	//Seleccionamos si deseamos una salida Pull-up, Pull-down o "libre"
	uint8_t GPIO_PinOPType;			//Trabaja de la mano con el anterior, selecciona salida PUPD o OpenDrain
	uint8_t GPIO_PinAltFunMode;		//Selecciona cual es la función alternativa que se esta configurando
} GPIO_PinConfig_t;

/* Definición de la estructura Handler, la cual contiene dos elementos:
 * - La dirección del puerto que se esta utilizando (se escribe la referencia del puerto)
 * - La configuración específica del Pin que se está utilizando
 */
typedef struct
{
	GPIOx_RegDef_t		*pGPIOx; //Dirección del puerto al que el Pin corresponde
	GPIO_PinConfig_t	GPIO_PinConfig; //Configuracióndel Pin
} GPIO_Handler_t;

/*Definición de las cabeceras de las funciones del GPIOxDriver*/
void GPIO_Config (GPIO_Handler_t *pGPIOHandler);
void GPIO_WritePin (GPIO_Handler_t *pPinHandler, uint8_t newState);
uint32_t GPIOReadPin(GPIO_Handler_t *pPinHandler);
void GPIO_TooglePin(GPIO_Handler_t *pPinHandler);

#endif /* GPIOXDRIVER_H_ */
