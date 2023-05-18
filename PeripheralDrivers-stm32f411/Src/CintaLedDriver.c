/*
 * CintaLedDriver.c
 *
 *  Created on: May 17, 2023
 *      Author: samuel
 */
#include "PLLDriver.h"
#include "CintaLedDriver.h"

/*====Funcion para crear un uno en la Cinta Led=====*/
void Uno(GPIO_Handler_t *pGPIOHandler){
	__NOP();
	__NOP();
	__NOP();

	pGPIOHandler->pGPIOx->ODR |= (1 << pGPIOHandler->GPIO_PinConfig.GPIO_PinNumber);
	__NOP();
	__NOP();
	__NOP();
	__NOP();
	__NOP();
	__NOP();
	__NOP();
	__NOP();
	__NOP();
	__NOP();
	__NOP();
	__NOP();
	__NOP();
	__NOP();
	__NOP();
	__NOP();
	__NOP();
	__NOP();
	__NOP();
	__NOP();
	__NOP();
	__NOP();
	__NOP();
	__NOP();
	__NOP();
	__NOP();
	__NOP();
	__NOP();
	__NOP();
	__NOP();
	__NOP();
	__NOP();
	__NOP();
	__NOP();
	__NOP();
	__NOP();
	__NOP();
	__NOP();

	pGPIOHandler->pGPIOx->ODR |= (0 << pGPIOHandler->GPIO_PinConfig.GPIO_PinNumber);
	__NOP();
	__NOP();
	__NOP();
	__NOP();
	__NOP();
	__NOP();
	__NOP();
	__NOP();
	__NOP();
	__NOP();
	__NOP();
	__NOP();
	__NOP();
	__NOP();
	__NOP();
	__NOP();
	__NOP();
	__NOP();
	__NOP();
	__NOP();
	__NOP();
	__NOP();
	__NOP();
	__NOP();
	__NOP();
	__NOP();
	__NOP();
	__NOP();
	__NOP();
	__NOP();
	__NOP();
	__NOP();
	__NOP();
	__NOP();
	__NOP();
	__NOP();
	__NOP();
	__NOP();
	__NOP();
	__NOP();
}

/*====Funcion para crear un cero en la cinta led====*/
void Cero(GPIO_Handler_t *pGPIOHandler){

		pGPIOHandler->pGPIOx->ODR |= (1 << pGPIOHandler->GPIO_PinConfig.GPIO_PinNumber);
		__NOP();
		__NOP();
		__NOP();
		__NOP();
		__NOP();
		__NOP();
		__NOP();
		__NOP();
		__NOP();
		__NOP();
		__NOP();
		__NOP();
		__NOP();
		__NOP();
		__NOP();
		__NOP();
		__NOP();
		__NOP();
		__NOP();
		__NOP();
		__NOP();
		__NOP();
		__NOP();
		__NOP();
		__NOP();
		__NOP();
		__NOP();
		__NOP();
		__NOP();
		__NOP();
		__NOP();
		__NOP();
		__NOP();
		__NOP();
		__NOP();
		__NOP();
		__NOP();
		__NOP();
		__NOP();
		__NOP();
		__NOP();
		__NOP();
		__NOP();
		__NOP();
		__NOP();
		__NOP();
		__NOP();
		__NOP();
		__NOP();
		__NOP();
		__NOP();
		__NOP();
		__NOP();
		__NOP();
		__NOP();
		__NOP();
		__NOP();
		__NOP();
		__NOP();
		__NOP();
		__NOP();
		__NOP();
		__NOP();
		__NOP();
		__NOP();
		__NOP();
		__NOP();
		__NOP();
		__NOP();
		__NOP();
		__NOP();
		__NOP();
		__NOP();
		__NOP();
		__NOP();
		__NOP();
		__NOP();
		__NOP();

		pGPIOHandler->pGPIOx->ODR |= (0 << pGPIOHandler->GPIO_PinConfig.GPIO_PinNumber);
		__NOP();
		__NOP();
		__NOP();
		__NOP();
		__NOP();
}

