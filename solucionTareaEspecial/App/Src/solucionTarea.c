/*
 * solucionTarea.c
 *
 *  Created on: May 20, 2023
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

/*----Definicion de variables----*/

/*==========================
 *Configuracion del BlinkyPin
 *==========================*/
GPIO_Handler_t BlinkyPin = {0};
BasicTimer_Handler_t handlerTimerBlinkyPin = {0};

/*========================
 * Configuracion del PWM
 * ======================*/
GPIO_Handler_t Pwm1 = {0};
GPIO_Handler_t Pwm2 = {0};
GPIO_Handler_t Pwm3 = {0};
PWM_Handler_t handlerPwm1 = {0};

/*==============================
 * Configuracion del USART
 *===============================*/
/*USART2*/
GPIO_Handler_t handlerPinTX = {0};
GPIO_Handler_t handlerPinRX = {0};
USART_Handler_t handlerCommTerminal = {0};

/*USART6*/
GPIO_Handler_t handlerPinUSART6_TX = {0};
GPIO_Handler_t handlerPinUSART6_RX = {0};
USART_Handler_t handlerUSART6 = {0};

uint8_t rxData = 0;
char bufferData[24] = "accel MPU-6050 testing..";

/*==========================
 * Configuracion del SysTick
 * =========================*/
uint32_t systemTicks = 0;
uint32_t systemTicksStart = 0;
uint32_t systemTicksEnd = 0;

/*==========================
 * Configuracion para el I2C
 * ==========================*/
//Acelerometro
GPIO_Handler_t SDAAccel = {0};
GPIO_Handler_t SCLAccel = {0};
I2C_Handler_t handlerAccelerometer = {0};
uint8_t i2cBuffer = 0;
//Display LCD
GPIO_Handler_t SDADisplayLcd = {0};
GPIO_Handler_t SCLDisplayLcd = {0};
I2C_Handler_t handlerDisplayLcd = {0};

/*Definicion de macros de las posiciones de memoria*/
#define ACCEL_ADDRESS	0b1101001; //0xD2 -> Direccion del accel con Logic_1
#define ACCEL_XOUT_H	59 //0x3B
#define ACCEL_XOUT_L	60 //0x3C
#define ACCEL_YOUT_H	61 //0x3D
#define ACCEL_YOUT_L	62 //0x3E
#define ACCEL_ZOUT_H	63 //0x3F
#define ACCEL_ZOUT_L	64 //0x40

#define PWR_MGMT_1		107 //Registros del power_manager_1, mantiene el equipo en reset
#define WHO_AM_I		117 //Registros del who_am_i el cual es del MPU-6050 y es 0x68 (identificar que es el sensor)

//#define LCD_ADDRESS

/*============================
 * Configuracion del PLL
 * ===========================*/
PLL_Config_t handlerPLL = {0}; //Se va a guardar la configuracion del PLL
GPIO_Handler_t pinVerificacionPLL = {0};
uint16_t freqMCU = 0; //Variable para guardar la configuracion del MCU

uint16_t var = 0;


/*=========================
 * Cabeceras de las funciones
 * ==========================*/
void initSystem(void);

/*=====================
 * 		MAIN
 * ====================*/
