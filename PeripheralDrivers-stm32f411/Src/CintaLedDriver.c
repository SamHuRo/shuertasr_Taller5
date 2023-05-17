/*
 * CintaLedDriver.c
 *
 *  Created on: May 17, 2023
 *      Author: samuel
 */
#include "PLLDriver.h"
#include "CintaLedDriver.h"

/*====Funcion para crear un uno en la Cinta Led=====*/
void Uno(void){
	__NOP();
	__NOP();
	__NOP();

	GPIOA->ODR &= ~(1<<7);
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

	GPIOA->ODR |= (1<<7);
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
void Cero(void){
		__NOP();
		__NOP();
		__NOP();

		GPIOA->ODR &= ~(1<<7);
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

		GPIOA->ODR &= ~(1<<7);
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
void Verde(void){
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

//Rojo
void Rojo(void){
	//8 bits Verde
	Cero();
	Cero();
	Cero();
	Cero();
	Cero();
	Cero();
	Cero();
	Cero();
	//8 bits Rojo
	Uno();
	Uno();
	Uno();
	Uno();
	Uno();
	Uno();
	Uno();
	Uno();
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

//Azul
void Azul(void){
	//8 bits Verde
	Cero();
	Cero();
	Cero();
	Cero();
	Cero();
	Cero();
	Cero();
	Cero();
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
	Uno();
	Uno();
	Uno();
	Uno();
	Uno();
	Uno();
	Uno();
	Uno();
}

