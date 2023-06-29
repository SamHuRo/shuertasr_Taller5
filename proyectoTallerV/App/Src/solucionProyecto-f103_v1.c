/*
 * solucionProyecto-f103_v1.c
 *
 *  Created on: Jun 28, 2023
 *      Author: samuel
 */
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include <stm32f1xx.h>

//Drivers para la implementacion de los perifericos
#include "BasicTimer.h"
#include "ExtiDriver.h"
#include "GPIOxDriver.h"
#include "I2CDriver.h"
#include "SysTickDriver.h"
#include "USARTxDriver.h"


#define HSI_CLOCK_CONFIGURED	0 //8MHz

/*==========================
 *  Handler del BlinkyPin
 *==========================*/
//Pin de salida para el blinky pin
GPIO_Handler_t BlinkyPin = {0};
//Handler para la configuracion del Timer
BasicTimer_Handler_t handlerTimerBlinkyPin = {0};

/*===================================
 *     Handler del USART Bluetooth
 *====================================*/
//Pines para la comunicacion USART
GPIO_Handler_t handlerPinTXHC_05 = {0};
GPIO_Handler_t handlerPinRXHC_05 = {0};
//Handler para la configuracion USART
USART_Handler_t handlerTerminalHC_05 = {0};

/*==============================================
 *     Handler para las interrupciones EXTI
 *==============================================*/
//Handler de los pines
GPIO_Handler_t izquierda = {0};
GPIO_Handler_t derecha = {0};
GPIO_Handler_t stop = {0};
GPIO_Handler_t freno = {0};
//Handler de los EXTI
EXTI_Config_t handlerExtiIzquierda = {0};
EXTI_Config_t handlerExtiDerecha = {0};
EXTI_Config_t handlerExtiStop = {0};
EXTI_Config_t handlerExtiFreno = {0};
//Handler para la configuracion del Timer
BasicTimer_Handler_t handlerTimerDireccional = {0};

/*=========================
 * Cabeceras de las funciones
 * ==========================*/
void initSystem(void);
//Funcion para enviar el mensaje de la direccional
void direccional(void);
//Funcion para contar los RPM
void rpmContador(void);

/*==========================
 *      Pines de prueba
 * ==========================*/
GPIO_Handler_t LedIzquierda = {0};
GPIO_Handler_t LedStop = {0};
GPIO_Handler_t LedDerecha = {0};

uint8_t onIzquierda = 0;
uint8_t onDerecha = 0;
uint8_t onStop = 0;
uint8_t onFreno = 0;

uint8_t mensajeEnviado = 0;

/*=========================
 *    Varaibles
 * ==========================*/
uint16_t contadorVuelta = 0;
uint16_t rpm1 = 0;
uint16_t rpm2 = 0;
int16_t restaRpm = 0;




int main(void){
	//Cargamos la configuracion de los pines
	initSystem();
	//Configuracion del SysTick a 16MHz
	config_SysTick_ms(HSI_CLOCK_CONFIGURED);
	while(1){
		direccional();
		rpmContador();
	}
}
/*=================================================
 * Funcion para cargar la configuracion de los pines
 * ================================================*/
