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
#include "ExtiDriver.h"
#include "GPIOxDriver.h"
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

/*==============================
 *     Handler del USART
 *===============================*/
//Pines para la comunicacion USART
GPIO_Handler_t handlerPinTX = {0};
GPIO_Handler_t handlerPinRX = {0};
//Handler para la configuracion USART
USART_Handler_t handlerTerminal = {0};

/*==========================
 *    Handler del PLL
 *==========================*/
//Handler para la configuracion del PLL
PLL_Config_t handlerPLL = {0};

/*=========================
 * Cabeceras de las funciones
 * ==========================*/
void initSystem(void);
//Funcion encargada de analizar los comandos que envia el usuario
void parseCommands(char *prtBufferReception);
//Funcion para obtener los comandos del usuario
void cmdUsuario(void);

/*=========================
 * 	 Variables Globales
 * ==========================*/
uint8_t rxData = 0; //Esta variable se encarga de guardar el caracter que el usuario hunde en el teclado
uint16_t contadorComando = 0; //Variable encargada de saber la posicion en la que se encuentra en el arreglo de bufferComando
char bufferComando[BUFFER_SIZE] = {0}; //Arreglo para almacenar los comandos que el usuario introduce
uint8_t stringComplete = 0;	//Bandera encargada de activarse cuando el usuario termina de introducir un comando
/*Variable utilizadas en la funcion parseCommand()*/
char cmd[BUFFER_SIZE] = {0};
unsigned int firstParameter = 0;
unsigned int secondParameter = 0;
uint16_t thirdParameter = 0;
char userMsg[BUFFER_SIZE] = {0};

char bufferData[BUFFER_SIZE + 24] = "Funcionando Prueba Proyecto...\n";

int main(void){
	//Cargamos la configuracion de los pines
	initSystem();
	//Mensaje de que esta funcionando coreectamente el USART
	writeMsg(&handlerTerminal, bufferData);

	while(1){
		//Captualos los comandos ingrasados por el usuario
		cmdUsuario();
	}
}


/*=================================================
 * Funcion para cargar la configuracion de los pines
 * ================================================*/
void initSystem(void){
	/*=======================
	 * Configuracion del PLL
	 *=======================*/
//	handlerPLL.PLL_ON											= PLL_ENABLE;
//	handlerPLL.PLL_PLLM											= 4;
//	handlerPLL.PLL_PLLN											= 50;
//	handlerPLL.PLL_PLLP											= PLLP_2;
//	handlerPLL.PLL_MCO1PRE										= PLL_MCO1PRE_2;
//	handlerPLL.APB1_PRESC										= PRESC_APB1_2;
//	handlerPLL.APB2_PRESC										= PRESC_APB2_NONE;
//	handlerPLL.HSI_TRIM											= 0b01100;
//	//Cargar la configuracion del PLL
//	ConfigPLL(&handlerPLL);

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
	handlerTimerBlinkyPin.TIMx_Config.TIMx_speed				= BTIMER_SPEED_100us;
	handlerTimerBlinkyPin.TIMx_Config.TIMx_interruptEnable		= SET;
	//Cargar la configuracion del Timer
	BasicTimer_Config(&handlerTimerBlinkyPin);

	/*========================================
	 * Configuracion de la comunicacion serial
	 *========================================*/
	//Configuracion para el pin de transmicion
	handlerPinTX.pGPIOx 										= GPIOA;
	handlerPinTX.GPIO_PinConfig.GPIO_PinNumber					= PIN_2;
	handlerPinTX.GPIO_PinConfig.GPIO_PinMode					= GPIO_MODE_ALTFN;
	handlerPinTX.GPIO_PinConfig.GPIO_PinOPType					= GPIO_OTYPER_PUSHPULL;
	handlerPinTX.GPIO_PinConfig.GPIO_PinPuPdControl				= GPIO_PUPDR_NOTHING;
	handlerPinTX.GPIO_PinConfig.GPIO_PinSpeed					= GPIO_OSPEED_FAST;
	handlerPinTX.GPIO_PinConfig.GPIO_PinAltFunMode				= AF8;
	//Cargar la configuracion del pin
	GPIO_Config(&handlerPinTX);
	//configuracion del pin para la recepcion
	handlerPinRX.pGPIOx											= GPIOA;
	handlerPinRX.GPIO_PinConfig.GPIO_PinNumber					= PIN_3;
	handlerPinRX.GPIO_PinConfig.GPIO_PinMode					= GPIO_MODE_ALTFN;
	handlerPinRX.GPIO_PinConfig.GPIO_PinOPType					= GPIO_OTYPER_PUSHPULL;
	handlerPinRX.GPIO_PinConfig.GPIO_PinPuPdControl				= GPIO_PUPDR_NOTHING;
	handlerPinRX.GPIO_PinConfig.GPIO_PinSpeed					= GPIO_OSPEED_FAST;
	handlerPinRX.GPIO_PinConfig.GPIO_PinAltFunMode				= AF8;
	//Cargar la configuracion del pin
	GPIO_Config(&handlerPinRX);
	//Configuracion del USART
	handlerTerminal.ptrUSARTx 									= USART2;
	handlerTerminal.USART_Config.USART_baudrate 				= USART_BAUDRATE_115200;
	handlerTerminal.USART_Config.USART_datasize					= USART_DATASIZE_8BIT;
	handlerTerminal.USART_Config.USART_parity					= USART_PARITY_NONE;
	handlerTerminal.USART_Config.USART_stopbits					= USART_STOPBIT_1;
	handlerTerminal.USART_Config.USART_mode						= USART_MODE_RXTX;
	handlerTerminal.USART_Config.USART_enableIntRX				= USART_RX_INTERRUP_ENABLE;
	handlerTerminal.USART_Config.USART_enableIntTX				= USART_TX_INTERRUP_DISABLE;
//	handlerTerminal.USART_Config.USART_PLL_Enable				= PLL_ENABLE;
	//Cargar la configuracion del USART
	USART_Config(&handlerTerminal);
}