int main(void){
	//Llamammos la funcion para inicializar el MPU
	initSystem();

	//Imprimir un mensaje de inicio
//	writeMsg(&handlerUSART6, bufferData);

	/* Main Loop*/
	while(1){
//		if(rxData != '\0'){
//			if(rxData == 'h'){
//				//Prueba con el USART6
//				writeMsgTX(&handlerUSART6, "Hola mundo cruel\n");
//				rxData = '\0';
//			}
//		}

		//Hacemos un "eco" con el valor que nos llega por el serial
		if(rxData != '\0'){
			writeChar(&handlerCommTerminal, rxData);

			if(rxData == 'w'){
				sprintf(bufferData, "WHO_AM_I? (r)\n");
				writeMsgTX(&handlerCommTerminal, bufferData);

				i2cBuffer = i2c_readSingleRegister(&handlerAccelerometer, WHO_AM_I);
				sprintf(bufferData, "DataRead = 0x%x \n", (unsigned int) i2cBuffer);
				writeMsgTX(&handlerCommTerminal, bufferData);
				rxData = '\0';
			}
			else if(rxData == 'p'){
				sprintf(bufferData, "PWR_MGMT_1 stat (r)\n");
				writeMsgTX(&handlerCommTerminal, bufferData);

				i2cBuffer = i2c_readSingleRegister(&handlerAccelerometer, PWR_MGMT_1);
				sprintf(bufferData, "DataRead = 0x%x \n", (unsigned int) i2cBuffer);
				writeMsgTX(&handlerCommTerminal, bufferData);
				rxData = '\0';
			}
			else if(rxData == 'r'){
				sprintf(bufferData, "PWR_MGMT_1 reset (w)\n");
				writeMsgTX(&handlerCommTerminal, bufferData);

				i2c_writeSingleRegister(&handlerAccelerometer, PWR_MGMT_1, 0x00);
				rxData = '\0';
			}
			else if(rxData == 'r'){
				sprintf(bufferData, "PWR_MGMT_1 reset (w)\n");
				writeMsgTX(&handlerCommTerminal, bufferData);

				i2c_writeSingleRegister(&handlerAccelerometer, PWR_MGMT_1, 0x00);
				rxData = '\0';
			}
			else if(rxData == 'x'){
				sprintf(bufferData, "Axis X data (r)\n");
				writeMsgTX(&handlerCommTerminal, bufferData);

				uint8_t AccelX_low = i2c_readSingleRegister(&handlerAccelerometer, ACCEL_XOUT_L);
				uint8_t AccelX_high = i2c_readSingleRegister(&handlerAccelerometer, ACCEL_XOUT_H);
				int16_t AccelX = AccelX_high << 8 | AccelX_low;
				sprintf(bufferData, "AccelX = %d \n", (int) AccelX);
				writeMsgTX(&handlerCommTerminal, bufferData);
				rxData = '\0';
			}
			else if(rxData == 'y'){
				sprintf(bufferData, "Axis Y data (r)\n");
				writeMsgTX(&handlerCommTerminal, bufferData);

				uint8_t AccelY_low = i2c_readSingleRegister(&handlerAccelerometer, ACCEL_YOUT_L);
				uint8_t AccelY_high = i2c_readSingleRegister(&handlerAccelerometer, ACCEL_YOUT_H);
				int16_t AccelY = AccelY_high << 8 | AccelY_low;
				sprintf(bufferData, "AccelY = %d \n", (int) AccelY);
				writeMsgTX(&handlerCommTerminal, bufferData);
				rxData = '\0';
			}
			else if(rxData == 'z'){
				sprintf(bufferData, "Axis Z data (r)\n");
				writeMsgTX(&handlerCommTerminal, bufferData);

				uint8_t AccelZ_low = i2c_readSingleRegister(&handlerAccelerometer, ACCEL_ZOUT_L);
				uint8_t AccelZ_high = i2c_readSingleRegister(&handlerAccelerometer, ACCEL_ZOUT_H);
				int16_t AccelZ = AccelZ_high << 8 | AccelZ_low;
				sprintf(bufferData, "AccelZ = %d \n", (int) AccelZ);
				writeMsgTX(&handlerCommTerminal, bufferData);
				rxData = '\0';
			}
			else if(rxData == 'c'){
				freqMCU = getConfigPLL();
				sprintf(bufferData, "MCU Freq = %u \n", freqMCU);
				writeMsgTX(&handlerCommTerminal, bufferData);
				rxData = '\0';
			}
			else{
				rxData = '\0';
			}
		}
	}
	return 0;
}


/*=================================================
 * Funcion para cargar la configuracion de los pines
 * ================================================*/
