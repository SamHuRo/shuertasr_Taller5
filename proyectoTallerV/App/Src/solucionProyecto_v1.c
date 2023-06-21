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

/*==============================
 *     Handler del USART
 *===============================*/
//Pines para la comunicacion USART
GPIO_Handler_t handlerPinTX = {0};
GPIO_Handler_t handlerPinRX = {0};
//Handler para la configuracion USART
USART_Handler_t handlerTerminal = {0};

/*===================================
 *     Handler del USART Bluetooth
 *====================================*/
//Pines para la comunicacion USART
GPIO_Handler_t handlerPinTXHC_05 = {0};
GPIO_Handler_t handlerPinRXHC_05 = {0};
//Handler para la configuracion USART
USART_Handler_t handlerTerminalHC_05 = {0};

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
uint8_t i2cBuffer = 0;

/*=========================
 * Cabeceras de las funciones
 * ==========================*/
void initSystem(void);
//Funcion encargada de analizar los comandos que envia el usuario
void parseCommands(char *prtBufferReception);
//Funcion para obtener los comandos del usuario
void cmdUsuario(void);
//Funcion para obtener los comandos del modulo HC-05
void cmdHC_05(void);
//Funcion para inicializar el Acelerometro
void initAccel(void);
//Funcion al hundir una tecla por el usuario
void tecladoAccel(void);

/*=========================
 * 	 Variables Globales
 * ==========================*/
