/*
 * solucionExamen.c
 *
 *  Created on: Jun 2, 2023
 *      Author: samuel
 */
#include <stm32f4xx.h>

#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "AdcDriver.h"
#include "BasicTimer.h"
#include "ExtiDriver.h"
#include "GPIOxDriver.h"
#include "I2CDriver.h"
#include "MPUxDriver.h"
#include "PLLDriver.h"
#include "PwmDriver.h"
#include "SysTickDriver.h"
#include "USARTxDriver.h"

/*==========================
 *  		Macros
 *==========================*/
#define BUFFER_SIZE 		25


/*==========================
 *  Handler del BlinkyPin
 *==========================*/
GPIO_Handler_t BlinkyPin = {0};
BasicTimer_Handler_t handlerTimerBlinkyPin = {0};

/*==============================
 *     Handler del USART
 *===============================*/
GPIO_Handler_t handlerPinTX = {0};
GPIO_Handler_t handlerPinRX = {0};
USART_Handler_t handlerTerminal = {0};

/*==========================
 *   Handler para el I2C
 * ==========================*/
//Acelerometro
GPIO_Handler_t SDAAccel = {0};
GPIO_Handler_t SCLAccel = {0};
I2C_Handler_t handlerAccelerometer = {0};
uint8_t i2cBuffer = 0;

/*==========================
 *   Handler del MCO1
 *==========================*/
GPIO_Handler_t Mco1Pin = {0};

/*==========================
 *    Handler del PLL
 *==========================*/
PLL_Config_t handlerPLL = {0};

/*==========================
 *   Handler del ADC
 *==========================*/
ADC_Config_t handlerAdc = {0};

/*=========================
 * Cabeceras de las funciones
 * ==========================*/
void initSystem(void);
void parseCommands(char *prtBufferReception);

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
char userMsg[BUFFER_SIZE] = {0};

char bufferData[BUFFER_SIZE] = "accel MPU-6050 testing..";



int main(void){
	//Cargamos la configuracion de los pines
	initSystem();

	while(1){
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

			if(rxData == 'w'){
				sprintf(bufferData, "WHO_AM_I? (r)\n");
				writeMsg(&handlerTerminal, bufferData);

				i2cBuffer = i2c_readSingleRegister(&handlerAccelerometer, WHO_AM_I);
				sprintf(bufferData, "DataRead = 0x%x \n", (unsigned int) i2cBuffer);
				writeMsg(&handlerTerminal, bufferData);
			}
			rxData = '\0';
		}
		if(stringComplete == 1){
			//Cuando se finaliza de introducir el comando se entra a la funcion parseCommands()
			parseCommands(bufferComando);
			//Se baja la bandera
			stringComplete = 0;
		}
	}
}



/*=================================================
 * Funcion para cargar la configuracion de los pines
 * ================================================*/
void initSystem(void){
	/*=======================
	 * Configuracion del PLL
	 *=======================*/
	handlerPLL.PLL_ON											= PLL_DISABLE;
//	handlerPLL.PLL_PLLM											= 10;
//	handlerPLL.PLL_PLLN											= 100;
//	handlerPLL.PLL_PLLP											= PLLP_2;
//	handlerPLL.PLL_MCO1PRE										= PLL_MCO1PRE_4;
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
	handlerTimerBlinkyPin.ptrTIMx								= TIM2;
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
	handlerTerminal.USART_Config.USART_enableIntTX				= USART_TX_INTERRUP_ENABLE;
	handlerTerminal.USART_Config.USART_PLL_Enable				= 0;
	//Cargar la configuracion del USART
	USART_Config(&handlerTerminal);

	/*=========================================================
	 * Configuracion para el protocolo I2C para el Acelerometro
	 *==========================================================*/
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
	handlerAccelerometer.I2C_RISE								= I2C_MAX_RISE_TIME_FM;
	handlerAccelerometer.I2C_SPEED								= I2C_MODE_FM_SPEED_400KHz;
	handlerAccelerometer.I2C_MAIN_CLOCK							= MAIN_CLOCK_16_MHz_FOR_I2C;
	i2c_Config(&handlerAccelerometer);

	/*=====================================
	 * Configuracion del pin para el MCO1
	 *====================================*/
	Mco1Pin.pGPIOx												= GPIOA;
	Mco1Pin.GPIO_PinConfig.GPIO_PinNumber						= PIN_8;
	Mco1Pin.GPIO_PinConfig.GPIO_PinMode							= GPIO_MODE_ALTFN;
	Mco1Pin.GPIO_PinConfig.GPIO_PinOPType						= GPIO_OTYPER_OPENDRAIN;
	Mco1Pin.GPIO_PinConfig.GPIO_PinPuPdControl					= GPIO_PUPDR_NOTHING;
	Mco1Pin.GPIO_PinConfig.GPIO_PinSpeed						= GPIO_OSPEED_FAST;
	Mco1Pin.GPIO_PinConfig.GPIO_PinAltFunMode					= AF0;
	//Cargamos la configuracion
	GPIO_Config(&Mco1Pin);

	/*=====================================
	 * 	   Configuracion para el ADC
	 *====================================*/
}

/*=======================================
 *   Funciones utilizadas en el Main
 *=======================================*/
void parseCommands(char *prtBufferReception){
	/*Esta funcion se encarga de leer la cadena de caracteres a la que apunta 'prt' y la divide
	 * y la almacena en tres elementos:
	 * 		1) un string llamado 'cmd'
	 * 		2) un numero intiger llamado fiertParameter
	 * 		3) un numero intiger llamado secondParameter
	 * 		4) un string llamado 'userMsg'
	 * 	De esta forma podemos introducir informacion al micro desde el puerto serial*/
	sscanf(prtBufferReception, "%s %u %u %s", cmd, &firstParameter, &secondParameter, userMsg);

	//El primer comando imprime una lista de los comandos que existen
	if(strcmp(cmd, "help") == 0){
		writeMsg(&handlerTerminal, "Help Menu  CMDs\n");
		writeMsg(&handlerTerminal, "1) Control MCO1\n");
		writeMsg(&handlerTerminal, "2) Control MCO1\n");
		writeMsg(&handlerTerminal, "3) Configurar RTC\n");
		writeMsg(&handlerTerminal, "4) Configurar RTC\n");
		writeMsg(&handlerTerminal, "5) Configurar RTC\n");
		writeMsg(&handlerTerminal, "6) Configurar RTC\n");
		writeMsg(&handlerTerminal, "7) Adquisicion ADC\n");
		writeMsg(&handlerTerminal, "8) Adquisicion ADC\n");
		writeMsg(&handlerTerminal, "9) Utilizar acelerometro(captura de datos)\n");
		writeMsg(&handlerTerminal, "10) Utilizar acelerometro(CMSIS-FFT)\n");

	}
}

/*=======================================
 * Funciones Callback de los perifericos
 *=======================================*/
//Funcion callback del Timer
void BasicTimer2_Callback(void){
	//Se hace el blinky del pin
	GPIO_TooglePin(&BlinkyPin);
}
/*Funcion callback del usart*/
void usart2Rx_Callback(void){
	//Almacenamos el dato que se envio  en la variable rxData
	rxData = getRxData();
}