void initSystem(void){
	/* ----Configuracion del Blinky Pin---- */
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
//	handlerTimerBlinkyPin.TIMx_Config.TIMx_speed				= BTIMER_SPEED_100us_80MHz;
	handlerTimerBlinkyPin.TIMx_Config.TIMx_interruptEnable		= SET;
	//Cargar la configuracion del Timer
	BasicTimer_Config(&handlerTimerBlinkyPin);

	/*----Configuracion de la comunicacion serial-----*/
	/*======
	 *USART2
	 *======*/
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
	handlerCommTerminal.ptrUSARTx 								= USART2;
	handlerCommTerminal.USART_Config.USART_baudrate 			= USART_BAUDRATE_115200;
	handlerCommTerminal.USART_Config.USART_datasize				= USART_DATASIZE_8BIT;
	handlerCommTerminal.USART_Config.USART_parity				= USART_PARITY_NONE;
	handlerCommTerminal.USART_Config.USART_stopbits				= USART_STOPBIT_1;
	handlerCommTerminal.USART_Config.USART_mode					= USART_MODE_RXTX;
	handlerCommTerminal.USART_Config.USART_enableIntRX			= USART_RX_INTERRUP_ENABLE;
	handlerCommTerminal.USART_Config.USART_enableIntTX			= USART_TX_INTERRUP_ENABLE;
	handlerCommTerminal.USART_Config.USART_PLL_Enable			= PLL_DISABLE;
	//Cargar la configuracion del USART
	USART_Config(&handlerCommTerminal);

	/*=====
	 *USART6
	 *=====*/
//	//Configuracion para el pin de transmicion
//	handlerPinUSART6_TX.pGPIOx 									= GPIOA;
//	handlerPinUSART6_TX.GPIO_PinConfig.GPIO_PinNumber			= PIN_11;
//	handlerPinUSART6_TX.GPIO_PinConfig.GPIO_PinMode				= GPIO_MODE_ALTFN;
//	handlerPinUSART6_TX.GPIO_PinConfig.GPIO_PinOPType			= GPIO_OTYPER_PUSHPULL;
//	handlerPinUSART6_TX.GPIO_PinConfig.GPIO_PinPuPdControl		= GPIO_PUPDR_NOTHING;
//	handlerPinUSART6_TX.GPIO_PinConfig.GPIO_PinSpeed			= GPIO_OSPEED_FAST;
//	handlerPinUSART6_TX.GPIO_PinConfig.GPIO_PinAltFunMode		= AF8;
//	//Cargar la configuracion del pin
//	GPIO_Config(&handlerPinUSART6_TX);
//	//configuracion del pin para la recepcion
//	handlerPinUSART6_RX.pGPIOx									= GPIOA;
//	handlerPinUSART6_RX.GPIO_PinConfig.GPIO_PinNumber			= PIN_12;
//	handlerPinUSART6_RX.GPIO_PinConfig.GPIO_PinMode				= GPIO_MODE_ALTFN;
//	handlerPinUSART6_RX.GPIO_PinConfig.GPIO_PinOPType			= GPIO_OTYPER_PUSHPULL;
//	handlerPinUSART6_RX.GPIO_PinConfig.GPIO_PinPuPdControl		= GPIO_PUPDR_NOTHING;
//	handlerPinUSART6_RX.GPIO_PinConfig.GPIO_PinSpeed			= GPIO_OSPEED_FAST;
//	handlerPinUSART6_RX.GPIO_PinConfig.GPIO_PinAltFunMode		= AF8;
//	//Cargar la configuracion del pin
//	GPIO_Config(&handlerPinUSART6_RX);
//	//Configuracion del USART
//	handlerUSART6.ptrUSARTx 									= USART6;
//	handlerUSART6.USART_Config.USART_baudrate 					= USART_BAUDRATE_115200;
//	handlerUSART6.USART_Config.USART_datasize					= USART_DATASIZE_8BIT;
//	handlerUSART6.USART_Config.USART_parity						= USART_PARITY_NONE;
//	handlerUSART6.USART_Config.USART_stopbits					= USART_STOPBIT_1;
//	handlerUSART6.USART_Config.USART_mode						= USART_MODE_RXTX;
//	handlerUSART6.USART_Config.USART_enableIntRX				= USART_RX_INTERRUP_ENABLE;
//	handlerUSART6.USART_Config.USART_enableIntTX				= USART_TX_INTERRUP_ENABLE;
//	handlerUSART6.USART_Config.USART_PLL_Enable					= PLL_ENABLE;
//	//Cargar la configuracion del USART
//	USART_Config(&handlerUSART6);

	/*----Configuracion para el protocolo I2C para el Acelerometro----*/
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
	i2c_Config(&handlerAccelerometer);

	/*----Configuracion para el protocolo I2C para el display LCD----*/
	//Configuracion de los pines para el I2C -> SCL
	SCLDisplayLcd.pGPIOx										= GPIOB;
	SCLDisplayLcd.GPIO_PinConfig.GPIO_PinNumber					= PIN_10;
	SCLDisplayLcd.GPIO_PinConfig.GPIO_PinMode					= GPIO_MODE_ALTFN;
	SCLDisplayLcd.GPIO_PinConfig.GPIO_PinOPType					= GPIO_OTYPER_OPENDRAIN;
	SCLDisplayLcd.GPIO_PinConfig.GPIO_PinPuPdControl			= GPIO_PUPDR_NOTHING;
	SCLDisplayLcd.GPIO_PinConfig.GPIO_PinSpeed					= GPIO_OSPEED_FAST;
	SCLDisplayLcd.GPIO_PinConfig.GPIO_PinAltFunMode				= AF4;
	GPIO_Config(&SCLDisplayLcd);
	//Configuracion de los pines para el I2C -> SDA
	SDADisplayLcd.pGPIOx										= GPIOB;
	SDADisplayLcd.GPIO_PinConfig.GPIO_PinNumber					= PIN_11;
	SDADisplayLcd.GPIO_PinConfig.GPIO_PinMode					= GPIO_MODE_ALTFN;
	SDADisplayLcd.GPIO_PinConfig.GPIO_PinOPType					= GPIO_OTYPER_OPENDRAIN;
	SDADisplayLcd.GPIO_PinConfig.GPIO_PinPuPdControl			= GPIO_PUPDR_NOTHING;
	SDADisplayLcd.GPIO_PinConfig.GPIO_PinSpeed					= GPIO_OSPEED_FAST;
	SDADisplayLcd.GPIO_PinConfig.GPIO_PinAltFunMode				= AF4;
	GPIO_Config(&SDADisplayLcd);
	//Configuracion de la cominicacion I2C
	handlerDisplayLcd.ptrI2Cx									= I2C2;
	handlerDisplayLcd.modeI2C									= I2C_MODE_FM;
	i2c_Config(&handlerDisplayLcd);

	/*----Configuracion del PLL----*/
//	//Se configura la velocidad del MCU para que este a 80 MHz
//	handlerPLL.PLL_PLLM											= 10;
//	handlerPLL.PLL_PLLN											= 100;
//	handlerPLL.PLL_PLLP											= PLLP_2;
//	handlerPLL.PLL_MCO1PRE										= PLL_MCO1PRE_4;
//	//Cargar la configuracion del PLL
//	ConfigPLL(&handlerPLL);

	/*====Verificacion del PLL====*/
	//Pin para verificar que si se configuro correctamente el PLL
//	pinVerificacionPLL.pGPIOx									= GPIOA;
//	pinVerificacionPLL.GPIO_PinConfig.GPIO_PinNumber			= PIN_8;
//	pinVerificacionPLL.GPIO_PinConfig.GPIO_PinMode				= GPIO_MODE_ALTFN;
//	pinVerificacionPLL.GPIO_PinConfig.GPIO_PinOPType			= GPIO_OTYPER_OPENDRAIN;
//	pinVerificacionPLL.GPIO_PinConfig.GPIO_PinPuPdControl		= GPIO_PUPDR_NOTHING;
//	pinVerificacionPLL.GPIO_PinConfig.GPIO_PinSpeed				= GPIO_OSPEED_FAST;
//	pinVerificacionPLL.GPIO_PinConfig.GPIO_PinAltFunMode		= AF0;
//	GPIO_Config(&pinVerificacionPLL);
}

/*Funcion del timer*/
void BasicTimer2_Callback(void){
	//Hacemos el blinky, para indicar que el equipo esta funcionando correctamente.
	GPIO_TooglePin(&BlinkyPin);
}

/*Funcion callback del usart*/
void usart2Rx_Callback(void){
	//Leemos el valor del registro DR, donde se almacena el dato que llega.
	//Esto ademas debe bajar la bandera de la interrupcion
	rxData = getRxData();
}


