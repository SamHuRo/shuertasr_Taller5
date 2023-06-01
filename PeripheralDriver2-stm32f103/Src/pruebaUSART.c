/*
 * pruebaUSART.c
 *
 *  Created on: May 12, 2023
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
#include "USARTxDriver.h"

/*Elementos para la comunicacion serial*/
GPIO_Handler_t handlerPinTX = {0};
GPIO_Handler_t handlerPinRX = {0};
USART_Handler_t usart2Comm = {0};
uint32_t printMsg = 0;
uint8_t sendMsg = 0;
char mensaje[] = "\nPrueba de sonido!\n";
char bufferMsg[64] = {0};
uint8_t usart2DataReceived = 0;

/*Elementos para el timer*/
GPIO_Handler_t BlinkyPin = {0};      //BlinkyPin (pin de estado)
BasicTimer_Handler_t handlerTimerBlinkyPin = {0};  //Definicion del handler para el timer del blinkypin o pin de estado

/*============================
 * Cabeceras de las funciones
 *==========================*/
void initSystem(void);


int main(void)
{

	//Se llama a la funcion initSystem para iniciar el sistema
	initSystem();

    /* Loop forever */
	while(1){

		//Primer ejemplo del usart
//		if(sendMsg > 4){
//			writeChar(&usart2Comm, 'H');
//			writeChar(&usart2Comm, 'O');
//			writeChar(&usart2Comm, 'L');
//			writeChar(&usart2Comm, 'A');
//			writeChar(&usart2Comm, ' ');
//			writeChar(&usart2Comm, 'M');
//			writeChar(&usart2Comm, 'U');
//			writeChar(&usart2Comm, 'N');
//			writeChar(&usart2Comm, 'D');
//			writeChar(&usart2Comm, 'O');
//			sendMsg = 0;
//		}

		//Segunda prueba con el usart, enviando un mensaje
		if(sendMsg > 10){
			writeMsg(&usart2Comm, mensaje);
			sendMsg = 0;
		}


		/*Se va a actuvar cuando se detecte la interrupcion, ya que una interrupcion no se puede demorar mas de 1ms, poniendo esta opcion en el main,
		 * el tiempo de la interrupcion se demora 5 micro-segundos*/
		if(usart2DataReceived != '\0'){
			sprintf(bufferMsg, "Recibido char = %c \n", usart2DataReceived);
			writeMsg(&usart2Comm, bufferMsg);
			/*Se vuelve a poner la variable en nulo para que no se ejecute mas de una vez*/
			usart2DataReceived = '\0';
		}


	}
	return 0;
}

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
	handlerTimerBlinkyPin.TIMx_Config.TIMx_period			= 2500;
	handlerTimerBlinkyPin.TIMx_Config.TIMx_speed			= BTIMER_SPEED_100us;
	handlerTimerBlinkyPin.TIMx_Config.TIMx_interruptEnable	= SET;
	//Cargar la configuracion del Timer
	BasicTimer_Config(&handlerTimerBlinkyPin);

	/*Configuracion de la comunicacion serial*/
	//Configuracion para el pin de transmicion
	handlerPinTX.pGPIOx 								= GPIOA;
	handlerPinTX.GPIO_PinConfig.GPIO_PinNumber			= PIN_2;
	handlerPinTX.GPIO_PinConfig.GPIO_PinMode			= GPIO_MODE_AF;
	handlerPinTX.GPIO_PinConfig.GPIO_PinOPType			= GPIO_OTYPER_PUSHPULL;
	//Cargar la configuracion del pin
	GPIO_Config(&handlerPinTX);

	//configuracion del pin para la recepcion
	handlerPinRX.pGPIOx									= GPIOA;
	handlerPinRX.GPIO_PinConfig.GPIO_PinNumber			= PIN_3;
	handlerPinRX.GPIO_PinConfig.GPIO_PinMode			= GPIO_MODE_AF;
	handlerPinRX.GPIO_PinConfig.GPIO_PinOPType			= GPIO_OTYPER_PUSHPULL;
	//Cargar la configuracion del pin
	GPIO_Config(&handlerPinRX);

	//Configuracion del USART
	usart2Comm.ptrUSARTx 							= USART2;
	usart2Comm.USART_Config.USART_baudrate 			= USART_BAUDRATE_115200;
	usart2Comm.USART_Config.USART_datasize			= USART_DATASIZE_8BIT;
	usart2Comm.USART_Config.USART_parity			= USART_PARITY_NONE;
	usart2Comm.USART_Config.USART_stopbits			= USART_STOPBIT_1;
	usart2Comm.USART_Config.USART_mode				= USART_MODE_RXTX;
	usart2Comm.USART_Config.USART_enableIntRX		= USART_RX_INTERRUP_ENABLE;
	//Cargar la configuracion del USART
	USART_Config(&usart2Comm);
}
/*Callback del timer2 -> Hace blinky al led*/
void BasicTimer2_Callback(void){
	GPIO_TooglePin(&BlinkyPin);
	sendMsg++;
}
/*Esta funcion se ejecuta cada vez que un caracter es recibido
 * por el puerto USART2*/
void usart2Rx_Callback(void){
	/*Verificacion del tiempo que se demora la interrupcion*/
	usart2DataReceived = getRxData();
}