uint8_t rxData = 0; //Esta variable se encarga de guardar el caracter que el usuario hunde en el teclado
uint8_t rxDataHC_05 = 0;
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
	/*Activar el co-prcesador matematico o unidad de punto flotante*/
	SCB->CPACR |= (0xF << 20);
	//Cargamos la configuracion de los pines
	initSystem();
	//Mensaje de que esta funcionando coreectamente el USART
	writeMsg(&handlerTerminal, bufferData);
	//Inicializacion del acelerometro
	initAccel();
	while(1){
		//Captualos los comandos ingrasados por el usuario
//		cmdUsuario();
		//Capturar la tecla que hunde el usuario
		tecladoAccel();
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
	handlerTimerBlinkyPin.TIMx_Config.TIMx_speed				= BTIMER_SPEED_100us;
	handlerTimerBlinkyPin.TIMx_Config.TIMx_interruptEnable		= SET;
	//Cargar la configuracion del Timer
	BasicTimer_Config(&handlerTimerBlinkyPin);

	/*========================================
	 * Configuracion de la comunicacion serial
	 *========================================*/
	/*---------------------- Configuracion USART2, comunicación terminal ----------------------*/
	//Configuracion para el pin de transmicion
	handlerPinTX.pGPIOx 										= GPIOA;
	handlerPinTX.GPIO_PinConfig.GPIO_PinNumber					= PIN_2;
	handlerPinTX.GPIO_PinConfig.GPIO_PinMode					= GPIO_MODE_ALTFN;
	handlerPinTX.GPIO_PinConfig.GPIO_PinOPType					= GPIO_OTYPER_PUSHPULL;
	handlerPinTX.GPIO_PinConfig.GPIO_PinPuPdControl				= GPIO_PUPDR_NOTHING;
	handlerPinTX.GPIO_PinConfig.GPIO_PinSpeed					= GPIO_OSPEED_FAST;
	handlerPinTX.GPIO_PinConfig.GPIO_PinAltFunMode				= AF7;
	//Cargar la configuracion del pin
	GPIO_Config(&handlerPinTX);
	//configuracion del pin para la recepcion
	handlerPinRX.pGPIOx											= GPIOA;
	handlerPinRX.GPIO_PinConfig.GPIO_PinNumber					= PIN_3;
	handlerPinRX.GPIO_PinConfig.GPIO_PinMode					= GPIO_MODE_ALTFN;
	handlerPinRX.GPIO_PinConfig.GPIO_PinOPType					= GPIO_OTYPER_PUSHPULL;
	handlerPinRX.GPIO_PinConfig.GPIO_PinPuPdControl				= GPIO_PUPDR_NOTHING;
	handlerPinRX.GPIO_PinConfig.GPIO_PinSpeed					= GPIO_OSPEED_FAST;
	handlerPinRX.GPIO_PinConfig.GPIO_PinAltFunMode				= AF7;
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
	handlerTerminalHC_05.USART_Config.USART_baudrate 				= USART_BAUDRATE_115200;
	handlerTerminalHC_05.USART_Config.USART_datasize				= USART_DATASIZE_8BIT;
	handlerTerminalHC_05.USART_Config.USART_parity					= USART_PARITY_NONE;
	handlerTerminalHC_05.USART_Config.USART_stopbits				= USART_STOPBIT_1;
	handlerTerminalHC_05.USART_Config.USART_mode					= USART_MODE_RXTX;
	handlerTerminalHC_05.USART_Config.USART_enableIntRX				= USART_RX_INTERRUP_ENABLE;
	handlerTerminalHC_05.USART_Config.USART_enableIntTX				= USART_TX_INTERRUP_DISABLE;
//	handlerTerminalHC_05.USART_Config.USART_PLL_Enable				= PLL_ENABLE;
	//Cargar la configuracion del USART
	USART_Config(&handlerTerminalHC_05);

	/*--------------------- Configuracion para el protocolo I2C para el Acelerometro ------------------*/
	//Configuracion de los pines para el I2C -> SCL
	SCLAccel.pGPIOx												= GPIOB;
	SCLAccel.GPIO_PinConfig.GPIO_PinNumber						= PIN_8;
	SCLAccel.GPIO_PinConfig.GPIO_PinMode						= GPIO_MODE_ALTFN;
	SCLAccel.GPIO_PinConfig.GPIO_PinOPType						= GPIO_OTYPER_OPENDRAIN;
	SCLAccel.GPIO_PinConfig.GPIO_PinPuPdControl					= GPIO_PUPDR_NOTHING;
	SCLAccel.GPIO_PinConfig.GPIO_PinSpeed						= GPIO_OSPEED_FAST;
	SCLAccel.GPIO_PinConfig.GPIO_PinAltFunMode					= AF4;
	GPIO_Config(&SCLAccel);
	//Configuracion de los pines para el I2C -> SDA
	SDAAccel.pGPIOx												= GPIOB;
	SDAAccel.GPIO_PinConfig.GPIO_PinNumber						= PIN_9;
	SDAAccel.GPIO_PinConfig.GPIO_PinMode						= GPIO_MODE_ALTFN;
	SDAAccel.GPIO_PinConfig.GPIO_PinOPType						= GPIO_OTYPER_OPENDRAIN;
	SDAAccel.GPIO_PinConfig.GPIO_PinPuPdControl					= GPIO_PUPDR_NOTHING;
	SDAAccel.GPIO_PinConfig.GPIO_PinSpeed						= GPIO_OSPEED_FAST;
	SDAAccel.GPIO_PinConfig.GPIO_PinAltFunMode					= AF4;
	GPIO_Config(&SDAAccel);
	//Configuracion de la cominicacion I2C
	handlerAccelerometer.ptrI2Cx								= I2C1;
	handlerAccelerometer.modeI2C								= I2C_MODE_FM;
	handlerAccelerometer.slaveAddress							= ACCEL_ADDRESS;
//	handlerAccelerometer.PLL_ON									= PLL_ENABLE;
	i2c_Config(&handlerAccelerometer);
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

	sprintf(bufferData, "CMD %s\n", cmd);
	writeMsg(&handlerTerminal, bufferData);

	writeMsg(&handlerTerminalHC_05, cmd);

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
/*Funcion que guarda las interacciones del acelerometro con el teclado*/
void tecladoAccel(void){
	if(rxData != '\0'){
		if(rxData == 'a'){
			uint8_t AccelX_low = i2c_readSingleRegister(&handlerAccelerometer, ACCEL_XOUT_L);
			uint8_t AccelX_high = i2c_readSingleRegister(&handlerAccelerometer, ACCEL_XOUT_H);
			int16_t AccelX = AccelX_high << 8 | AccelX_low;
			uint8_t AccelY_low = i2c_readSingleRegister(&handlerAccelerometer, ACCEL_YOUT_L);
			uint8_t AccelY_high = i2c_readSingleRegister(&handlerAccelerometer, ACCEL_YOUT_H);
			int16_t AccelY = AccelY_high << 8 | AccelY_low;
			float ang_x = atan(AccelX/sqrt(pow(AccelX,2) + pow(AccelY,2)))*(180.0/3.14);
			sprintf(bufferData, "AngX = %.2f \n", ang_x);
			writeMsgTX(&handlerTerminal, bufferData);
			rxData = '\0';
		}
		else{
			rxData = '\0';
		}
	}
}
//Funcion para inicializar el acelerometro
void initAccel(void){
	sprintf(bufferData, "WHO_AM_I? (r)\n");
	writeMsgTX(&handlerTerminal, bufferData);

	i2cBuffer = i2c_readSingleRegister(&handlerAccelerometer, WHO_AM_I);
	sprintf(bufferData, "DataRead = 0x%x \n", (unsigned int) i2cBuffer);
	writeMsgTX(&handlerTerminal, bufferData);

	__NOP();
	__NOP();
	__NOP();
	__NOP();
	__NOP();
	__NOP();

	sprintf(bufferData, "PWR_MGMT_1 stat (r)\n");
	writeMsgTX(&handlerTerminal, bufferData);

	i2cBuffer = i2c_readSingleRegister(&handlerAccelerometer, PWR_MGMT_1);
	sprintf(bufferData, "DataRead = 0x%x \n", (unsigned int) i2cBuffer);
	writeMsgTX(&handlerTerminal, bufferData);

	__NOP();
	__NOP();
	__NOP();
	__NOP();
	__NOP();
	__NOP();

	sprintf(bufferData, "PWR_MGMT_1 reset (w)\n");
	writeMsgTX(&handlerTerminal, bufferData);

	i2c_writeSingleRegister(&handlerAccelerometer, PWR_MGMT_1, 0x00);
}

/*=======================================
 * Funciones Callback de los perifericos
 *=======================================*/
/*Funcion callback del Timer 5*/
void BasicTimer5_Callback(void){
	//Se hace el blinky del pin
	GPIO_TooglePin(&BlinkyPin);
}
/*Funcion callback del usart2*/
void usart2Rx_Callback(void){
	//Almacenamos el dato que se envio  en la variable rxData
	rxData = getRxData();
}
/*Funcion callback del usart6*/
void usart6Rx_Callback(void){
	//Almacenamos el dato que se envio  en la variable rxData
	rxDataHC_05 = getRxData();
}