void initSystem(void){
	/* ------------------- Configuracion del Blinky Pin ---------------------- */
	BlinkyPin.pGPIOx												= GPIOC;
	BlinkyPin.GPIO_PinConfig.GPIO_PinNumber							= PIN_13;
	BlinkyPin.GPIO_PinConfig.GPIO_PinMode							= GPIO_MODE_OUT;
	BlinkyPin.GPIO_PinConfig.GPIO_PinOPType							= GPIO_OTYPER_PUSHPULL;
	BlinkyPin.GPIO_PinConfig.GPIO_PinSpeed							= GPIO_OSPEED_10MHz;
	//Cargamos la configuracion
	GPIO_Config(&BlinkyPin);
	/* Configuracion del timer */
	//Seleccionar el timer que se va a usar
	handlerTimerBlinkyPin.ptrTIMx									= TIM2;
	//Configuracion con la cual se maneja el timer
	handlerTimerBlinkyPin.TIMx_Config.TIMx_mode						= BTIMER_MODE_UP;
	handlerTimerBlinkyPin.TIMx_Config.TIMx_period					= 2500;
	handlerTimerBlinkyPin.TIMx_Config.TIMx_speed					= BTIMER_SPEED_100us;
	handlerTimerBlinkyPin.TIMx_Config.TIMx_interruptEnable			= SET;
	//Cargar la configuracion del Timer
	BasicTimer_Config(&handlerTimerBlinkyPin);

	/*--------------------- Configuracion de la comunicacion serial con Bluetooth ---------------*/
	//Configuracion para el pin de transmicion
	handlerPinTXHC_05.pGPIOx 										= GPIOA;
	handlerPinTXHC_05.GPIO_PinConfig.GPIO_PinNumber					= PIN_9;
	handlerPinTXHC_05.GPIO_PinConfig.GPIO_PinMode					= GPIO_MODE_AF;
	handlerPinTXHC_05.GPIO_PinConfig.GPIO_PinOPType					= GPIO_OTYPER_PUSHPULL;
	//Cargar la configuracion del pin
	GPIO_Config(&handlerPinTXHC_05);
	//configuracion del pin para la recepcion
	handlerPinRXHC_05.pGPIOx										= GPIOA;
	handlerPinRXHC_05.GPIO_PinConfig.GPIO_PinNumber					= PIN_10;
	handlerPinRXHC_05.GPIO_PinConfig.GPIO_PinMode					= GPIO_MODE_IN;
	handlerPinRXHC_05.GPIO_PinConfig.GPIO_PinPuPdControl			= GPIO_PUPDR_PULLUP;
	//Cargar la configuracion del pin
	GPIO_Config(&handlerPinRXHC_05);
	//Configuracion del USART
	handlerTerminalHC_05.ptrUSARTx 									= USART1;
	handlerTerminalHC_05.USART_Config.USART_baudrate 				= USART_BAUDRATE_9600;
	handlerTerminalHC_05.USART_Config.USART_datasize				= USART_DATASIZE_8BIT;
	handlerTerminalHC_05.USART_Config.USART_parity					= USART_PARITY_NONE;
	handlerTerminalHC_05.USART_Config.USART_stopbits				= USART_STOPBIT_1;
	handlerTerminalHC_05.USART_Config.USART_mode					= USART_MODE_RXTX;
	handlerTerminalHC_05.USART_Config.USART_enableIntRX				= USART_RX_INTERRUP_ENABLE;
	//Cargar la configuracion del USART
	USART_Config(&handlerTerminalHC_05);

	/*----------------------- Configuracion para los EXTI --------------------------------------*/
	//Boton Izquierdo
	izquierda.pGPIOx 												= GPIOB;
	izquierda.GPIO_PinConfig.GPIO_PinNumber							= PIN_5;
	izquierda.GPIO_PinConfig.GPIO_PinMode							= GPIO_MODE_IN;
	izquierda.GPIO_PinConfig.GPIO_PinOPType 						= GPIO_OTYPER_PUSHPULL;
	izquierda.GPIO_PinConfig.GPIO_PinPuPdControl					= GPIO_PUPDR_PULLDOWN;
	//Interrupcion para encender las direccionales a la izquierda
	handlerExtiIzquierda.edgeType									= EXTERNAL_INTERRUPT_FALLING_EDGE;
	handlerExtiIzquierda.pGPIOHandler								= &izquierda;
	//Cargar la configuracion
	extInt_Config(&handlerExtiIzquierda);

	//Boton Stop
	stop.pGPIOx 													= GPIOB;
	stop.GPIO_PinConfig.GPIO_PinNumber								= PIN_4;
	stop.GPIO_PinConfig.GPIO_PinMode								= GPIO_MODE_IN;
	stop.GPIO_PinConfig.GPIO_PinOPType 								= GPIO_OTYPER_PUSHPULL;
	stop.GPIO_PinConfig.GPIO_PinPuPdControl							= GPIO_PUPDR_PULLDOWN;
	//Interrupcion para apagar las direccionales
	handlerExtiStop.edgeType										= EXTERNAL_INTERRUPT_FALLING_EDGE;
	handlerExtiStop.pGPIOHandler									= &stop;
	//Cargar la configuracion
	extInt_Config(&handlerExtiStop);

	//Boton Derecha
	derecha.pGPIOx 													= GPIOA;
	derecha.GPIO_PinConfig.GPIO_PinNumber							= PIN_15;
	derecha.GPIO_PinConfig.GPIO_PinMode								= GPIO_MODE_IN;
	derecha.GPIO_PinConfig.GPIO_PinOPType 							= GPIO_OTYPER_PUSHPULL;
	derecha.GPIO_PinConfig.GPIO_PinPuPdControl						= GPIO_PUPDR_PULLDOWN;
	//Interrupcion para encender las direccionales a la derecha
	handlerExtiDerecha.edgeType										= EXTERNAL_INTERRUPT_FALLING_EDGE;
	handlerExtiDerecha.pGPIOHandler									= &derecha;
	//Cargar la configuracion
	extInt_Config(&handlerExtiDerecha);

	//Deteccion del freno
	freno.pGPIOx 													= GPIOB;
	freno.GPIO_PinConfig.GPIO_PinNumber								= PIN_12;
	freno.GPIO_PinConfig.GPIO_PinMode								= GPIO_MODE_IN;
	freno.GPIO_PinConfig.GPIO_PinOPType 							= GPIO_OTYPER_PUSHPULL;
	freno.GPIO_PinConfig.GPIO_PinPuPdControl						= GPIO_PUPDR_PULLDOWN;
	//Interrupcion para encender las direccionales a la derecha
	handlerExtiFreno.edgeType										= EXTERNAL_INTERRUPT_FALLING_EDGE;
	handlerExtiFreno.pGPIOHandler									= &freno;
	//Cargar la configuracion
	extInt_Config(&handlerExtiFreno);

	/*----------------------- Configuracion para los pines de prueba --------------------------------------*/
	LedIzquierda.pGPIOx												= GPIOB;
	LedIzquierda.GPIO_PinConfig.GPIO_PinNumber						= PIN_9;
	LedIzquierda.GPIO_PinConfig.GPIO_PinMode						= GPIO_MODE_OUT;
	LedIzquierda.GPIO_PinConfig.GPIO_PinOPType						= GPIO_OTYPER_PUSHPULL;
	//Cargamos la configuracion de los pines
	GPIO_Config(&LedIzquierda);
	LedStop.pGPIOx													= GPIOB;
	LedStop.GPIO_PinConfig.GPIO_PinNumber							= PIN_8;
	LedStop.GPIO_PinConfig.GPIO_PinMode								= GPIO_MODE_OUT;
	LedStop.GPIO_PinConfig.GPIO_PinOPType							= GPIO_OTYPER_PUSHPULL;
	//Cargamos la configuracion de los pines
	GPIO_Config(&LedStop);
	LedDerecha.pGPIOx												= GPIOB;
	LedDerecha.GPIO_PinConfig.GPIO_PinNumber						= PIN_7;
	LedDerecha.GPIO_PinConfig.GPIO_PinMode							= GPIO_MODE_OUT;
	LedDerecha.GPIO_PinConfig.GPIO_PinOPType						= GPIO_OTYPER_PUSHPULL;
	//Cargamos la configuracion de los pines
	GPIO_Config(&LedDerecha);
	/* Configuracion del timer */
	//Seleccionar el timer que se va a usar
	handlerTimerDireccional.ptrTIMx									= TIM3;
	//Configuracion con la cual se maneja el timer
	handlerTimerDireccional.TIMx_Config.TIMx_mode					= BTIMER_MODE_UP;
	handlerTimerDireccional.TIMx_Config.TIMx_period					= 1500;
	handlerTimerDireccional.TIMx_Config.TIMx_speed					= BTIMER_SPEED_100us;
	handlerTimerDireccional.TIMx_Config.TIMx_interruptEnable		= SET;
	//Cargar la configuracion del Timer
	BasicTimer_Config(&handlerTimerDireccional);
}

