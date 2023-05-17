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

/*----Funcion para configurar el PLL----*/
void PLLConfig(void);


/*Pin para ver si funciona el PLL*/
GPIO_Handler_t handlerPLL = {0};

/*Funcion main*/
int main(void){
	//Inicializamos los elementos
	initSystem();
	//Configuramos el PLL
	ConfigPLL();

	while(1){
		//Verde
		//8 bits Verde
		Uno();
		Uno();
		Uno();
		Uno();
		Uno();
		Uno();
		Uno();
		Uno();
		//8 bits Rojo
		Cero();
		Cero();
		Cero();
		Cero();
		Cero();
		Cero();
		Cero();
		Cero();
		//8 bits Azul
		Cero();
		Cero();
		Cero();
		Cero();
		Cero();
		Cero();
		Cero();
		Cero();
	}
	return 0;
}


void initSystem(void){
	handlerPLL.pGPIOx										= GPIOA;
	handlerPLL.GPIO_PinConfig.GPIO_PinNumber				= PIN_8;
	handlerPLL.GPIO_PinConfig.GPIO_PinMode					= GPIO_MODE_ALTFN;
	handlerPLL.GPIO_PinConfig.GPIO_PinOPType				= GPIO_OTYPER_PUSHPULL;
	handlerPLL.GPIO_PinConfig.GPIO_PinSpeed					= GPIO_OSPEED_HIGH;
	handlerPLL.GPIO_PinConfig.GPIO_PinAltFunMode			= AF0;
}


