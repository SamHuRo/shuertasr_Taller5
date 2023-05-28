/*
 * solucionTarea.c
 *
 *  Created on: May 20, 2023
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
#include "DisplayLCDDriver.h"
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

/*========================
 * Configuracion del PWM
 * ======================*/
//Pines
GPIO_Handler_t Pwm1 = {0};
GPIO_Handler_t Pwm2 = {0};
GPIO_Handler_t Pwm3 = {0};
//Handler del PWM
PWM_Handler_t handlerPwm1 = {0};
PWM_Handler_t handlerPwm2 = {0};
PWM_Handler_t handlerPwm3 = {0};
//Dutty cicle
uint16_t duttyValue = 0;

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
char bufferData[100] = "Solucion Tarea...\n";

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
BasicTimer_Handler_t timerAccel = {0};

//Display LCD
GPIO_Handler_t SDADisplayLcd = {0};
GPIO_Handler_t SCLDisplayLcd = {0};
I2C_Handler_t handlerDisplayLcd = {0};

//#define LCD_ADDRESS

/*============================
 * Configuracion del PLL
 * ===========================*/
PLL_Config_t handlerPLL = {0}; //Se va a guardar la configuracion del PLL
GPIO_Handler_t pinVerificacionPLL = {0};
uint16_t freqMCU = 0; //Variable para guardar la configuracion del MCU

uint16_t var = 0;

/*===============================
 *         Variables
 *===============================*/
/*Arreglo para guardar la infomracion que se obtiene del acelerometro, como la frecuencia de muestreo va a ser de 1 KHz, y la toma de datos se realiza en un 1 Hz,
 * el tamaño de cada uno de los arreglos debe de ser de 2000 datos, cada uno de 16 bits.*/
float arregloEjeX[2000];
float arregloEjeY[2000];
float arregloEjeZ[2000];
//Variable para empezar a realizar el muestreo
uint8_t startMuestreo = 0;
//Variable para comenzar la captura de datos en dos segundos
uint8_t capturarDatos = 0;
//Variable para indicar que el muiestreo ya se realizo
uint8_t muestreoListo = 3;
//Contador, esta variable nos va a ayudar a contar los dos segundos que se necesitan para la captura de los datos
uint32_t tiempo = 0;
//Contador para ir guardando los datos en los arreglos
uint16_t i = 0;
//Mensaje para mostrar que la captura esta lista
char mensaje[50] = "Listo captura";
//Varaible que nos ayuda a almacenar los datos
uint16_t j = 0;
//Bandera para saber cuando se ha enviado un mensaje
uint8_t mensajeEnviado = 0;


/*=========================
 * Cabeceras de las funciones
 * ==========================*/
void initSystem(void);
void muestreoAccel(void);
void tecladoAccel(void);

/*=====================
 * 		MAIN
 * ====================*/
