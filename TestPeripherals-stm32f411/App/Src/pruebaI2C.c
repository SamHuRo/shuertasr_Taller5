/*
 * pruebaI2C.c
 *
 *  Created on: May 18, 2023
 *      Author: samuel
 */
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include <stm32f4xx.h>

#include "GPIOxDriver.h"
#include "ExtiDriver.h"
#include "BasicTimer.h"
#include "USARTxDriver.h"
#include "SysTickDriver.h"
#include "PwmDriver.h"
#include "PLLDriver.h"
#include "I2CDriver.h"

/*Definicion de variables*/
GPIO_Handler_t handlerLedOK = {0};
GPIO_Handler_t handlerPinTX = {0};
GPIO_Handler_t handlerPinRX = {0};

BasicTimer_Handler_t handlerStateOKTimer = {0};

USART_Handler_t handlerCommTerminal = {0};
uint8_t rxData = 0;
char bufferData[24] = "Accel MPU-6050 testing...";

uint32_t systemTicks = 0;
uint32_t systemTicksStart = 0;
uint32_t systemTicksEnd = 0;

/*==========================
 * Configuracion para el I2C
 * ==========================*/
GPIO_Handler_t handlerI2cSDA = {0};
GPIO_Handler_t handlerI2cSCL = {0};
I2C_Handler_t handlerAccelerometer = {0};
uint8_t i2cBuffer = 0;

/*Definicion de macros*/
#define ACCEL_ADDRESS	0b1101001; //0xD2 -> Direccion del accel con Logic_1
#define ACCEL_XOUT_H	59; //0x3B
#define ACCEL_XOUT_L	60; //0x3C
#define ACCEL_YOUT_H	61; //0x3D


/*=========================
 * Cabeceras de las funciones
 * ==========================*/
void initSystem(void);

/*=====================
 * 		MAIN
 * ====================*/
int main(void){
	//Inicializamos los elementos
	initSystem();

	while(1){
	}
	return 0;
}


/*=================================================
 * Funcion para cargar la configuracion de los pines
 * ================================================*/
