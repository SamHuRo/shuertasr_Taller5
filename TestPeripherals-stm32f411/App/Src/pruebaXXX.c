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

	while(1){

	}
	return 0;
}


void initSystem(void){
	handlerPLL.pGPIOx										= GPIOA;
	handlerPLL.GPIO_PinConfig.GPIO_PinNumber				= PIN_8;
	handlerPLL.GPIO_PinConfig.GPIO_PinMode					= GPIO_MODE_OUT;
	handlerPLL.GPIO_PinConfig.GPIO_PinOPType				= GPIO_OTYPER_PUSHPULL;
	handlerPLL.GPIO_PinConfig.GPIO_PinSpeed					= GPIO_OSPEED_HIGH;
	handlerPLL.GPIO_PinConfig.GPIO_PinAltFunMode			= AF0;

	//Aseguramos la entrada del PLL es el HSI
	RCC->PLLCFGR &= ~(RCC_PLLCFGR_PLLSRC);

	//Limpiamos el registro
	RCC->PLLCFGR &= ~(RCC_PLLCFGR_PLLM);
	//Ponemos un 8 en el PLLM
	RCC->PLLCFGR |= (8 << RCC_PLLCFGR_PLLM_Pos);

	//Ponemos en 100 el PLLN
	RCC->PLLCFGR &= ~(RCC_PLLCFGR_PLLN); //Limpiamos el registro
	RCC->PLLCFGR |= (100 << RCC_PLLCFGR_PLLN_Pos);

	//Ponemos en 2 el PLLP
	RCC->PLLCFGR &= ~(RCC_PLLCFGR_PLLP);

	/*Cambiamos la latencia en FLASH_ACR para que el micro espere un poco para entrear la informacion en
	 * los registros*/

	FLASH->ACR &= ~(FLASH_ACR_LATENCY); //Limpiamos el registro
	//3WS de latencia
	FLASH->ACR |= FLASH_ACR_LATENCY_1WS;
	FLASH->ACR |= FLASH_ACR_LATENCY_2WS;

	//Seleccionamos el PLL
	RCC->CFGR |= (RCC_CFGR_MCO1);

	//Utilizamos el prescaler para poder ver la señal en el osciloscopio
	RCC->CFGR |= (RCC_CFGR_MCO1PRE_0);
	RCC->CFGR |= (RCC_CFGR_MCO1PRE_1);
	RCC->CFGR |= (RCC_CFGR_MCO1PRE_2);
	//Encender el PLL
	RCC->CR |= RCC_CR_PLLON;

	//Esperamos que el PLL se estabilice
	while(!(RCC->CR & RCC_CR_PLLRDY)){
		__NOP();
	}
	//Cambiamos el CPU clock source cambiamoando los }SW bits
	RCC->CFGR &= ~(RCC_CFGR_SW);
	RCC->CFGR |= (RCC_CFGR_SWS_1);
}


