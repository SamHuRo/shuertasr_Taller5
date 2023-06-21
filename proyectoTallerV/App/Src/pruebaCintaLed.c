/*
 * pruebaCintaLed.c
 *
 *  Created on: Jun 21, 2023
 *      Author: samuel
 */
#include <stm32f4xx.h>

#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "BasicTimer.h"
#include "CintaLedDriver.h"
#include "ExtiDriver.h"
#include "GPIOxDriver.h"
#include "PLLDriver.h"
#include "SysTickDriver.h"
#include "USARTxDriver.h"


/*==========================
 *  Handler del BlinkyPin
 *==========================*/
//Pin de salida para el blinky pin
GPIO_Handler_t BlinkyPin = {0};
//Handler para la configuracion del Timer
BasicTimer_Handler_t handlerTimerBlinkyPin = {0};

/*==========================
 *    Handler del PLL
 *==========================*/
//Handler para la configuracion del PLL
PLL_Config_t handlerPLL = {0};

/*=====================================
 *    Handler para probar la cinta led
 *====================================*/
GPIO_Handler_t cintaLed = {0};
GPIO_Handler_t cintaPositivo = {0};

uint16_t contador = 0;
/*=========================
 * Cabeceras de las funciones
 * ==========================*/
void initSystem(void);
void apagarLed(void);


int main(void){
	//Cargamos la configuracion de los pines
	initSystem();
	apagarLed();
	while(1){

	}
}


/*=================================================
 * Funcion para cargar la configuracion de los pines
 * ================================================*/
void initSystem(void){
	/*=======================
	 * Configuracion del PLL
	 *=======================*/
	handlerPLL.PLL_ON											= PLL_ENABLE;
	handlerPLL.PLL_PLLM											= 4;
	handlerPLL.PLL_PLLN											= 50;
	handlerPLL.PLL_PLLP											= PLLP_2;
	handlerPLL.PLL_MCO1PRE										= PLL_MCO1PRE_2;
	handlerPLL.APB1_PRESC										= PRESC_APB1_2;
	handlerPLL.APB2_PRESC										= PRESC_APB2_NONE;
	handlerPLL.HSI_TRIM											= 0b01100;
	//Cargar la configuracion del PLL
	ConfigPLL(&handlerPLL);

	/*========================================
	 * Configuracion del Blinky Pin
	 *========================================*/
	BlinkyPin.pGPIOx											= GPIOH;
	BlinkyPin.GPIO_PinConfig.GPIO_PinNumber						= PIN_1;
	BlinkyPin.GPIO_PinConfig.GPIO_PinMode						= GPIO_MODE_OUT;
	BlinkyPin.GPIO_PinConfig.GPIO_PinOPType						= GPIO_OTYPER_PUSHPULL;
	BlinkyPin.GPIO_PinConfig.GPIO_PinSpeed						= GPIO_OSPEED_HIGH;
	//Cargamos la configuracion
	GPIO_Config(&BlinkyPin);
	/* Configuracion del timer */
	//Seleccionar el timer que se va a usar
	handlerTimerBlinkyPin.ptrTIMx								= TIM5;
	//Configuracion con la cual se maneja el timer
	handlerTimerBlinkyPin.TIMx_Config.TIMx_mode					= BTIMER_MODE_UP;
	handlerTimerBlinkyPin.TIMx_Config.TIMx_period				= 2500;
	handlerTimerBlinkyPin.TIMx_Config.TIMx_speed				= BTIMER_SPEED_100us_100MHz;
	handlerTimerBlinkyPin.TIMx_Config.TIMx_interruptEnable		= SET;
	//Cargar la configuracion del Timer
	BasicTimer_Config(&handlerTimerBlinkyPin);

	/*---------------------- Configuracion para el pin de la cinta led ----------------------*/
	cintaLed.pGPIOx 											= GPIOA;
	cintaLed.GPIO_PinConfig.GPIO_PinNumber 						= PIN_6;
	cintaLed.GPIO_PinConfig.GPIO_PinMode						= GPIO_MODE_OUT;
	cintaLed.GPIO_PinConfig.GPIO_PinOPType						= GPIO_OTYPER_PUSHPULL;
	cintaLed.GPIO_PinConfig.GPIO_PinSpeed  						= GPIO_OSPEED_FAST;
	cintaLed.GPIO_PinConfig.GPIO_PinPuPdControl					= GPIO_PUPDR_NOTHING;
	//Cargamos la configuracion
	GPIO_Config(&cintaLed);

	cintaPositivo.pGPIOx 										= GPIOC;
	cintaPositivo.GPIO_PinConfig.GPIO_PinNumber 				= PIN_9;
	cintaPositivo.GPIO_PinConfig.GPIO_PinMode				 	= GPIO_MODE_OUT;
	cintaPositivo.GPIO_PinConfig.GPIO_PinOPType					= GPIO_OTYPER_PUSHPULL;
	cintaPositivo.GPIO_PinConfig.GPIO_PinSpeed  				= GPIO_OSPEED_FAST;
	cintaPositivo.GPIO_PinConfig.GPIO_PinPuPdControl			= GPIO_PUPDR_NOTHING;
	//Cargamos la configuracion
	GPIO_Config(&cintaPositivo);
	GPIO_WritePin(&cintaPositivo, 1);
}

/*=======================================
 * Funciones Callback de los perifericos
 *=======================================*/
/*Funcion callback del Timer 5*/
void BasicTimer5_Callback(void){
	//Se hace el blinky del pin
	GPIO_TooglePin(&BlinkyPin);
	if(contador == 50){
		Verde(&cintaLed);
	}else if(contador == 55){
		apagarLed();
	}else if(contador == 100){
		Rojo(&cintaLed);
	}else if(contador == 102){
		apagarLed();
	}else if(contador == 150){
		Azul(&cintaLed);
	}else if(contador > 150){
		//8 bits Verde
		apagarLed();
		contador = 0;
	}
	contador++;
}

void apagarLed(void){
	//8 bits verde
	Cero(&cintaLed);
	Cero(&cintaLed);
	Cero(&cintaLed);
	Cero(&cintaLed);
	Cero(&cintaLed);
	Cero(&cintaLed);
	Cero(&cintaLed);
	Cero(&cintaLed);
	//8 bits Rojo
	Cero(&cintaLed);
	Cero(&cintaLed);
	Cero(&cintaLed);
	Cero(&cintaLed);
	Cero(&cintaLed);
	Cero(&cintaLed);
	Cero(&cintaLed);
	Cero(&cintaLed);
	//8 bits Azul
	Cero(&cintaLed);
	Cero(&cintaLed);
	Cero(&cintaLed);
	Cero(&cintaLed);
	Cero(&cintaLed);
	Cero(&cintaLed);
	Cero(&cintaLed);
	Cero(&cintaLed);
}


