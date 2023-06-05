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
 *Configuracion del BlinkyPin
 *==========================*/
GPIO_Handler_t BlinkyPin = {0};
BasicTimer_Handler_t handlerTimerBlinkyPin = {0};

/*==============================
 * Configuracion del USART
 *===============================*/
GPIO_Handler_t handlerPinTX = {0};
GPIO_Handler_t handlerPinRX = {0};
USART_Handler_t handlerTerminal = {0};

/*==========================
 * Configuracion para el I2C
 * ==========================*/
//Acelerometro
GPIO_Handler_t SDAAccel = {0};
GPIO_Handler_t SCLAccel = {0};
I2C_Handler_t handlerAccelerometer = {0};
uint8_t i2cBuffer = 0;

/*==========================
 * Configuracion del MCO1
 *==========================*/
GPIO_Handler_t Mco1Pin = {0};

/*==========================
 * Configuracion del PLL
 *==========================*/
PLL_Config_t handlerPLL = {0}; //Se va a guardar la configuracion del PLL

/*=========================
 * Cabeceras de las funciones
 * ==========================*/
void initSystem(void);

/*=========================
 * 	 Variables Globales
 * ==========================*/
uint8_t rxData = 0;
uint16_t contadorRecepcion = 0;
uint8_t bufferReception[] = {0};
bool stringComplete = false;
unsigned int firstParameter;
unsigned int secondParameter;
char bufferData[24] = "accel MPU-6050 testing..";

int main(void){
	//Cargamos la configuracion de los pines
	initSystem();

	while(1){
		if(rxData != '\0'){
					writeChar(&handlerTerminal, rxData);

					if(rxData == 'w'){
						sprintf(bufferData, "WHO_AM_I? (r)\n");
						writeMsg(&handlerTerminal, bufferData);

						i2cBuffer = i2c_readSingleRegister(&handlerAccelerometer, WHO_AM_I);
						sprintf(bufferData, "DataRead = 0x%x \n", (unsigned int) i2cBuffer);
						writeMsg(&handlerTerminal, bufferData);
						rxData = '\0';
					}
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
//	handlerPLL.PLL_PLLM											= 10;
//	handlerPLL.PLL_PLLN											= 100;
//	handlerPLL.PLL_PLLP											= PLLP_2;
//	handlerPLL.PLL_MCO1PRE										= PLL_MCO1PRE_4;
	handlerPLL.PLL_ON											= PLL_DISABLE;
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
