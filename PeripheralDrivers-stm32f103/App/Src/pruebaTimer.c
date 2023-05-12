/*
 * pruebaTimer.c
 *
 *  Created on: May 12, 2023
 *      Author: samuel
 */
#include <stdint.h>
#include <stm32f1xx.h>
#include "GPIOxDriver.h"
#include "BasicTimer.h"

/* #----Definicion de los handler----# */
GPIO_Handler_t BlinkyPin = {0};      //BlinkyPin (pin de estado)
/* #----Definicion del Timer----# */
BasicTimer_Handler_t handlerTimerBlinkyPin = {0};  //Definicion del handler para el timer del blinkypin o pin de estado
/* #----Cabeceras de las de funciones----# */
//Funcion para inicializar el sistema, se definen los pines, timer y las interrupciones
void initSystem(void);

/* #----Definicion de la funcion main----# */
int main(void){
	initSystem();
}

/* #----Definicion de la funcion initSystem----# */
void initSystem(void){
	/* ----Configuracion del Blinky Pin---- */
	BlinkyPin.pGPIOx										= GPIOC;
	BlinkyPin.GPIO_PinConfig.GPIO_PinNumber					= PIN_13;
	BlinkyPin.GPIO_PinConfig.GPIO_PinMode					= GPIO_MODE_OUT;
	BlinkyPin.GPIO_PinConfig.GPIO_PinOPType					= GPIO_OTYPER_PUSHPULL;
	BlinkyPin.GPIO_PinConfig.GPIO_PinSpeed					= GPIO_OSPEED_10MHz;

	//Cargamos la configuracion
	GPIO_Config(&BlinkyPin);

	/* Configuracion del timer */
	//Seleccionar el timer que se va a usar
	handlerTimerBlinkyPin.ptrTIMx							= TIM2;

	//Configuracion con la cual se maneja el timer
	handlerTimerBlinkyPin.TIMx_Config.TIMx_mode				= BTIMER_MODE_UP;
	handlerTimerBlinkyPin.TIMx_Config.TIMx_period			= 250;
	handlerTimerBlinkyPin.TIMx_Config.TIMx_speed			= BTIMER_SPEED_1ms;
	handlerTimerBlinkyPin.TIMx_Config.TIMx_interruptEnable	= SET;

	//Cargar la configuracion del Timer
	BasicTimer_Config(&handlerTimerBlinkyPin);
}