void initSystem(void){
	/* ----Configuracion del Blinky Pin---- */
	handlerLedOK.pGPIOx											= GPIOA;
	handlerLedOK.GPIO_PinConfig.GPIO_PinNumber					= PIN_5;
	handlerLedOK.GPIO_PinConfig.GPIO_PinMode					= GPIO_MODE_OUT;
	handlerLedOK.GPIO_PinConfig.GPIO_PinOPType					= GPIO_OTYPER_PUSHPULL;
	handlerLedOK.GPIO_PinConfig.GPIO_PinSpeed					= GPIO_OSPEED_HIGH;
	//Cargamos la configuracion
	GPIO_Config(&handlerLedOK);
	/* Configuracion del timer */
	//Seleccionar el timer que se va a usar
	handlerStateOKTimer.ptrTIMx								= TIM2;
	//Configuracion con la cual se maneja el timer
	handlerStateOKTimer.TIMx_Config.TIMx_mode				= BTIMER_MODE_UP;
	handlerStateOKTimer.TIMx_Config.TIMx_period				= 2500;
	handlerStateOKTimer.TIMx_Config.TIMx_speed				= BTIMER_SPEED_100us;
	handlerStateOKTimer.TIMx_Config.TIMx_interruptEnable	= SET;
	//Cargar la configuracion del Timer
	BasicTimer_Config(&handlerStateOKTimer);

	/*El pin UserBotton es una entrada simple que entregara la interrupcion EXTI-13
	 * Observar que el pin seleccionado es el pin-13, por lo tanto el callback que se debe
	 * configurar es el callback_extInt13()
	 * */
	handlerUserBotton.pGPIOx 									= GPIOC;
	handlerUserBotton.GPIO_PinConfig.GPIO_PinNumber				= PIN_13;
	handlerUserBotton.GPIO_PinConfig.GPIO_PinMode				= GPIO_MODE_IN;
	handlerUserBotton.GPIO_PinConfig.GPIO_PinPuPdControl		= GPIO_PUPDR_NOTHING;
	GPIO_Config(&handlerUserBotton);

	handlerUserBottonExti.pGPIOHandler 	= &handlerUserBotton;
	handlerUserBottonExti.edgeType		= EXTERNAL_INTERRUPT_RISING_EDGE;
	extInt_Config(&handlerUserBottonExti);

	/*----Configuracion de la comunicacion serial-----*/
	//Configuracion para el pin de transmicion
	handlerPinTX.pGPIOx 								= GPIOA;
	handlerPinTX.GPIO_PinConfig.GPIO_PinNumber			= PIN_2;
	handlerPinTX.GPIO_PinConfig.GPIO_PinMode			= GPIO_MODE_ALTFN;
	handlerPinTX.GPIO_PinConfig.GPIO_PinAltFunMode		= AF7;
	//Cargar la configuracion del pin
	GPIO_Config(&handlerPinTX);
	//configuracion del pin para la recepcion
	handlerPinRX.pGPIOx									= GPIOA;
	handlerPinRX.GPIO_PinConfig.GPIO_PinNumber			= PIN_3;
	handlerPinRX.GPIO_PinConfig.GPIO_PinMode			= GPIO_MODE_ALTFN;
	handlerPinRX.GPIO_PinConfig.GPIO_PinAltFunMode		= AF7;
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
	usart2Comm.USART_Config.USART_enableIntTX		= USART_TX_INTERRUP_DISABLE;
	//Cargar la configuracion del USART
	USART_Config(&usart2Comm);

	/*----Configuracion para el protocolo I2C----*/
	//Configuracion de los pines para el I2C -> SCL
	handlerI2cSCL.pGPIOx								= GPIOB;
	handlerI2cSCL.GPIO_PinConfig.GPIO_PinNumber			= PIN_8;
	handlerI2cSCL.GPIO_PinConfig.GPIO_PinMode			= GPIO_MODE_ALTFN;
	handlerI2cSCL.GPIO_PinConfig.GPIO_PinOPType			= GPIO_OTYPE_OPENDRAIN;
	handlerI2cSCL.GPIO_PinConfig.GPIO_PinPuPdControl	= GPIO_PUPDR_PULLUP;
	handlerI2cSCL.GPIO_PinConfig.GPIO_PinSpeed			= GPIO_OSPEED_FAST;
	handlerI2cSCL.GPIO_PinConfig.GPIO_PinAltFunMode		= AF4;
	GPIO_Config(&handlerI2cSCL);
	//Configuracion de los pines para el I2C -> SDA
	handlerI2cSDA.pGPIOx								= GPIOB;
	handlerI2cSDA.GPIO_PinConfig.GPIO_PinNumber			= PIN_9;
	handlerI2cSDA.GPIO_PinConfig.GPIO_PinMode			= GPIO_MODE_ALTFN;
	handlerI2cSDA.GPIO_PinConfig.GPIO_PinOPType			= GPIO_OTYPE_OPENDRAIN;
	handlerI2cSDA.GPIO_PinConfig.GPIO_PinPuPdControl	= GPIO_PUPDR_PULLUP;
	handlerI2cSDA.GPIO_PinConfig.GPIO_PinSpeed			= GPIO_OSPEED_FAST;
	handlerI2cSDA.GPIO_PinConfig.GPIO_PinAltFunMode		= AF4;
	GPIO_Config(&handlerI2cSDA);
	//Configuracion de la cominicacion I2C
	handlerAccelerometer.ptrI2Cx			= I2C1;
	handlerAccelerometer.modeI2C			= I2C_MODE_FM;
	handlerAccelerometer.slaveAddress		= ACCEL_ADDRESS;
	i2c_Config(&handlerAccelerometer);
}


