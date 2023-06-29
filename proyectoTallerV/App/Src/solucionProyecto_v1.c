/*
 * solucionProyecto_v1.c
 *
 *  Created on: Jun 15, 2023
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
#include "CintaLedDriver_Stop.h"
#include "CintaLedDriver_Derecha.h"
#include "ExtiDriver.h"
#include "GPIOxDriver.h"
#include "I2CDriver.h"
#include "MPUxDriver.h"
#include "PLLDriver.h"
#include "SysTickDriver.h"
#include "USARTxDriver.h"

/*==========================
 *  		Macros
 *==========================*/
#define BUFFER_SIZE 			25


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
/*----------------------------------- Variables para la comunicacion con el modulo HC-05 -----------------------------------*/
uint8_t rxDataHC_05 = 0;
uint8_t dato = 0;

/*----------------------------------- Variables para las direccionales -----------------------------------*/
uint8_t izquierdaOn = 0;
uint8_t stopOn = 0;
uint8_t derechaOn = 0;

/*==========================
 *    Handler del PLL
 *==========================*/
//Handler para la configuracion del PLL
PLL_Config_t handlerPLL = {0};

/*==========================
 * Configuracion para el I2C
 * ==========================*/
//Acelerometro
GPIO_Handler_t SDAAccel = {0};
GPIO_Handler_t SCLAccel = {0};
I2C_Handler_t handlerAccelerometer = {0};
BasicTimer_Handler_t timerAccel = {0};
uint8_t i2cBuffer = 0;

/*--------- Variables -------------*/
uint8_t startMuestreo = 0;
//Variables para guardar el muestreo de cada uno de los ejes
float muestreoEjeX = 0;
float muestreoEjeY = 0;
float muestreoEjeZ = 0;

/*===========================================
 * Configuracion para los pines de los leds
 * ==========================================*/
//Acelerometro
GPIO_Handler_t ledIzquierda = {0};
GPIO_Handler_t ledDerecha = {0};
GPIO_Handler_t ledStop = {0};
uint8_t contadorLed = 0;
BasicTimer_Handler_t handlerTimerLed = {0};

/*=====================================
 *    Handler para probar la cinta led
 *====================================*/
GPIO_Handler_t cintaLedIzquierda = {0};
GPIO_Handler_t cintaLedDerecha = {0};
GPIO_Handler_t cintaLedStop = {0};

GPIO_Handler_t freno = {0};
EXTI_Config_t handlerExtiFreno = {0};

/*=========================
 * Cabeceras de las funciones
 * ==========================*/
void initSystem(void);
//Funcion para obtener los comandos del modulo HC-05
void cmdHC_05(void);

/*=========================
 * 	 Variables Globales
 * ==========================*/