int main(void){
	/*Activar el co-prcesador matematico o unidad de punto flotante*/
	SCB->CPACR |= (0xF << 20);
	//Llamammos la funcion para inicializar el MPU
	initSystem();
	//Primer mensaje de que esta funcionando
	writeMsgTX(&handlerUSART6, bufferData);
	/* Main Loop*/
	while(1){
		//Esta es la funcion en la cual se esta la comunicacion serial con el PC
		tecladoAccel();
		//Funcion para realizar el muestreo del acelerometro a 1 KHz
		muestreoAccel();
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
	handlerTimerBlinkyPin.TIMx_Config.TIMx_speed				= BTIMER_SPEED_100us_80MHz;
	handlerTimerBlinkyPin.TIMx_Config.TIMx_interruptEnable		= SET;
	//Cargar la configuracion del Timer
	BasicTimer_Config(&handlerTimerBlinkyPin);

	/*----Configuracion de la comunicacion serial-----*/
	/*==============================
	 *			USART6
	 *==============================*/
	//Configuracion para el pin de transmicion
	handlerPinUSART6_TX.pGPIOx 									= GPIOC;
	handlerPinUSART6_TX.GPIO_PinConfig.GPIO_PinNumber			= PIN_6;
	handlerPinUSART6_TX.GPIO_PinConfig.GPIO_PinMode				= GPIO_MODE_ALTFN;
	handlerPinUSART6_TX.GPIO_PinConfig.GPIO_PinOPType			= GPIO_OTYPER_PUSHPULL;
	handlerPinUSART6_TX.GPIO_PinConfig.GPIO_PinPuPdControl		= GPIO_PUPDR_NOTHING;
	handlerPinUSART6_TX.GPIO_PinConfig.GPIO_PinSpeed			= GPIO_OSPEED_FAST;
	handlerPinUSART6_TX.GPIO_PinConfig.GPIO_PinAltFunMode		= AF8;
	//Cargar la configuracion del pin
	GPIO_Config(&handlerPinUSART6_TX);
	//configuracion del pin para la recepcion
	handlerPinUSART6_RX.pGPIOx									= GPIOC;
	handlerPinUSART6_RX.GPIO_PinConfig.GPIO_PinNumber			= PIN_7;
	handlerPinUSART6_RX.GPIO_PinConfig.GPIO_PinMode				= GPIO_MODE_ALTFN;
	handlerPinUSART6_RX.GPIO_PinConfig.GPIO_PinOPType			= GPIO_OTYPER_PUSHPULL;
	handlerPinUSART6_RX.GPIO_PinConfig.GPIO_PinPuPdControl		= GPIO_PUPDR_NOTHING;
	handlerPinUSART6_RX.GPIO_PinConfig.GPIO_PinSpeed			= GPIO_OSPEED_FAST;
	handlerPinUSART6_RX.GPIO_PinConfig.GPIO_PinAltFunMode		= AF8;
	//Cargar la configuracion del pin
	GPIO_Config(&handlerPinUSART6_RX);
	//Configuracion del USART
	handlerUSART6.ptrUSARTx 									= USART6;
	handlerUSART6.USART_Config.USART_baudrate 					= USART_BAUDRATE_115200;
	handlerUSART6.USART_Config.USART_datasize					= USART_DATASIZE_8BIT;
	handlerUSART6.USART_Config.USART_parity						= USART_PARITY_NONE;
	handlerUSART6.USART_Config.USART_stopbits					= USART_STOPBIT_1;
	handlerUSART6.USART_Config.USART_mode						= USART_MODE_RXTX;
	handlerUSART6.USART_Config.USART_enableIntRX				= USART_RX_INTERRUP_ENABLE;
	handlerUSART6.USART_Config.USART_enableIntTX				= USART_TX_INTERRUP_ENABLE;
	handlerUSART6.USART_Config.USART_PLL_Enable					= PLL_ENABLE;
	//Cargar la configuracion del USART
	USART_Config(&handlerUSART6);

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
	handlerAccelerometer.PLL_ON									= PLL_ENABLE;
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
	handlerDisplayLcd.slaveAddress								= ADDRES_LCD;
	handlerDisplayLcd.PLL_ON									= PLL_ENABLE;
	i2c_Config(&handlerDisplayLcd);

	/*----Configuracion del PLL----*/
	//Se configura la velocidad del MCU para que este a 80 MHz
	handlerPLL.PLL_PLLM											= 10;
	handlerPLL.PLL_PLLN											= 100;
	handlerPLL.PLL_PLLP											= PLLP_2;
	handlerPLL.PLL_MCO1PRE										= PLL_MCO1PRE_4;
	//Cargar la configuracion del PLL
	ConfigPLL(&handlerPLL);

	/*----Configuracion del timer para el muestreo del Accel----*/
	timerAccel.ptrTIMx											= TIM5;
	timerAccel.TIMx_Config.TIMx_mode							= BTIMER_MODE_UP;
	timerAccel.TIMx_Config.TIMx_period							= 10;
	timerAccel.TIMx_Config.TIMx_speed							= BTIMER_SPEED_100us_80MHz;
	timerAccel.TIMx_Config.TIMx_interruptEnable					= SET;
	BasicTimer_Config(&timerAccel);

	/*----Configuracion del PWM----*/
	//Primer pwm
	Pwm1.pGPIOx 												= GPIOB;
	Pwm1.GPIO_PinConfig.GPIO_PinNumber							= PIN_1;
	Pwm1.GPIO_PinConfig.GPIO_PinMode							= GPIO_MODE_ALTFN;
	Pwm1.GPIO_PinConfig.GPIO_PinOPType							= GPIO_OTYPER_PUSHPULL;
	Pwm1.GPIO_PinConfig.GPIO_PinPuPdControl						= GPIO_PUPDR_NOTHING;
	Pwm1.GPIO_PinConfig.GPIO_PinSpeed							= GPIO_OSPEED_FAST;
	Pwm1.GPIO_PinConfig.GPIO_PinAltFunMode						= AF2;
	//Cargamos la configuracion
	GPIO_Config(&Pwm1);
	//Configuramos el Timer para la señal PWM
	handlerPwm1.ptrTIMx											= TIM3;
	handlerPwm1.config.channel									= PWM_CHANNEL_4;
	handlerPwm1.config.duttyCicle								= duttyValue;
	handlerPwm1.config.periodo									= 20000;
	handlerPwm1.config.prescaler								= 16;
	//Cargamos la configuracion
	pwm_Config(&handlerPwm1);
	//Activamos la señal
	enableOutput(&handlerPwm1);
	startPwmSignal(&handlerPwm1);

	//Segundo pwm
	Pwm2.pGPIOx 												= GPIOB;
	Pwm2.GPIO_PinConfig.GPIO_PinNumber							= PIN_4;
	Pwm2.GPIO_PinConfig.GPIO_PinMode							= GPIO_MODE_ALTFN;
	Pwm2.GPIO_PinConfig.GPIO_PinOPType							= GPIO_OTYPER_PUSHPULL;
	Pwm2.GPIO_PinConfig.GPIO_PinPuPdControl						= GPIO_PUPDR_NOTHING;
	Pwm2.GPIO_PinConfig.GPIO_PinSpeed							= GPIO_OSPEED_FAST;
	Pwm2.GPIO_PinConfig.GPIO_PinAltFunMode						= AF2;
	//Cargamos la configuracion
	GPIO_Config(&Pwm2);
	//Configuramos el Timer para la señal PWM
	handlerPwm2.ptrTIMx											= TIM3;
	handlerPwm2.config.channel									= PWM_CHANNEL_1;
	handlerPwm2.config.duttyCicle								= duttyValue;
	handlerPwm2.config.periodo									= 20000;
	handlerPwm2.config.prescaler								= 16;
	//Cargamos la configuracion
	pwm_Config(&handlerPwm2);
	//Activamos la señal
	enableOutput(&handlerPwm2);
	startPwmSignal(&handlerPwm2);

	//Tercer pwm
	Pwm3.pGPIOx 												= GPIOB;
	Pwm3.GPIO_PinConfig.GPIO_PinNumber							= PIN_5;
	Pwm3.GPIO_PinConfig.GPIO_PinMode							= GPIO_MODE_ALTFN;
	Pwm3.GPIO_PinConfig.GPIO_PinOPType							= GPIO_OTYPER_PUSHPULL;
	Pwm3.GPIO_PinConfig.GPIO_PinPuPdControl						= GPIO_PUPDR_NOTHING;
	Pwm3.GPIO_PinConfig.GPIO_PinSpeed							= GPIO_OSPEED_FAST;
	Pwm3.GPIO_PinConfig.GPIO_PinAltFunMode						= AF2;
	//Cargamos la configuracion
	GPIO_Config(&Pwm3);
	//Configuramos el Timer para la señal PWM
	handlerPwm3.ptrTIMx											= TIM3;
	handlerPwm3.config.channel									= PWM_CHANNEL_2;
	handlerPwm3.config.duttyCicle								= duttyValue;
	handlerPwm3.config.periodo									= 20000;
	handlerPwm3.config.prescaler								= 16;
	//Cargamos la configuracion
	pwm_Config(&handlerPwm3);
	//Activamos la señal
	enableOutput(&handlerPwm3);
	startPwmSignal(&handlerPwm3);
}

/*Funcion callback del timer para led de estado*/
void BasicTimer2_Callback(void){
	//Hacemos el blinky, para indicar que el equipo esta funcionando correctamente.
	GPIO_TooglePin(&BlinkyPin);
	//Voy aumentando el tiempo de la captura
	tiempo++;
}
/*Funcion callback del timer para el muestreo de los datos del acelerometro*/
void BasicTimer5_Callback(void){
	startMuestreo = 1; //Cambiamos el valor de la variable startMuestreo
}
/*Funcion callback del usart*/
void usart6Rx_Callback(void){
	//Almacenamos el dato que se envio  en la variable rxData
	rxData = getRxData();
}
void usart6Tx_Callback(void){
	mensajeEnviado ^= 1;
}
/*Funcion para realizar el muestreo*/
void muestreoAccel(void){
	if(startMuestreo == 1){
		uint8_t AccelX_low = i2c_readSingleRegister(&handlerAccelerometer, ACCEL_XOUT_L);
		uint8_t AccelX_high = i2c_readSingleRegister(&handlerAccelerometer, ACCEL_XOUT_H);
		float muestreoEjeX =  ((AccelX_high << 8 | AccelX_low)/2560000.f) * 9.78;
		uint8_t AccelY_low = i2c_readSingleRegister(&handlerAccelerometer, ACCEL_YOUT_L);
		uint8_t AccelY_high = i2c_readSingleRegister(&handlerAccelerometer, ACCEL_YOUT_H);
		float muestreoEjeY = ((AccelY_high << 8 | AccelY_low)/327680.f) * 9.78 + 8.02;
		uint8_t AccelZ_low = i2c_readSingleRegister(&handlerAccelerometer, ACCEL_ZOUT_L);
		uint8_t AccelZ_high = i2c_readSingleRegister(&handlerAccelerometer, ACCEL_ZOUT_H);
		float muestreoEjeZ = ((AccelZ_high << 8 | AccelZ_low)/3276800.f) * 9.78 ;
		startMuestreo = 0;
		/*Se espera hasta que el usuario hunda la tecla 'c' para poder comenzar la captura de los datos del
		 * acelerometro y guardarlos en los arreglos correspondientes a cada uno de los ejes.*/
		if(capturarDatos == 1){
			/*Como la variable tiempo se encuentra en el Callback del timer y este incrementa cada 250 ms,
			 * se espera a que este llegue a 8, con lo que han pasado los dos segundos de captura.*/
			if(tiempo < 9){
				//Con este if se va a comenzar la captura de los datos del acelerometro durante dos segundos
				arregloEjeX[i] = muestreoEjeX;
				arregloEjeY[i] = muestreoEjeY;
				arregloEjeZ[i] = muestreoEjeZ;
				//Se aumenta la posicion en el arreglo para guardar el dato en dicha posicion
				i++;
			}else{
				/*Una vez completada la captira de los datos se baja la bandera y se levanta la bandera de que
				 *  el muestreo esta completo*/
				muestreoListo = 1;
				capturarDatos = 0;
			}
		}
		if(tiempo > 1000){
			tiempo = 0;
		}
	}
	//Condicional para verificar si el muestreo se realizo
	if(muestreoListo == 1){
		writeMsg(&handlerUSART6, mensaje);
		muestreoListo = 0;
	}
}
/*Funcion que guarda las interacciones del acelerometro con el teclado*/
void tecladoAccel(void){
	if(rxData != '\0'){
		if(rxData == 'w'){
			sprintf(bufferData, "WHO_AM_I? (r)\n");
			writeMsgTX(&handlerUSART6, bufferData);

			i2cBuffer = i2c_readSingleRegister(&handlerAccelerometer, WHO_AM_I);
			sprintf(bufferData, "DataRead = 0x%x \n", (unsigned int) i2cBuffer);
			writeMsgTX(&handlerUSART6, bufferData);
			rxData = '\0';
		}
		else if(rxData == 'p'){
			sprintf(bufferData, "PWR_MGMT_1 stat (r)\n");
			writeMsgTX(&handlerUSART6, bufferData);

			i2cBuffer = i2c_readSingleRegister(&handlerAccelerometer, PWR_MGMT_1);
			sprintf(bufferData, "DataRead = 0x%x \n", (unsigned int) i2cBuffer);
			writeMsgTX(&handlerUSART6, bufferData);
			rxData = '\0';
		}
		else if(rxData == 'r'){
			sprintf(bufferData, "PWR_MGMT_1 reset (w)\n");
			writeMsgTX(&handlerUSART6, bufferData);

			i2c_writeSingleRegister(&handlerAccelerometer, PWR_MGMT_1, 0x00);
			rxData = '\0';
		}
		else if(rxData == 'x'){
			sprintf(bufferData, "Axis X data (r)\n");
			writeMsgTX(&handlerUSART6, bufferData);

			uint8_t AccelX_low = i2c_readSingleRegister(&handlerAccelerometer, ACCEL_XOUT_L);
			uint8_t AccelX_high = i2c_readSingleRegister(&handlerAccelerometer, ACCEL_XOUT_H);
			int16_t AccelX = AccelX_high << 8 | AccelX_low;
			float AccelX_Value = (AccelX/2560000.f) * 9.78;
			sprintf(bufferData, "AccelX = %.2f m/s2\n", AccelX_Value);
			writeMsgTX(&handlerUSART6, bufferData);
			rxData = '\0';
		}
		else if(rxData == 'y'){
			sprintf(bufferData, "Axis Y data (r)\n");
			writeMsgTX(&handlerUSART6, bufferData);

			uint8_t AccelY_low = i2c_readSingleRegister(&handlerAccelerometer, ACCEL_YOUT_L);
			uint8_t AccelY_high = i2c_readSingleRegister(&handlerAccelerometer, ACCEL_YOUT_H);
			int16_t AccelY = AccelY_high << 8 | AccelY_low;
			float Accely_Value = (AccelY/2560.f) * 9.78 + 12.01;
			sprintf(bufferData, "AccelY = %.2f m/s2\n", Accely_Value);
			writeMsgTX(&handlerUSART6, bufferData);
			rxData = '\0';
		}
		else if(rxData == 'z'){
			sprintf(bufferData, "Axis Z data (r)\n");
			writeMsgTX(&handlerUSART6, bufferData);

			uint8_t AccelZ_low = i2c_readSingleRegister(&handlerAccelerometer, ACCEL_ZOUT_L);
			uint8_t AccelZ_high = i2c_readSingleRegister(&handlerAccelerometer, ACCEL_ZOUT_H);
			int16_t AccelZ = AccelZ_high << 8 | AccelZ_low;
			float AccelZ_Value = (AccelZ/2560.f) * 9.78 + 14.45;
			sprintf(bufferData, "AccelZ = %.2f m/s2\n", AccelZ_Value);
			writeMsgTX(&handlerUSART6, bufferData);
			rxData = '\0';
		}
		else if(rxData == 'q'){
			freqMCU = getConfigPLL();
			sprintf(bufferData, "Freq MCU = %i MHz \n", (unsigned int) freqMCU);
			writeMsgTX(&handlerUSART6, bufferData);
			rxData = '\0';
		}
		else if(rxData == 'c'){
			capturarDatos = 1;
			muestreoListo = 0;
			tiempo = 0;
			i = 0;
			rxData = '\0';
		}else if(rxData == 'd'){
			j = 0;
			while(j < 2001){
				sprintf(bufferData, "(%.2f; %.2f; %.2f) m/s2 # %i \n", arregloEjeX[j], arregloEjeY[j], arregloEjeZ[j], j);
				writeMsg(&handlerUSART6, bufferData);
				j++;
			}
			rxData = '\0';
		}
		else{
			rxData = '\0';
		}
	}
}
