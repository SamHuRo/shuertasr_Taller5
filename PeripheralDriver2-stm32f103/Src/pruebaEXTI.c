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


uint32_t contador = 0;// Es la variable que guarda el numero
uint8_t valor1 = 0;
uint8_t valor2 = 0;
uint8_t valor3 = 0;
uint8_t valor4 = 0;

/* #----Cabeceras de las de funciones----# */
//Funcion para inicializar el sistema, se definen los pines, timer y las interrupciones
void initSystem(void);

/* #----Definicion de los handler para los pines----# */
GPIO_Handler_t handlerLed1 = {0};
GPIO_Handler_t handlerLed2 = {0};
GPIO_Handler_t handlerLed3 = {0};
GPIO_Handler_t handlerLed4 = {0};

/* #----Definicion de los handler----# */
GPIO_Handler_t BlinkyPin = {0};      //BlinkyPin (pin de estado)
/* #----Definicion del Timer----# */
BasicTimer_Handler_t handlerTimerBlinkyPin = {0};  //Definicion del handler para el timer del blinkypin o pin de estado

/*Pin de entrada*/
GPIO_Handler_t handlerPinEntrada = {0};

int main(void)
{
	//Se inicia el MCU con las configuraciones de los pines
	initSystem();
    while(1){
		//Ciclo for para crear el delay de 1 segundo
		for(uint32_t i = 0; i <= 1600000; i ++){
		}

		if(GPIOReadPin(&handlerPinEntrada) == 1){
			contador += 1; //se pone el contador a incrementar de 1 si el boton no está precionado
		}else if(GPIOReadPin(&handlerPinEntrada) == 0){
			contador -= 1;
		}

		if(contador == 61){
			contador = 1; //el reinicio del contador cuando llegue a 60
		}else if(contador == 0){
			contador = 60;
		}
		//Mover el bit del contador a su pin correspondinete
		valor1 = (contador);
		valor2 = (contador >> 1);
		valor3 = (contador >> 2);
		valor4 = (contador >> 3);

		valor1 &= (0b1);
		valor2 &= (0b1);
		valor3 &= (0b1);
		valor4 &= (0b1);

		GPIO_WritePin(&handlerLed1, valor1);
		GPIO_WritePin(&handlerLed2, valor2);
		GPIO_WritePin(&handlerLed3, valor3);
		GPIO_WritePin(&handlerLed4, valor4);
    }
}

/* #----Definicion de la funcion initSystem----# */
void initSystem(void){
	handlerLed1.pGPIOx									= GPIOA;
	handlerLed1.GPIO_PinConfig.GPIO_PinNumber			= PIN_8;
	handlerLed1.GPIO_PinConfig.GPIO_PinMode				= GPIO_MODE_OUT;
	handlerLed1.GPIO_PinConfig.GPIO_PinOPType			= GPIO_OTYPER_PUSHPULL;
	//Cargamos la configuracion de los pines
	GPIO_Config(&handlerLed1);

	handlerLed2.pGPIOx									= GPIOA;
	handlerLed2.GPIO_PinConfig.GPIO_PinNumber			= PIN_9;
	handlerLed2.GPIO_PinConfig.GPIO_PinMode				= GPIO_MODE_OUT;
	handlerLed2.GPIO_PinConfig.GPIO_PinOPType			= GPIO_OTYPER_PUSHPULL;
	//Cargamos la configuracion de los pines
	GPIO_Config(&handlerLed2);

	handlerLed3.pGPIOx									= GPIOA;
	handlerLed3.GPIO_PinConfig.GPIO_PinNumber			= PIN_10;
	handlerLed3.GPIO_PinConfig.GPIO_PinMode				= GPIO_MODE_OUT;
	handlerLed3.GPIO_PinConfig.GPIO_PinOPType			= GPIO_OTYPER_PUSHPULL;
	//Cargamos la configuracion de los pines
	GPIO_Config(&handlerLed3);

	handlerLed4.pGPIOx									= GPIOB;
	handlerLed4.GPIO_PinConfig.GPIO_PinNumber			= PIN_1;
	handlerLed4.GPIO_PinConfig.GPIO_PinMode				= GPIO_MODE_OUT;
	handlerLed4.GPIO_PinConfig.GPIO_PinOPType			= GPIO_OTYPER_PUSHPULL;
	//Cargamos la configuracion de los pines
	GPIO_Config(&handlerLed4);

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

	/*Configuracion del pin de entrada*/
	handlerPinEntrada.pGPIOx							= GPIOB;
	handlerPinEntrada.GPIO_PinConfig.GPIO_PinNumber		= PIN_12;
	handlerPinEntrada.GPIO_PinConfig.GPIO_PinMode		= GPIO_MODE_IN;
	handlerPinEntrada.GPIO_PinConfig.GPIO_PinOPType 	= GPIO_OTYPER_PUSHPULL;
}

void BasicTimer2_Callback(void){
	GPIO_TooglePin(&BlinkyPin);
}