int main(void){
	//Cargamos la configuracion de los pines
	initSystem();
	while(1){
		//Verificamos si se envio el mensaje al modulo HC-05
		cmdHC_05();
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
	BlinkyPin.pGPIOx											= GPIOA;
	BlinkyPin.GPIO_PinConfig.GPIO_PinNumber						= PIN_5;
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

	/*========================================
	 * Configuracion de la comunicacion serial
	 *========================================*/
	/*---------------------- Configuracion HC-05, comunicación terminal ----------------------*/
	//Configuracion para el pin de transmicion
	handlerPinTXHC_05.pGPIOx 										= GPIOC;
	handlerPinTXHC_05.GPIO_PinConfig.GPIO_PinNumber					= PIN_6;
	handlerPinTXHC_05.GPIO_PinConfig.GPIO_PinMode					= GPIO_MODE_ALTFN;
	handlerPinTXHC_05.GPIO_PinConfig.GPIO_PinOPType					= GPIO_OTYPER_PUSHPULL;
	handlerPinTXHC_05.GPIO_PinConfig.GPIO_PinPuPdControl			= GPIO_PUPDR_NOTHING;
	handlerPinTXHC_05.GPIO_PinConfig.GPIO_PinSpeed					= GPIO_OSPEED_FAST;
	handlerPinTXHC_05.GPIO_PinConfig.GPIO_PinAltFunMode				= AF8;
	//Cargar la configuracion del pin
	GPIO_Config(&handlerPinTXHC_05);
	//configuracion del pin para la recepcion
	handlerPinRXHC_05.pGPIOx										= GPIOC;
	handlerPinRXHC_05.GPIO_PinConfig.GPIO_PinNumber					= PIN_7;
	handlerPinRXHC_05.GPIO_PinConfig.GPIO_PinMode					= GPIO_MODE_ALTFN;
	handlerPinRXHC_05.GPIO_PinConfig.GPIO_PinOPType					= GPIO_OTYPER_PUSHPULL;
	handlerPinRXHC_05.GPIO_PinConfig.GPIO_PinPuPdControl			= GPIO_PUPDR_NOTHING;
	handlerPinRXHC_05.GPIO_PinConfig.GPIO_PinSpeed					= GPIO_OSPEED_FAST;
	handlerPinRXHC_05.GPIO_PinConfig.GPIO_PinAltFunMode				= AF8;
	//Cargar la configuracion del pin
	GPIO_Config(&handlerPinRXHC_05);
	//Configuracion del USART
	handlerTerminalHC_05.ptrUSARTx 									= USART6;
	handlerTerminalHC_05.USART_Config.USART_baudrate 				= USART_BAUDRATE_9600;
	handlerTerminalHC_05.USART_Config.USART_datasize				= USART_DATASIZE_8BIT;
	handlerTerminalHC_05.USART_Config.USART_parity					= USART_PARITY_NONE;
	handlerTerminalHC_05.USART_Config.USART_stopbits				= USART_STOPBIT_1;
	handlerTerminalHC_05.USART_Config.USART_mode					= USART_MODE_RXTX;
	handlerTerminalHC_05.USART_Config.USART_enableIntRX				= USART_RX_INTERRUP_ENABLE;
	handlerTerminalHC_05.USART_Config.USART_enableIntTX				= USART_TX_INTERRUP_DISABLE;
	handlerTerminalHC_05.USART_Config.USART_PLL_Enable				= PLL_ENABLE;
	//Cargar la configuracion del USART
	USART_Config(&handlerTerminalHC_05);

	/*--------------------- Configuracion de los pines para las direccionales ------------------*/
	ledIzquierda.pGPIOx											= GPIOA;
	ledIzquierda.GPIO_PinConfig.GPIO_PinNumber					= PIN_8;
	ledIzquierda.GPIO_PinConfig.GPIO_PinMode					= GPIO_MODE_OUT;
	ledIzquierda.GPIO_PinConfig.GPIO_PinOPType					= GPIO_OTYPER_PUSHPULL;
	ledIzquierda.GPIO_PinConfig.GPIO_PinSpeed					= GPIO_OSPEED_HIGH;
	//Cargamos la configuracion
	GPIO_Config(&ledIzquierda);

	ledDerecha.pGPIOx											= GPIOB;
	ledDerecha.GPIO_PinConfig.GPIO_PinNumber					= PIN_5;
	ledDerecha.GPIO_PinConfig.GPIO_PinMode						= GPIO_MODE_OUT;
	ledDerecha.GPIO_PinConfig.GPIO_PinOPType					= GPIO_OTYPER_PUSHPULL;
	ledDerecha.GPIO_PinConfig.GPIO_PinSpeed						= GPIO_OSPEED_HIGH;
	//Cargamos la configuracion
	GPIO_Config(&ledDerecha);

	/* Configuracion del timer */
	//Seleccionar el timer que se va a usar
	handlerTimerLed.ptrTIMx										= TIM2;
	//Configuracion con la cual se maneja el timer
	handlerTimerLed.TIMx_Config.TIMx_mode						= BTIMER_MODE_UP;
	handlerTimerLed.TIMx_Config.TIMx_period						= 1000;
	handlerTimerLed.TIMx_Config.TIMx_speed						= BTIMER_SPEED_100us_100MHz;
	handlerTimerLed.TIMx_Config.TIMx_interruptEnable			= SET;
	//Cargar la configuracion del Timer
	BasicTimer_Config(&handlerTimerLed);

	/*---------------------- Configuracion para el pin de la cinta led ----------------------*/
	cintaLedIzquierda.pGPIOx 									= GPIOC;
	cintaLedIzquierda.GPIO_PinConfig.GPIO_PinNumber 			= PIN_10;
	cintaLedIzquierda.GPIO_PinConfig.GPIO_PinMode				= GPIO_MODE_OUT;
	cintaLedIzquierda.GPIO_PinConfig.GPIO_PinOPType				= GPIO_OTYPER_PUSHPULL;
	cintaLedIzquierda.GPIO_PinConfig.GPIO_PinSpeed  			= GPIO_OSPEED_FAST;
	cintaLedIzquierda.GPIO_PinConfig.GPIO_PinPuPdControl		= GPIO_PUPDR_NOTHING;
	//Cargamos la configuracion
	GPIO_Config(&cintaLedIzquierda);

	cintaLedStop.pGPIOx 										= GPIOC;
	cintaLedStop.GPIO_PinConfig.GPIO_PinNumber 					= PIN_12;
	cintaLedStop.GPIO_PinConfig.GPIO_PinMode					= GPIO_MODE_OUT;
	cintaLedStop.GPIO_PinConfig.GPIO_PinOPType					= GPIO_OTYPER_PUSHPULL;
	cintaLedStop.GPIO_PinConfig.GPIO_PinSpeed  					= GPIO_OSPEED_FAST;
	cintaLedStop.GPIO_PinConfig.GPIO_PinPuPdControl				= GPIO_PUPDR_NOTHING;
	//Cargamos la configuracion
	GPIO_Config(&cintaLedStop);

	cintaLedDerecha.pGPIOx 										= GPIOC;
	cintaLedDerecha.GPIO_PinConfig.GPIO_PinNumber 				= PIN_11;
	cintaLedDerecha.GPIO_PinConfig.GPIO_PinMode					= GPIO_MODE_OUT;
	cintaLedDerecha.GPIO_PinConfig.GPIO_PinOPType				= GPIO_OTYPER_PUSHPULL;
	cintaLedDerecha.GPIO_PinConfig.GPIO_PinSpeed  				= GPIO_OSPEED_FAST;
	cintaLedDerecha.GPIO_PinConfig.GPIO_PinPuPdControl			= GPIO_PUPDR_NOTHING;
	//Cargamos la configuracion
	GPIO_Config(&cintaLedDerecha);
}


/*=======================================
 *   Funciones utilizadas en el Main
 *=======================================*/
//Funcion para obtener la respuesta del modulo Hc-05
void cmdHC_05(void){
	if(dato == 's'){
		izquierdaOn = 0;
		derechaOn = 0;
		stopOn = 1;
		GPIO_WritePin(&ledDerecha, 0);
		GPIO_WritePin(&ledIzquierda, 0);
		dato = '\0';
	}else if(dato == 'd'){
		izquierdaOn = 0;
		derechaOn = 1;
		stopOn = 0;
		GPIO_WritePin(&ledIzquierda, 0);
		dato = '\0';
	}else if(dato == 'i'){
		izquierdaOn = 1;
		derechaOn = 0;
		stopOn = 0;
		GPIO_WritePin(&ledDerecha, 0);
		dato = '\0';
	}else if(dato == 'f'){
		izquierdaOn = 0;
		derechaOn = 0;
		stopOn = 2;
		GPIO_WritePin(&ledDerecha, 0);
		GPIO_WritePin(&ledIzquierda, 0);
		dato = '\0';
	}
}
/*=======================================
 * Funciones Callback de los perifericos
 *=======================================*/
/*Funcion callback del Timer 5*/
void BasicTimer5_Callback(void){
	//Se hace el blinky del pin
	GPIO_TooglePin(&BlinkyPin);
}
/*Funcion callback del Timer 2*/
void BasicTimer2_Callback(void){
	if(izquierdaOn == 1){
		//Se apagan los otros leds
		Apagado_Stop();
		Apagado_Stop();
		Apagado_Stop();
		Apagado_Stop();
		Apagado_Stop();
		Apagado_Stop();
		Apagado_Stop();
		Apagado_Stop();
		Apagado_Stop();
		Apagado_Stop();
		Apagado_Stop();
		Apagado_Stop();
		Apagado_Stop();
		Apagado_Stop();
		Apagado_Stop();
		Apagado_Stop();
		Apagado_Stop();
		Apagado_Stop();
		Apagado_Stop();
		Apagado_Stop();
		Apagado_Stop();
		Apagado_Stop();
		Apagado_Stop();
		Apagado_Stop();

		Apagado_Derecha();
		Apagado_Derecha();
		Apagado_Derecha();
		Apagado_Derecha();
		Apagado_Derecha();
		Apagado_Derecha();
		Apagado_Derecha();
		Apagado_Derecha();
		Apagado_Derecha();
		Apagado_Derecha();
		Apagado_Derecha();
		Apagado_Derecha();
		Apagado_Derecha();
		Apagado_Derecha();
		Apagado_Derecha();

		GPIO_TooglePin(&ledIzquierda);
		if(contadorLed == 1){
			Apagado();
			Apagado();
			Apagado();
			Apagado();
			Apagado();

			Apagado();
			Apagado();
			Apagado();
			Apagado();
			Apagado();

			Amarillo();
			Apagado();
			Apagado();
			Apagado();
			Amarillo();
		}else if(contadorLed == 2){
			Apagado();
			Apagado();
			Apagado();
			Apagado();
			Apagado();

			Apagado();
			Apagado();
			Apagado();
			Apagado();
			Apagado();

			Apagado();
			Amarillo();
			Apagado();
			Amarillo();
			Apagado();
		}else if(contadorLed == 3){
			Apagado();
			Apagado();
			Apagado();
			Apagado();
			Apagado();

			Apagado();
			Apagado();
			Apagado();
			Apagado();
			Apagado();

			Apagado();
			Apagado();
			Amarillo();
			Apagado();
			Apagado();
		}else if(contadorLed == 4){
			Apagado();
			Apagado();
			Apagado();
			Apagado();
			Apagado();

			Amarillo();
			Apagado();
			Apagado();
			Apagado();
			Amarillo();
			Apagado();
			Apagado();
			Apagado();
			Apagado();
			Apagado();
		}else if(contadorLed == 5){
			Apagado();
			Apagado();
			Apagado();
			Apagado();
			Apagado();
			Apagado();
			Amarillo();
			Apagado();
			Amarillo();
			Apagado();
			Apagado();
			Apagado();
			Apagado();
			Apagado();
			Apagado();
		}else if(contadorLed == 6){
			Apagado();
			Apagado();
			Apagado();
			Apagado();
			Apagado();
			Apagado();
			Apagado();
			Amarillo();
			Apagado();
			Apagado();
			Apagado();
			Apagado();
			Apagado();
			Apagado();
			Apagado();
		}else if(contadorLed == 7){
			Amarillo();
			Apagado();
			Apagado();
			Apagado();
			Amarillo();
			Apagado();
			Apagado();
			Apagado();
			Apagado();
			Apagado();
			Apagado();
			Apagado();
			Apagado();
			Apagado();
			Apagado();
		}else if(contadorLed == 8){
			Apagado();
			Amarillo();
			Apagado();
			Amarillo();
			Apagado();
			Apagado();
			Apagado();
			Apagado();
			Apagado();
			Apagado();
			Apagado();
			Apagado();
			Apagado();
			Apagado();
			Apagado();
		}else if(contadorLed == 9){
			Apagado();
			Apagado();
			Amarillo();
			Apagado();
			Apagado();
			Apagado();
			Apagado();
			Apagado();
			Apagado();
			Apagado();
			Apagado();
			Apagado();
			Apagado();
			Apagado();
			Apagado();
			contadorLed = 0;
		}
	}else if(derechaOn == 1){
		GPIO_TooglePin(&ledDerecha);
		//Se apagan los otros leds
		Apagado_Stop();
		Apagado_Stop();
		Apagado_Stop();
		Apagado_Stop();
		Apagado_Stop();
		Apagado_Stop();
		Apagado_Stop();
		Apagado_Stop();
		Apagado_Stop();
		Apagado_Stop();
		Apagado_Stop();
		Apagado_Stop();
		Apagado_Stop();
		Apagado_Stop();
		Apagado_Stop();
		Apagado_Stop();
		Apagado_Stop();
		Apagado_Stop();
		Apagado_Stop();
		Apagado_Stop();
		Apagado_Stop();
		Apagado_Stop();
		Apagado_Stop();
		Apagado_Stop();

		Apagado();
		Apagado();
		Apagado();
		Apagado();
		Apagado();
		Apagado();
		Apagado();
		Apagado();
		Apagado();
		Apagado();
		Apagado();
		Apagado();
		Apagado();
		Apagado();
		Apagado();

		if(contadorLed == 1){
			Amarillo_Derecha();
			Apagado_Derecha();
			Apagado_Derecha();
			Apagado_Derecha();
			Amarillo_Derecha();

			Apagado_Derecha();
			Apagado_Derecha();
			Apagado_Derecha();
			Apagado_Derecha();
			Apagado_Derecha();

			Apagado_Derecha();
			Apagado_Derecha();
			Apagado_Derecha();
			Apagado_Derecha();
			Apagado_Derecha();
		}else if(contadorLed == 2){
			Apagado_Derecha();
			Amarillo_Derecha();
			Apagado_Derecha();
			Amarillo_Derecha();
			Apagado_Derecha();

			Apagado_Derecha();
			Apagado_Derecha();
			Apagado_Derecha();
			Apagado_Derecha();
			Apagado_Derecha();

			Apagado_Derecha();
			Apagado_Derecha();
			Apagado_Derecha();
			Apagado_Derecha();
			Apagado_Derecha();
		}else if(contadorLed == 3){
			Apagado_Derecha();
			Apagado_Derecha();
			Amarillo_Derecha();
			Apagado_Derecha();
			Apagado_Derecha();

			Apagado_Derecha();
			Apagado_Derecha();
			Apagado_Derecha();
			Apagado_Derecha();
			Apagado_Derecha();

			Apagado_Derecha();
			Apagado_Derecha();
			Apagado_Derecha();
			Apagado_Derecha();
			Apagado_Derecha();
		}else if(contadorLed == 4){
			Apagado_Derecha();
			Apagado_Derecha();
			Apagado_Derecha();
			Apagado_Derecha();
			Apagado_Derecha();

			Amarillo_Derecha();
			Apagado_Derecha();
			Apagado_Derecha();
			Apagado_Derecha();
			Amarillo_Derecha();

			Apagado_Derecha();
			Apagado_Derecha();
			Apagado_Derecha();
			Apagado_Derecha();
			Apagado_Derecha();
		}else if(contadorLed == 5){
			Apagado_Derecha();
			Apagado_Derecha();
			Apagado_Derecha();
			Apagado_Derecha();
			Apagado_Derecha();

			Apagado_Derecha();
			Amarillo_Derecha();
			Apagado_Derecha();
			Amarillo_Derecha();
			Apagado_Derecha();

			Apagado_Derecha();
			Apagado_Derecha();
			Apagado_Derecha();
			Apagado_Derecha();
			Apagado_Derecha();
		}else if(contadorLed == 6){
			Apagado_Derecha();
			Apagado_Derecha();
			Apagado_Derecha();
			Apagado_Derecha();
			Apagado_Derecha();

			Apagado_Derecha();
			Apagado_Derecha();
			Amarillo_Derecha();
			Apagado_Derecha();
			Apagado_Derecha();

			Apagado_Derecha();
			Apagado_Derecha();
			Apagado_Derecha();
			Apagado_Derecha();
			Apagado_Derecha();
		}else if(contadorLed == 7){
			Apagado_Derecha();
			Apagado_Derecha();
			Apagado_Derecha();
			Apagado_Derecha();
			Apagado_Derecha();

			Apagado_Derecha();
			Apagado_Derecha();
			Apagado_Derecha();
			Apagado_Derecha();
			Apagado_Derecha();

			Amarillo_Derecha();
			Apagado_Derecha();
			Apagado_Derecha();
			Apagado_Derecha();
			Amarillo_Derecha();
		}else if(contadorLed == 8){
			Apagado_Derecha();
			Apagado_Derecha();
			Apagado_Derecha();
			Apagado_Derecha();
			Apagado_Derecha();

			Apagado_Derecha();
			Apagado_Derecha();
			Apagado_Derecha();
			Apagado_Derecha();
			Apagado_Derecha();

			Apagado_Derecha();
			Amarillo_Derecha();
			Apagado_Derecha();
			Amarillo_Derecha();
			Apagado_Derecha();
		}else if(contadorLed == 9){
			Apagado_Derecha();
			Apagado_Derecha();
			Apagado_Derecha();
			Apagado_Derecha();
			Apagado_Derecha();

			Apagado_Derecha();
			Apagado_Derecha();
			Apagado_Derecha();
			Apagado_Derecha();
			Apagado_Derecha();

			Apagado_Derecha();
			Apagado_Derecha();
			Amarillo_Derecha();
			Apagado_Derecha();
			Apagado_Derecha();
			contadorLed = 0;
		}
	}else if(stopOn == 2){
		GPIO_TooglePin(&ledStop);
		//Se apagan los otros leds
		Apagado();
		Apagado();
		Apagado();
		Apagado();
		Apagado();
		Apagado();
		Apagado();
		Apagado();
		Apagado();
		Apagado();
		Apagado();
		Apagado();
		Apagado();
		Apagado();
		Apagado();

		Apagado_Derecha();
		Apagado_Derecha();
		Apagado_Derecha();
		Apagado_Derecha();
		Apagado_Derecha();
		Apagado_Derecha();
		Apagado_Derecha();
		Apagado_Derecha();
		Apagado_Derecha();
		Apagado_Derecha();
		Apagado_Derecha();
		Apagado_Derecha();
		Apagado_Derecha();
		Apagado_Derecha();
		Apagado_Derecha();

		if(contadorLed == 1){
			Apagado_Stop();
			Rojo_Stop();
			Rojo_Stop();
			Rojo_Stop();
			Rojo_Stop();
			Rojo_Stop();
			Rojo_Stop();
			Rojo_Stop();
			Rojo_Stop();
			Apagado_Stop();
			Apagado_Stop();
			Apagado_Stop();
			Apagado_Stop();
			Apagado_Stop();
			Apagado_Stop();
			Apagado_Stop();
			Apagado_Stop();
			Apagado_Stop();
			Apagado_Stop();
			Apagado_Stop();
			Apagado_Stop();
			Apagado_Stop();
			Apagado_Stop();
			Apagado_Stop();
		}else if(contadorLed == 2){
			Rojo_Stop();
			Apagado_Stop();
			Apagado_Stop();
			Apagado_Stop();
			Apagado_Stop();
			Apagado_Stop();
			Apagado_Stop();
			Apagado_Stop();
			Apagado_Stop();
			Rojo_Stop();
			Rojo_Stop();
			Rojo_Stop();
			Rojo_Stop();
			Rojo_Stop();
			Rojo_Stop();
			Rojo_Stop();
			Rojo_Stop();
			Rojo_Stop();
			Rojo_Stop();
			Rojo_Stop();
			Rojo_Stop();
			Rojo_Stop();
			Rojo_Stop();
			Rojo_Stop();
			contadorLed = 0;
		}
	}
	else{
		GPIO_TooglePin(&ledStop);
		//Se apagan los otros leds
		Apagado();
		Apagado();
		Apagado();
		Apagado();
		Apagado();
		Apagado();
		Apagado();
		Apagado();
		Apagado();
		Apagado();
		Apagado();
		Apagado();
		Apagado();
		Apagado();
		Apagado();

		Apagado_Derecha();
		Apagado_Derecha();
		Apagado_Derecha();
		Apagado_Derecha();
		Apagado_Derecha();
		Apagado_Derecha();
		Apagado_Derecha();
		Apagado_Derecha();
		Apagado_Derecha();
		Apagado_Derecha();
		Apagado_Derecha();
		Apagado_Derecha();
		Apagado_Derecha();
		Apagado_Derecha();
		Apagado_Derecha();

		if(contadorLed == 1){
			Apagado_Stop();
			Verde_Stop();
			Verde_Stop();
			Verde_Stop();
			Verde_Stop();
			Verde_Stop();
			Verde_Stop();
			Verde_Stop();
			Verde_Stop();
			Apagado_Stop();
			Apagado_Stop();
			Apagado_Stop();
			Apagado_Stop();
			Apagado_Stop();
			Apagado_Stop();
			Apagado_Stop();
			Apagado_Stop();
			Apagado_Stop();
			Apagado_Stop();
			Apagado_Stop();
			Apagado_Stop();
			Apagado_Stop();
			Apagado_Stop();
			Apagado_Stop();
		}else if(contadorLed == 2){
			Verde_Stop();
			Apagado_Stop();
			Apagado_Stop();
			Apagado_Stop();
			Apagado_Stop();
			Apagado_Stop();
			Apagado_Stop();
			Apagado_Stop();
			Apagado_Stop();
			Verde_Stop();
			Verde_Stop();
			Verde_Stop();
			Verde_Stop();
			Verde_Stop();
			Verde_Stop();
			Verde_Stop();
			Verde_Stop();
			Verde_Stop();
			Verde_Stop();
			Verde_Stop();
			Verde_Stop();
			Verde_Stop();
			Verde_Stop();
			Verde_Stop();
			contadorLed = 0;
		}
	}
	contadorLed++;
}

/*Funcion callback del usart6*/
void usart6Rx_Callback(void){
	//Almacenamos el dato que se envio  en la variable rxData
	rxDataHC_05 = getRxData();
	if((rxDataHC_05 != '\r') && (rxDataHC_05 != '\n')){
		dato = rxDataHC_05;
	}
}
