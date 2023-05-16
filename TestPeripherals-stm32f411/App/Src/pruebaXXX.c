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

/*----Funcion para cargar iniciar el MCU----*/
void initSystem(void);

/*----Funcion para configurar el PLL----*/
void PLLConfig(void);

/*----Definicion de los handlers----*/
GPIO_Handler_t BlinkyPin = {0};      //BlinkyPin (pin de estado)
GPIO_Handler_t handlerUserBotton = {0};
BasicTimer_Handler_t handlerTimerBlinkyPin = {0};  //Definicion del handler para el timer del blinkypin o pin de estado

/*Pin para ver si funciona el PLL*/
GPIO_Handler_t handlerPLL = {0};

/*Funcion main*/
int main(void){

	//Inicializamos los elementos
	initSystem();

	//Configuracion del PLL
	PLLConfig();

	while(1){

	}
	return 0;
}


void initSystem(void){
	/* ----Configuracion del Blinky Pin---- */
	BlinkyPin.pGPIOx										= GPIOA;
	BlinkyPin.GPIO_PinConfig.GPIO_PinNumber					= PIN_5;
	BlinkyPin.GPIO_PinConfig.GPIO_PinMode					= GPIO_MODE_OUT;
	BlinkyPin.GPIO_PinConfig.GPIO_PinOPType					= GPIO_OTYPER_PUSHPULL;
	BlinkyPin.GPIO_PinConfig.GPIO_PinSpeed					= GPIO_OSPEED_HIGH;

	//Cargamos la configuracion
	GPIO_Config(&BlinkyPin);

	/* Configuracion del timer */
	//Seleccionar el timer que se va a usar
	handlerTimerBlinkyPin.ptrTIMx							= TIM2;

	//Configuracion con la cual se maneja el timer
	handlerTimerBlinkyPin.TIMx_Config.TIMx_mode				= BTIMER_MODE_UP;
	handlerTimerBlinkyPin.TIMx_Config.TIMx_period			= 2500;
	handlerTimerBlinkyPin.TIMx_Config.TIMx_speed			= BTIMER_SPEED_100us;
	handlerTimerBlinkyPin.TIMx_Config.TIMx_interruptEnable	= SET;
	//Cargar la configuracion del Timer
	BasicTimer_Config(&handlerTimerBlinkyPin);

}

/*Funcion para configurar el PLL en el MCU*/
void PLLConfig(void){
	//Habilitar el reloj HSE
	RCC->CR &= ~RCC_CR_HSEON; //Limpiamos el registro
	RCC->CR |= RCC_CR_HSEON;

	//Esperamos hasta que el HSE este listo
	while(!(RCC->CR & RCC_CR_HSERDY)){
		__NOP();
	}

	//Activamos el reloj
	RCC->AHB1ENR &= ~RCC_APB1ENR_PWREN; //Limpiamos el registro
	RCC->AHB1ENR |= RCC_APB1ENR_PWREN;

	//Configurar el reloj que se va a utilizar en el PLL
	RCC->PLLCFGR &= ~RCC_PLLCFGR_PLLSRC; //Limpiamos el registro
	RCC->PLLCFGR |= RCC_PLLCFGR_PLLSRC_HSE;

	//Configurar el PLLM, PLLN, PLLP
	//PLLM
	RCC->PLLCFGR |= (16 << RCC_PLLCFGR_PLLM_Pos);
	//PLLN
	RCC->PLLCFGR |= (16 << RCC_PLLCFGR_PLLN_Pos);
	//PLLP
	RCC->PLLCFGR |= (16 << RCC_PLLCFGR_PLLP_Pos);

	//Encender el PLL
	RCC->CR |= RCC_CR_PLLON;

	//Esperar haasta que el PLL este activo
	while(!(RCC->CR & RCC_CR_PLLRDY)){
		__NOP();
	}

	//Configurarr para que el pin utilice el PLL
	RCC->CFGR |= RCC_CFGR_SWS_PLL;

	//Esperar hasta que se active
	while(!(RCC->CFGR & RCC_CFGR_SWS_PLL)){
		__NOP();
	}
}
/*Callback del timer2 -> Hace blinky al led*/
void BasicTimer2_Callback(void){
	GPIO_TooglePin(&BlinkyPin);
	sendMsg++;
}

