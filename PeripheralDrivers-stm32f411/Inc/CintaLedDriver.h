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
void Uno(void);
void Cero(void);

/*Funciones para escribir los colores en la cinta led*/
void Verde(void);
void Rojo(void);
void Azul(void);

#endif /* CINTALEDDRIVER_H_ */
