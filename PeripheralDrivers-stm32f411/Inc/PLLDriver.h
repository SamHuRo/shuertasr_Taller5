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

void ConfigPLL(void);
void Uno(GPIO_Handler_t *pGPIOHandler);
void Cero(GPIO_Handler_t *pGPIOHandler);

#endif /* PLLDRIVER_H_ */