/*=======================================
 *   Funciones utilizadas en el Main
 *=======================================*/
//Funcion para analizar los comandos que introduce el usuario
void parseCommands(char *prtBufferReception){
	/*Esta funcion se encarga de leer la cadena de caracteres a la que apunta 'prt' y la divide
	 * y la almacena en tres elementos:
	 * 		1) un string llamado 'cmd'
	 * 		2) un numero intiger llamado fiertParameter
	 * 		3) un numero intiger llamado secondParameter
	 * 		4) un string llamado 'userMsg'
	 * 	De esta forma podemos introducir informacion al micro desde el puerto serial*/
	sscanf(prtBufferReception, "%s %u %u %s %hd", cmd, &firstParameter, &secondParameter, userMsg, &thirdParameter);

	//El primer comando imprime una lista de los comandos que existen
	if(strcmp(cmd, "help") == 0){
		writeMsg(&handlerTerminal, "Help Menu  CMDs\n");
		writeMsg(&handlerTerminal, "1) dummy #A #B -- dummy cmd, #A and #B are uint32_t\n");
		writeMsg(&handlerTerminal, "2) usermsg #msg -- usermsg cmd, #msg is a char(msg to show)\n");

	}
	//El segundo comando imprime los numeros A y B que el usuario escribe
	else if(strcmp(cmd, "dummy") == 0){
		writeMsg(&handlerTerminal, "CMD: dummy\n");

		sprintf(bufferData, "number A = %u\n", firstParameter);
		writeMsg(&handlerTerminal, bufferData);

		sprintf(bufferData, "number B = %u\n", secondParameter);
		writeMsg(&handlerTerminal, bufferData);
		unsigned int suma = 0;
		suma = firstParameter + secondParameter;
		sprintf(bufferData, "number C = %u\n", suma);
		writeMsg(&handlerTerminal, bufferData);
	}
	//El tercer comando imprime el mensaje que el usuario escribe
	else if(strcmp(cmd, "usermsg") == 0){
		writeMsg(&handlerTerminal, "CMD: usermsg\n");

		sprintf(bufferData, "User Msg = %s\n", userMsg);
		writeMsg(&handlerTerminal, bufferData);
	}
	//Limpiamos los registros
	firstParameter = 0;
	secondParameter = 0;
}

//Funcion para obtener los comandos del usuario
void cmdUsuario(void){
	/*Ciclo if para la capturar los comandos que el usuario manda*/
	if(rxData != '\0'){
		//Almacenamos el caracter introducido por el usuario en el arreglo
		bufferComando[contadorComando] = rxData;
		//Aumentamos en uno el contador
		contadorComando++;
		/*Cuando el usuario introdusca '@' es cuando ya se ha finalizado de introducir el comando
		 * por lo que se debe de levantar la bandera stringComplete.*/
		if(rxData == '@'){
			//Se levanta la bandera
			stringComplete = 1;
			//Se cambia el ultimo caracter introducido por '\0'
			bufferComando[(contadorComando - 1)] = '\0';
			//Se reinicia el contador
			contadorComando = 0;
		}
		rxData = '\0';
	}
	/*Ciclo if para revisar si el usuario termino de escribir el comando*/
	if(stringComplete == 1){
		//Cuando se finaliza de introducir el comando se entra a la funcion parseCommands()
		parseCommands(bufferComando);
		//Se baja la bandera
		stringComplete = 0;
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
/*Funcion callback del usart*/
void usart2Rx_Callback(void){
	//Almacenamos el dato que se envio  en la variable rxData
	rxData = getRxData();
}

