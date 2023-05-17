/*
 * pruebaEXTI.c
 *
 *  Created on: May 12, 2023
 *      Author: samuel
 */

#include <stdint.h>

#include <stm32f1xx.h>

#include "GPIOxDriver.h"
#include "BasicTimer.h"
#include "ExtiDriver.h"

/* #----Cabeceras de las de funciones----# */
//Funcion para inicializar el sistema, se definen los pines, timer y las interrupciones
void initSystem(void);
void funcionIzquierda(void);
void funcionDerecha(void);

/* #----Definicion de los handler para los pines----# */
GPIO_Handler_t handlerIzquierda = {0};
GPIO_Handler_t handlerStop = {0};
GPIO_Handler_t handlerDerecha = {0};

/* #----Definicion de los handler----# */
GPIO_Handler_t BlinkyPin = {0};      //BlinkyPin (pin de estado)
/* #----Definicion del Timer----# */
BasicTimer_Handler_t handlerTimerBlinkyPin = {0};  //Definicion del handler para el timer del blinkypin o pin de estado

/*====Definicion del modo default====*/
BasicTimer_Handler_t handlerTimerModo = {0};

/*====Definicion de los handler para el exti====*/
//Encoder
GPIO_Handler_t SW = {0};  //SW del Encoder
GPIO_Handler_t DT = {0};  //DT del Encoder
GPIO_Handler_t CLK = {0}; //CLK del Encoder
/* #----Definicion de las interrupciones----# */
EXTI_Config_t handlerEXTI_SW = {0}; 	//Interrupcion del SW
EXTI_Config_t handlerEXTI_CLK = {0};	//Interrupcion del DT

uint8_t parar = 0;

int main(void)
{
	//Se inicia el MCU con las configuraciones de los pines
	initSystem();
    while(1){
    	if(GPIOReadPin(&SW) == 1){
    		funcionIzquierda();
    	}
    }
}

/* #----Definicion de la funcion initSystem----# */
void initSystem(void){
	handlerIzquierda.pGPIOx									= GPIOA;
	handlerIzquierda.GPIO_PinConfig.GPIO_PinNumber			= PIN_8;
	handlerIzquierda.GPIO_PinConfig.GPIO_PinMode			= GPIO_MODE_OUT;
	handlerIzquierda.GPIO_PinConfig.GPIO_PinOPType			= GPIO_OTYPER_PUSHPULL;
	//Cargamos la configuracion de los pines
	GPIO_Config(&handlerIzquierda);

	handlerStop.pGPIOx									= GPIOA;
	handlerStop.GPIO_PinConfig.GPIO_PinNumber			= PIN_9;
	handlerStop.GPIO_PinConfig.GPIO_PinMode				= GPIO_MODE_OUT;
	handlerStop.GPIO_PinConfig.GPIO_PinOPType			= GPIO_OTYPER_PUSHPULL;
	//Cargamos la configuracion de los pines
	GPIO_Config(&handlerStop);

	handlerDerecha.pGPIOx									= GPIOA;
	handlerDerecha.GPIO_PinConfig.GPIO_PinNumber			= PIN_10;
	handlerDerecha.GPIO_PinConfig.GPIO_PinMode				= GPIO_MODE_OUT;
	handlerDerecha.GPIO_PinConfig.GPIO_PinOPType			= GPIO_OTYPER_PUSHPULL;
	//Cargamos la configuracion de los pines
	GPIO_Config(&handlerDerecha);

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

	/*Configuracion del timer del modo*/
	//Seleccionamos el timer
	handlerTimerModo.ptrTIMx							= TIM3;

	//Configuracion del timer
	handlerTimerModo.TIMx_Config.TIMx_mode				= BTIMER_MODE_UP;
	handlerTimerModo.TIMx_Config.TIMx_period			= 50;
	handlerTimerModo.TIMx_Config.TIMx_speed				= BTIMER_SPEED_1ms;
	handlerTimerModo.TIMx_Config.TIMx_interruptEnable	= SET;
	//Cargar la configuracion del Timer
	BasicTimer_Config(&handlerTimerModo);

	/* ----Configuracion del encoder---- */
	/* Configuracion de los pines */
	//Configuracion del SW
	SW.pGPIOx 									= GPIOB;
	SW.GPIO_PinConfig.GPIO_PinNumber			= PIN_12;
	SW.GPIO_PinConfig.GPIO_PinMode				= GPIO_MODE_IN;
	SW.GPIO_PinConfig.GPIO_PinOPType 			= GPIO_OTYPER_PUSHPULL;
	SW.GPIO_PinConfig.GPIO_PinPuPdControl		= GPIO_PUPDR_PULLDOWN;

	//Configuracion del DT
	DT.pGPIOx 									= GPIOB;
	DT.GPIO_PinConfig.GPIO_PinNumber			= PIN_13;
	DT.GPIO_PinConfig.GPIO_PinMode				= GPIO_MODE_IN;
	DT.GPIO_PinConfig.GPIO_PinSpeed				= GPIO_OSPEED_10MHz;
	DT.GPIO_PinConfig.GPIO_PinAltFunMode		= AF0;
	GPIO_Config(&DT);


	//Configuracion del CLK
	CLK.pGPIOx 									= GPIOB;
	CLK.GPIO_PinConfig.GPIO_PinNumber			= PIN_14;
	CLK.GPIO_PinConfig.GPIO_PinMode				= GPIO_MODE_IN;
	CLK.GPIO_PinConfig.GPIO_PinSpeed			= GPIO_OSPEED_10MHz;

	//Interrupcion para cambiar de modo -> SW
	handlerEXTI_SW.edgeType						= EXTERNAL_INTERRUPT_FALLING_EDGE;
	handlerEXTI_SW.pGPIOHandler					= &SW;
	//Cargar la configuracion
	extInt_Config(&handlerEXTI_SW);

	//Interrupcion para detectar el giro del encoder -> CLK
	handlerEXTI_CLK.edgeType					= EXTERNAL_INTERRUPT_FALLING_EDGE;
	handlerEXTI_CLK.pGPIOHandler				= &CLK;
	//Cargar la configuracion
	extInt_Config(&handlerEXTI_CLK);
}

void BasicTimer2_Callback(void){
	GPIO_TooglePin(&BlinkyPin);
}

void BasicTimer3_Callback(void){
	GPIO_TooglePin(&handlerStop);
}

void callback_extInt12(void){
	parar = 1;
}

void funcionIzquierda(void){
	for(uint32_t i = 0; i < 1600000; i++){
		__NOP();
	}
	GPIO_TooglePin(&handlerIzquierda);
	for(uint32_t i = 0; i < 16000; i++){
		__NOP();
	}
	GPIO_TooglePin(&handlerIzquierda);
	for(uint32_t i = 0; i < 1600000; i++){
		__NOP();
	}
	GPIO_TooglePin(&handlerIzquierda);
	for(uint32_t i = 0; i < 16000; i++){
		__NOP();
	}
	GPIO_TooglePin(&handlerIzquierda);
	for(uint32_t i = 0; i < 1600000; i++){
		__NOP();
	}
	GPIO_TooglePin(&handlerIzquierda);
	for(uint32_t i = 0; i < 16000; i++){
		__NOP();
	}
	GPIO_TooglePin(&handlerIzquierda);
	for(uint32_t i = 0; i < 1600000; i++){
		__NOP();
	}
	GPIO_TooglePin(&handlerIzquierda);
	for(uint32_t i = 0; i < 16000; i++){
		__NOP();
	}
	GPIO_TooglePin(&handlerIzquierda);
}

