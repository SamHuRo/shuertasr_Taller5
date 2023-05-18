/*
 * CintaLedDriver.h
 *
 *  Created on: May 17, 2023
 *      Author: samuel
 */

#ifndef CINTALEDDRIVER_H_
#define CINTALEDDRIVER_H_

#include <stm32f4xx.h>

#include "GPIOxDriver.h"
#include "PLLDriver.h"

/*Funciones para escribir uno o cero en la cinta led*/
void Uno(GPIO_Handler_t *pGPIOHandler);
void Cero(GPIO_Handler_t *pGPIOHandler);

/*Funciones para escribir los colores en la cinta led*/
void Verde(GPIO_Handler_t *pGPIOHandler);
void Rojo(GPIO_Handler_t *pGPIOHandler);
void Azul(GPIO_Handler_t *pGPIOHandler);

#endif /* CINTALEDDRIVER_H_ */