/*================================
 * En esta seccion se van a escribir las funciones para escribir los colores
 * en la cinta led.
 * ===============================
 * */

//Verde
void Verde(GPIO_Handler_t *pGPIOHandler){
	//8 bits Verde
	Uno(&*pGPIOHandler);
	Uno(&*pGPIOHandler);
	Uno(&*pGPIOHandler);
	Uno(&*pGPIOHandler);
	Uno(&*pGPIOHandler);
	Uno(&*pGPIOHandler);
	Uno(&*pGPIOHandler);
	Uno(&*pGPIOHandler);
	//8 bits Rojo
	Cero(&*pGPIOHandler);
	Cero(&*pGPIOHandler);
	Cero(&*pGPIOHandler);
	Cero(&*pGPIOHandler);
	Cero(&*pGPIOHandler);
	Cero(&*pGPIOHandler);
	Cero(&*pGPIOHandler);
	Cero(&*pGPIOHandler);
	//8 bits Azul
	Cero(&*pGPIOHandler);
	Cero(&*pGPIOHandler);
	Cero(&*pGPIOHandler);
	Cero(&*pGPIOHandler);
	Cero(&*pGPIOHandler);
	Cero(&*pGPIOHandler);
	Cero(&*pGPIOHandler);
	Cero(&*pGPIOHandler);
}

//Rojo
void Rojo(GPIO_Handler_t *pGPIOHandler){
	//8 bits Verde
	Cero(&*pGPIOHandler);
	Cero(&*pGPIOHandler);
	Cero(&*pGPIOHandler);
	Cero(&*pGPIOHandler);
	Cero(&*pGPIOHandler);
	Cero(&*pGPIOHandler);
	Cero(&*pGPIOHandler);
	Cero(&*pGPIOHandler);
	//8 bits Rojo
	Uno(&*pGPIOHandler);
	Uno(&*pGPIOHandler);
	Uno(&*pGPIOHandler);
	Uno(&*pGPIOHandler);
	Uno(&*pGPIOHandler);
	Uno(&*pGPIOHandler);
	Uno(&*pGPIOHandler);
	Uno(&*pGPIOHandler);
	//8 bits Azul
	Cero(&*pGPIOHandler);
	Cero(&*pGPIOHandler);
	Cero(&*pGPIOHandler);
	Cero(&*pGPIOHandler);
	Cero(&*pGPIOHandler);
	Cero(&*pGPIOHandler);
	Cero(&*pGPIOHandler);
	Cero(&*pGPIOHandler);
}

//Azul
void Azul(GPIO_Handler_t *pGPIOHandler){
	//8 bits Verde
	Cero(&*pGPIOHandler);
	Cero(&*pGPIOHandler);
	Cero(&*pGPIOHandler);
	Cero(&*pGPIOHandler);
	Cero(&*pGPIOHandler);
	Cero(&*pGPIOHandler);
	Cero(&*pGPIOHandler);
	Cero(&*pGPIOHandler);
	//8 bits Rojo
	Cero(&*pGPIOHandler);
	Cero(&*pGPIOHandler);
	Cero(&*pGPIOHandler);
	Cero(&*pGPIOHandler);
	Cero(&*pGPIOHandler);
	Cero(&*pGPIOHandler);
	Cero(&*pGPIOHandler);
	Cero(&*pGPIOHandler);
	//8 bits Azul
	Uno(&*pGPIOHandler);
	Uno(&*pGPIOHandler);
	Uno(&*pGPIOHandler);
	Uno(&*pGPIOHandler);
	Uno(&*pGPIOHandler);
	Uno(&*pGPIOHandler);
	Uno(&*pGPIOHandler);
	Uno(&*pGPIOHandler);
}