/*=======================================
 *   Funciones utilizadas en el Main
 *=======================================*/
//Esta funcion se encarga de enviar la senal al otro MCU para que muetre la direccional adecuada
void direccional(void){
	if(mensajeEnviado == 1){
		if(onIzquierda == 1){
			writeMsg(&handlerTerminalHC_05, "i");
		}else if(onDerecha == 1){
			writeMsg(&handlerTerminalHC_05, "d");
		}else{
			if(onStop == 1){
				writeMsg(&handlerTerminalHC_05, "s");
			}else if(onFreno == 1){
				writeMsg(&handlerTerminalHC_05, "f");
			}
		}
		mensajeEnviado = 0;
	}
}
//Funcion para contar las RPM que tiene la bicicleta
void rpmContador(void){
	delay_ms(1000);
	rpm1 = contadorVuelta;
	contadorVuelta = 0;
	delay_ms(1000);
	rpm2 = contadorVuelta;
	contadorVuelta = 0;
	restaRpm = rpm1 - rpm2;
	if(restaRpm > 0 ){
		onFreno = 1;
		onStop = 0;
		mensajeEnviado = 1;
	}else{
		onStop = 1;
		onFreno = 0;
		mensajeEnviado = 1;
	}
}
/*=======================================
 * Funciones Callback de los perifericos
 *=======================================*/
/*Callback del timer2 -> Hace blinky al led*/
void BasicTimer2_Callback(void){
	GPIO_TooglePin(&BlinkyPin);
	if(onFreno == 1){
		GPIO_TooglePin(&LedStop);
	}
}
/*Callback del timer5 -> Hace blinky al led para las direccionales*/
void BasicTimer3_Callback(void){
	if(onIzquierda == 1){
		GPIO_TooglePin(&LedIzquierda);
	}else if(onDerecha == 1){
		GPIO_TooglePin(&LedDerecha);
	}else{
		if(onStop == 1){
			GPIO_TooglePin(&LedStop);
		}
	}
}

/*Callback para la direccional izquierda*/
void callback_extInt5(void){
	onIzquierda = 1;
	GPIO_WritePin(&LedStop, 0);
	onStop = 0;
	onFreno = 0;
	GPIO_WritePin(&LedDerecha, 0);
	onDerecha = 0;
	//Enviar mensaje por USART
	mensajeEnviado = 1;
}
/*Callback para la señal de stop*/
void callback_extInt4(void){
	GPIO_WritePin(&LedIzquierda, 0);
	onIzquierda = 0;
	onStop = 1;
	onFreno = 0;
	GPIO_WritePin(&LedDerecha, 0);
	onDerecha = 0;
	//Enviar mensaje por USART
	mensajeEnviado = 1;
}
/*Callback para la direccional derecha*/
void callback_extInt15(void){
	GPIO_WritePin(&LedIzquierda, 0);
	onIzquierda = 0;
	GPIO_WritePin(&LedStop, 0);
	onStop = 0;
	onFreno = 0;
	onDerecha = 1;
	//Enviar mensaje por USART
	mensajeEnviado = 1;
}
/*Callback para el freno*/
void callback_extInt12(void){
	contadorVuelta++;
}

