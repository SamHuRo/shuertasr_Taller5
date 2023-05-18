/*
 * pruebaXXX.c
 *
 *  Created on: May 12, 2023
 *      Author: samuel
 */

#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>

#include <stm32f4xx.h>

#include "GPIOxDriver.h"
#include "ExtiDriver.h"
#include "BasicTimer.h"
#include "USARTxDriver.h"
#include "SysTickDriver.h"
#include "PwmDriver.h"
#include "PLLDriver.h"
#include "CintaLedDriver.h"

/*----Funcion para cargar iniciar el MCU----*/
void initSystem(void);

/*Pin para ver si funciona el PLL*/
GPIO_Handler_t handlerPLL = {0};

/*Funcion main*/
int main(void){
	//Inicializamos los elementos
	initSystem();

	while(1){
		Rojo(&handlerPLL);
		Verde(&handlerPLL);
		Azul(&handlerPLL);
	}
	return 0;
}


void initSystem(void){
	handlerPLL.pGPIOx										= GPIOC;
	handlerPLL.GPIO_PinConfig.GPIO_PinNumber				= PIN_8;
	handlerPLL.GPIO_PinConfig.GPIO_PinMode					= GPIO_MODE_OUT;
	handlerPLL.GPIO_PinConfig.GPIO_PinOPType				= GPIO_OTYPER_PUSHPULL;
	handlerPLL.GPIO_PinConfig.GPIO_PinSpeed					= GPIO_OSPEED_HIGH;
	//Cargar la configuracion del pin
	GPIO_Config(&handlerPLL);

	ConfigPLL();
}


