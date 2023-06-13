/*
 * solucionExamen_v2.c
 *
 *  Created on: Jun 13, 2023
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
#include "RTCDriver.h"
#include "SysTickDriver.h"
#include "USARTxDriver.h"

#include "arm_math.h"

/*==========================
 *  		Macros
 *==========================*/
#define BUFFER_SIZE 			25
#define ADC_CHANNEL_NUMBER		2
#define NUMBER_DATA_ADC 		512
#define NUMBER_DATA_FFT 		512


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
 *   Handler para el I2C
 * ==========================*/
//Acelerometro -> Pines de salida del I2C
GPIO_Handler_t SDAAccel = {0};
GPIO_Handler_t SCLAccel = {0};
//Handler para la configuracion del I2C
I2C_Handler_t handlerAccelerometer = {0};
//Buffer para el I2C
uint8_t i2cBuffer = 0;
//Handler del timer para el muestreo del acelerometro
BasicTimer_Handler_t handlerTimerAccel = {0};
//Bandera para comenzar el muestreo del Accel
uint8_t startAccelMuestreo = 0;

/*Arreglo para guardar la infomracion que se obtiene del acelerometro, como la frecuencia de muestreo va a ser de 1 KHz, y la toma de datos se realiza en un 1 Hz,
 * el tamaño de cada uno de los arreglos debe de ser de 2000 datos, cada uno de 16 bits.*/
float arregloEjeX[NUMBER_DATA_FFT];
float arregloEjeY[NUMBER_DATA_FFT];
float arregloEjeZ[NUMBER_DATA_FFT];
//Variables para guardar el muestreo de cada uno de los ejes
float muestreoEjeX = 0;
float muestreoEjeY = 0;
float muestreoEjeZ = 0;
//Variable para comenzar la captura de datos en dos segundos
uint8_t capturarDatos_accel = 0;
//Variable para indicar que el muiestreo ya se realizo
uint8_t muestreoListo_accel = 3;

/*==========================
 *   Handler del MCO1
 *==========================*/
//Pin de salida para el MCO1
GPIO_Handler_t Mco1Pin = {0};
//Pin para la salida del MCO2
GPIO_Handler_t Mco2Pin = {0};

/*==========================
 *    Handler del PLL
 *==========================*/
//Handler para la configuracion del PLL
PLL_Config_t handlerPLL = {0};

/*==========================
 *   Handler del ADC
 *==========================*/
//Handler para la configuracion del ADC
ADC_Config_t handlerAdc = {0};
//Pin para la salida de la señal del PWM utilizada para la activacion del ADC
GPIO_Handler_t handlerPinPwmAdc = {0};
//Handler para la configuracion del PWM
PWM_Handler_t handlerPwmAdc = {0};
//Variables para guardar los canales que se van a utilizar y el samplin period
uint8_t adcChannel[ADC_CHANNEL_NUMBER] = {ADC_CHANNEL_10, ADC_CHANNEL_11};
uint16_t periodoSampling[ADC_CHANNEL_NUMBER] = {ADC_SAMPLING_PERIOD_28_CYCLES, ADC_SAMPLING_PERIOD_28_CYCLES};
//Variables para utilizar el ADC
uint8_t ADCISCOMPLETE = 0; //Bandera para saber cuando se completo la conversionHola
uint16_t adcData = 0; //Variables para guardar el dato de la conversion adc
uint16_t datosAdc[NUMBER_DATA_ADC] = {0};
//Variable para saber como se va a configurar el trigger
uint8_t triggerAdc = ADC_TIMER_TRIGGER_RISING;
//Bandera para indicar la captura de datos
uint8_t capDataAdc = 0;
//Variable para contar la cantidad de datos mostrados por el ADC
uint16_t contadorAdc = 0;
/*Variables para realizar la conversion a voltaje*/
float valorChannel1 = 0;
float valorChannel2 = 0;
//Contador para ir guardando los datos en los arreglos
uint16_t i_accel = 0;
uint8_t banderaListoAdc = 2;

/*==========================
 *   Handler del RTC
 *==========================*/
RTC_Config_t handlerRTC = {0};

/*============================================
 *   Elementos para la transformada de Fourier
 *=============================================*/
uint32_t ifftFlag = 0;
uint32_t doBitReverse = 1;
arm_rfft_fast_instance_f32 config_Rfft_fast_f32;
arm_cfft_radix4_instance_f32 configRadix4_f32;
arm_status status = ARM_MATH_ARGUMENT_ERROR;
arm_status statusInitFFT = ARM_MATH_ARGUMENT_ERROR;
uint16_t fftSize = 1024;
float32_t transformedSignal[NUMBER_DATA_FFT];

/*=========================
 * Cabeceras de las funciones
 * ==========================*/
void initSystem(void);
//Funcion encargada de analizar los comandos que envia el usuario
void parseCommands(char *prtBufferReception);
//Funcion encargada para relalizar el muestreo del accel
void muestreoAccel(void);
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

char bufferData[BUFFER_SIZE + 24] = "Exam Solution...\n";
char mensajeError[100] = "Please enter a correct value, try again.\n";


int main(void){
	/*Activamos la FPU*/
	SCB->CPACR |= (0xF << 20);
	//Cargamos la configuracion de los pines
	initSystem();
	//Mensaje de que esta funcionando coreectamente el USART
	writeMsg(&handlerTerminal, bufferData);

	while(1){
		//Una vez que se termine de capturar los datos del ADC se imprime mensaje y se para la señal PWM
		if(ADCISCOMPLETE == 1){
			ADCISCOMPLETE = 0;
			writeMsg(&handlerTerminal, "Capture Finished!!!\n");
			stopPwmSignal(&handlerPwmAdc);
		}
		//Captualos los comandos ingrasados por el usuario
		cmdUsuario();
		//Realizamos el muestreo del acelerometro
		muestreoAccel();
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

	/*=====================================
	 * Configuracion del pin para el MCO1
	 *====================================*/
	Mco1Pin.pGPIOx												= GPIOA;
	Mco1Pin.GPIO_PinConfig.GPIO_PinNumber						= PIN_8;
	Mco1Pin.GPIO_PinConfig.GPIO_PinMode							= GPIO_MODE_ALTFN;
	Mco1Pin.GPIO_PinConfig.GPIO_PinOPType						= GPIO_OTYPER_PUSHPULL;
	Mco1Pin.GPIO_PinConfig.GPIO_PinPuPdControl					= GPIO_PUPDR_NOTHING;
	Mco1Pin.GPIO_PinConfig.GPIO_PinSpeed						= GPIO_OSPEED_FAST;
	Mco1Pin.GPIO_PinConfig.GPIO_PinAltFunMode					= AF0;
	//Cargamos la configuracion
	GPIO_Config(&Mco1Pin);
	ConfigHSI(&handlerPLL);

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
	handlerTimerBlinkyPin.TIMx_Config.TIMx_speed				= BTIMER_SPEED_100us_100MHz;
	handlerTimerBlinkyPin.TIMx_Config.TIMx_interruptEnable		= SET;
	//Cargar la configuracion del Timer
	BasicTimer_Config(&handlerTimerBlinkyPin);

	/*========================================
	 * Configuracion de la comunicacion serial
	 *========================================*/
	//Configuracion para el pin de transmicion
	handlerPinTX.pGPIOx 										= GPIOC;
	handlerPinTX.GPIO_PinConfig.GPIO_PinNumber					= PIN_6;
	handlerPinTX.GPIO_PinConfig.GPIO_PinMode					= GPIO_MODE_ALTFN;
	handlerPinTX.GPIO_PinConfig.GPIO_PinOPType					= GPIO_OTYPER_PUSHPULL;
	handlerPinTX.GPIO_PinConfig.GPIO_PinPuPdControl				= GPIO_PUPDR_NOTHING;
	handlerPinTX.GPIO_PinConfig.GPIO_PinSpeed					= GPIO_OSPEED_FAST;
	handlerPinTX.GPIO_PinConfig.GPIO_PinAltFunMode				= AF8;
	//Cargar la configuracion del pin
	GPIO_Config(&handlerPinTX);
	//configuracion del pin para la recepcion
	handlerPinRX.pGPIOx											= GPIOC;
	handlerPinRX.GPIO_PinConfig.GPIO_PinNumber					= PIN_7;
	handlerPinRX.GPIO_PinConfig.GPIO_PinMode					= GPIO_MODE_ALTFN;
	handlerPinRX.GPIO_PinConfig.GPIO_PinOPType					= GPIO_OTYPER_PUSHPULL;
	handlerPinRX.GPIO_PinConfig.GPIO_PinPuPdControl				= GPIO_PUPDR_NOTHING;
	handlerPinRX.GPIO_PinConfig.GPIO_PinSpeed					= GPIO_OSPEED_FAST;
	handlerPinRX.GPIO_PinConfig.GPIO_PinAltFunMode				= AF8;
	//Cargar la configuracion del pin
	GPIO_Config(&handlerPinRX);
	//Configuracion del USART
	handlerTerminal.ptrUSARTx 									= USART6;
	handlerTerminal.USART_Config.USART_baudrate 				= USART_BAUDRATE_115200;
	handlerTerminal.USART_Config.USART_datasize					= USART_DATASIZE_8BIT;
	handlerTerminal.USART_Config.USART_parity					= USART_PARITY_NONE;
	handlerTerminal.USART_Config.USART_stopbits					= USART_STOPBIT_1;
	handlerTerminal.USART_Config.USART_mode						= USART_MODE_RXTX;
	handlerTerminal.USART_Config.USART_enableIntRX				= USART_RX_INTERRUP_ENABLE;
	handlerTerminal.USART_Config.USART_enableIntTX				= USART_TX_INTERRUP_DISABLE;
	handlerTerminal.USART_Config.USART_PLL_Enable				= PLL_ENABLE;
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
	handlerAccelerometer.I2C_MAIN_CLOCK							= MAIN_CLOCK_16_MHz_FOR_I2C;
	i2c_Config(&handlerAccelerometer);
	//Configuracion del timer para realizar el muestreo a 200Hz
	//Seleccionar el timer que se va a usar
	handlerTimerAccel.ptrTIMx									= TIM3;
	//Configuracion con la cual se maneja el timer
	handlerTimerAccel.TIMx_Config.TIMx_mode						= BTIMER_MODE_UP;
	handlerTimerAccel.TIMx_Config.TIMx_period					= 500;
	handlerTimerAccel.TIMx_Config.TIMx_speed					= BTIMER_SPEED_10us_100MHz;
	handlerTimerAccel.TIMx_Config.TIMx_interruptEnable			= SET;
	//Cargar la configuracion del Timer
	BasicTimer_Config(&handlerTimerAccel);

	/*=====================================
	 * 	   Configuracion para el ADC
	 *====================================*/
	/*Cionfiguracion del pin para ver PWM*/
	handlerPinPwmAdc.pGPIOx 									= GPIOA;
	handlerPinPwmAdc.GPIO_PinConfig.GPIO_PinNumber				= PIN_1;
	handlerPinPwmAdc.GPIO_PinConfig.GPIO_PinMode				= GPIO_MODE_ALTFN;
	handlerPinPwmAdc.GPIO_PinConfig.GPIO_PinOPType				= GPIO_OTYPER_PUSHPULL;
	handlerPinPwmAdc.GPIO_PinConfig.GPIO_PinPuPdControl			= GPIO_PUPDR_NOTHING;
	handlerPinPwmAdc.GPIO_PinConfig.GPIO_PinSpeed				= GPIO_OSPEED_FAST;
	handlerPinPwmAdc.GPIO_PinConfig.GPIO_PinAltFunMode			= AF1;
	//Cargamos la configuracion
	GPIO_Config(&handlerPinPwmAdc);

	//Configuramos el PWM para realizar la velocidad del muestreo del ADC
	//Configuramos el Timer para la señal PWM
	handlerPwmAdc.ptrTIMx										= TIM2;
	handlerPwmAdc.config.channel								= PWM_CHANNEL_2;
	handlerPwmAdc.config.duttyCicle								= 15;
	handlerPwmAdc.config.periodo								= 50;
	handlerPwmAdc.config.prescaler								= 100;
	//Cargamos la configuracion
	pwm_Config(&handlerPwmAdc);

	//Configuracion del handler del ADC
	handlerAdc.channel											= adcChannel;
	handlerAdc.dataAlignment									= ADC_ALIGNMENT_RIGHT;
	handlerAdc.samplingPeriod									= periodoSampling;
	handlerAdc.resolution										= ADC_RESOLUTION_12_BIT;
	ADC_ConfigMultichannel(&handlerAdc, ADC_CHANNEL_NUMBER);
	//Activamos la activacion del ADC con un timer
	extiTimerADC(&handlerPwmAdc, ADC_TIMER_TRIGGER_RISING);

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
		writeMsg(&handlerTerminal, "1) premco1 #prescales -- premco1 cmd, #prescales is a uint8_t (the value of the prescaler)\n");
		writeMsg(&handlerTerminal, "----------If #prescales = 0, MCO1 non division\n");
		writeMsg(&handlerTerminal, "----------If #prescales = 2, MCO1 division by 2\n");
		writeMsg(&handlerTerminal, "----------If #prescales = 3, MCO1 division by 3\n");
		writeMsg(&handlerTerminal, "----------If #prescales = 4, MCO1 division by 4\n");
		writeMsg(&handlerTerminal, "----------If #prescales = 5, MCO1 division by 5\n");
		writeMsg(&handlerTerminal, "2) clockmco1 #clock -- clockmco1 cmd, #clock is a uint8_t (clock you want to use)\n");
		writeMsg(&handlerTerminal, "----------If #clock = 0, MCO1 clock HSI\n");
		writeMsg(&handlerTerminal, "----------If #clock = 1, MCO1 clock LSE\n");
		writeMsg(&handlerTerminal, "----------If #clock = 2, MCO1 clock PLL\n");
		writeMsg(&handlerTerminal, "3) initHour #HOUR #MINUTS #24H-12H-- initHour cmd, #HOUR is a uint8_t, #MINUTS is a uint8_t, #24H-12H is a char (initialized the RTC, configure hour)\n");
		writeMsg(&handlerTerminal, "----------To enter the type of format you want, use #24H-12H\n");
		writeMsg(&handlerTerminal, "----------The allowed values ​​are:\n");
		writeMsg(&handlerTerminal, "---------------------24_H -> For a 24 hour format\n");
		writeMsg(&handlerTerminal, "---------------------AM -> For a 12 hour format and it's on AM\n");
		writeMsg(&handlerTerminal, "---------------------PM -> For a 12 hour format and it's on PM\n");
		writeMsg(&handlerTerminal, "4) seeHour -- seeHour cmd (this cmd allow you to see the hour)\n");
		writeMsg(&handlerTerminal, "5) initDate #DAY #MOUNTH #W #YEAR -- initDate cmd, #DAY and #MOUNTH are uint8_t, #W is a char, #YEAR is uint16_t (initialized the RTC, configure date)\n");
		writeMsg(&handlerTerminal, "6) seeDate -- seeDate cmd (this cmd allow you to see the date)\n");
		writeMsg(&handlerTerminal, "7) capDataAdc #freq -- capDataAdc cmd, #freq is a uint16_t (this cmd is to capture the data from the ADC and with what freq)\n");
		writeMsg(&handlerTerminal, "8) showDataAdc #channel -- showDataAdc cmd, #channel is a uint8_t (show the data and which channel)\n");
		writeMsg(&handlerTerminal, "----------If #channel = 1, show the data from channel #1\n");
		writeMsg(&handlerTerminal, "----------If #channel = 2, show the data from channel #2\n");
		writeMsg(&handlerTerminal, "9) capAcel -- capAcel cmd (capture accel data)\n");
		writeMsg(&handlerTerminal, "10) fftAcel -- fftAcel cmd (fft of accel data)\n");
		writeMsg(&handlerTerminal, "11) dummy #A #B -- dummy cmd, #A and #B are uint32_t\n");
		writeMsg(&handlerTerminal, "12) usermsg #msg -- usermsg cmd, #msg is a char(msg to show)\n");

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
	/*=======================
	 * Comandos para el MCO1
	 * ======================*/
	//Comando para elegir el prescaler del MCO1
	else if(strcmp(cmd, "premco1") == 0){
		writeMsg(&handlerTerminal, "CMD: premco1\n");
		//Limpiamos el registro del MCO1PRE
		RCC->CFGR &= ~RCC_CFGR_MCO1PRE;
		switch(firstParameter){
		case 0:
			RCC->CFGR &= ~RCC_CFGR_MCO1PRE;
			writeMsg(&handlerTerminal, "MCO1 non division\n");
			break;
		case 2:
			RCC->CFGR |= 0b100 << RCC_CFGR_MCO1PRE_Pos;
			writeMsg(&handlerTerminal, "MCO1 division by 2\n");
			break;
		case 3:
			RCC->CFGR |= 0b101 << RCC_CFGR_MCO1PRE_Pos;
			writeMsg(&handlerTerminal, "MCO1 division by 3\n");
			break;
		case 4:
			RCC->CFGR |= 0b110 << RCC_CFGR_MCO1PRE_Pos;
			writeMsg(&handlerTerminal, "MCO1 division by 4\n");
			break;
		case 5:
			RCC->CFGR |= 0b111 << RCC_CFGR_MCO1PRE_Pos;
			writeMsg(&handlerTerminal, "MCO1 division by 5\n");
			break;
		default:
			writeMsg(&handlerTerminal, mensajeError);
			break;
		}
	}
	//Comando para elegir el reloj en el MCO1
	else if(strcmp(cmd, "clockmco1") == 0){
		writeMsg(&handlerTerminal, "CMD: clockmco1\n");
		switch(firstParameter){
		case 0:
			RCC->CFGR &= ~RCC_CFGR_MCO1;
			writeMsg(&handlerTerminal, "MCO1 clock HSI\n");
			break;
		case 1:
			ConfigLSE();
			writeMsg(&handlerTerminal, "MCO1 clock LSE\n");
			break;
		case 2:
			RCC->CFGR &= ~RCC_CFGR_MCO1;
			RCC->CFGR |= 0b11 << RCC_CFGR_MCO1_Pos;
			writeMsg(&handlerTerminal, "MCO1 clock PLL\n");
			break;
		default:
			writeMsg(&handlerTerminal, mensajeError);
			break;
		}
	}
	/*=====================
	 * Comandos para el RTC
	 * ====================*/
	//Comando para guardar las horas en el RTC
	else if(strcmp(cmd, "initHour") == 0){
		writeMsg(&handlerTerminal, "CMD: initHour\n");
		//Se guardan las horas ingresadas por el usuario
		if((firstParameter <= 24) && (firstParameter > 0)){
			handlerRTC.RTC_HOURS = firstParameter;
		}else{
			writeMsg(&handlerTerminal, mensajeError);
		}
		//Se guardan los minutos ingresados por el usiario
		if((secondParameter > 0) && (secondParameter <= 60)){
			handlerRTC.RTC_MINUTS = secondParameter;
		}else{
			writeMsg(&handlerTerminal, mensajeError);
		}
		//Se guarda la el formato de las horas si se quiere 24 horas o 12 horas
		if(strcmp(userMsg, "24_H") == 0){
			handlerRTC.FORMAT_NOTATION = MODE_24H;
			ConfigRTC(&handlerRTC);
			writeMsg(&handlerTerminal, "Your time has been loaded\n");
		}else if(strcmp(userMsg, "AM") == 0){
			handlerRTC.FORMAT_NOTATION = MODE_12H;
			handlerRTC.FORMAT_AM_PM = MODE_AM;
			ConfigRTC(&handlerRTC);
			writeMsg(&handlerTerminal, "Your time has been loaded\n");
		}else if(strcmp(userMsg, "PM") == 0){
			handlerRTC.FORMAT_NOTATION = MODE_12H;
			handlerRTC.FORMAT_AM_PM = MODE_PM;
			ConfigRTC(&handlerRTC);
			writeMsg(&handlerTerminal, "Your time has been loaded\n");
		}else{
			writeMsg(&handlerTerminal, mensajeError);
		}

	}
	//Comando para visualizar las horas guardadas en el RTC
	else if(strcmp(cmd, "seeHour") == 0){
		writeMsg(&handlerTerminal, "CMD: seeHour\n");
		//Creamos una variable para guardar el valor de las horas
		uint8_t valorSegundosUnidades = 0;
		uint8_t valorSegundosDecenas = 0;
		uint8_t valorMinutosUnidades = 0;
		uint8_t valorMinutosDecenas = 0;
		uint8_t valorHorasUnidades = 0;
		uint8_t valorHorasDecenas = 0;
		uint8_t valorAM_PM = 0;
		uint8_t rectificar = 0;

		//Guardamos el valor del tiempo en las variables
		valorSegundosUnidades = read_time()[0];
		valorSegundosDecenas = read_time()[1];
		valorMinutosUnidades = read_time()[2];
		valorMinutosDecenas = read_time()[3];
		valorHorasUnidades = read_time()[4];
		valorHorasDecenas = read_time()[5];
		valorAM_PM = read_time()[6];
		rectificar = (valorHorasDecenas * 10) + valorHorasUnidades;

		if(valorAM_PM == 1){
			sprintf(bufferData, "Hour = %u%u:%u%u:%u%u PM\n", valorHorasDecenas, valorHorasUnidades, valorMinutosDecenas, valorMinutosUnidades, valorSegundosDecenas, valorSegundosUnidades);
			writeMsg(&handlerTerminal, bufferData);
		}else if((valorAM_PM == 0) && (rectificar <= 12)){
			sprintf(bufferData, "Hour = %u%u:%u%u:%u%u AM\n", valorHorasDecenas, valorHorasUnidades, valorMinutosDecenas, valorMinutosUnidades, valorSegundosDecenas, valorSegundosUnidades);
			writeMsg(&handlerTerminal, bufferData);
		}else{
			sprintf(bufferData, "Hour = %u%u:%u%u:%u%u \n", valorHorasDecenas, valorHorasUnidades, valorMinutosDecenas, valorMinutosUnidades, valorSegundosDecenas, valorSegundosUnidades);
			writeMsg(&handlerTerminal, bufferData);
		}
	}
	//Comando para guardar la fecha en el RTC
	else if(strcmp(cmd, "initDate") == 0){
		writeMsg(&handlerTerminal, "CMD: initDate\n");
		//Se guardan las horas ingresadas por el usuario
		if((firstParameter > 0) && (firstParameter <= 31)){
			handlerRTC.RTC_DAYS = firstParameter;
		}else{
			writeMsg(&handlerTerminal, mensajeError);
		}
		//Se guardan los minutos ingresados por el usiario
		if((secondParameter > 0 ) && (secondParameter <= 12)){
			handlerRTC.RTC_MONTH = secondParameter;
		}else{
			writeMsg(&handlerTerminal, mensajeError);
		}
		//Se guarda la el formato de las horas si se quiere 24 horas o 12 horas
		handlerRTC.RTC_YEARS = thirdParameter;
		if(strcmp(userMsg, "l") == 0){
			handlerRTC.RTC_TEXT_WEEK = 0b001;
		}else if(strcmp(userMsg, "m") == 0){
			handlerRTC.RTC_TEXT_WEEK = 0b010;
		}else if(strcmp(userMsg, "w") == 0){
			handlerRTC.RTC_TEXT_WEEK = 0b011;
		}else if(strcmp(userMsg, "j") == 0){
			handlerRTC.RTC_TEXT_WEEK = 0b100;
		}else if(strcmp(userMsg, "v") == 0){
			handlerRTC.RTC_TEXT_WEEK = 0b101;
		}else if(strcmp(userMsg, "s") == 0){
			handlerRTC.RTC_TEXT_WEEK = 0b110;
		}else if(strcmp(userMsg, "d") == 0){
			handlerRTC.RTC_TEXT_WEEK = 0b111;
		}else{
			handlerRTC.RTC_TEXT_WEEK = 0b001;
		}
		ConfigRTC(&handlerRTC);
		writeMsg(&handlerTerminal, "Your date has been loaded\n");
	}
	//Comando para visualizar la fecha en el RTC
	else if(strcmp(cmd, "seeDate") == 0){
		writeMsg(&handlerTerminal, "CMD: seeDate\n");
		//Creamos una variable para guardar el valor de las horas
		uint8_t valorDiasUnidades = 0;
		uint8_t valorDiasDecenas = 0;
		uint8_t valorMes = 0;
		uint8_t valorAnoUnidades = 0;
		uint8_t valorAnoDecenas = 0;
		uint16_t valorAnos = 0;
		uint8_t valorTextoSemana1 = 0;

		//Guardamos el valor del tiempo en las variables
		valorDiasUnidades = read_date()[0];
		valorDiasDecenas = read_date()[1];
		valorMes = read_date()[2];
		valorAnoUnidades = read_date()[3];
		valorAnoDecenas = read_date()[4];
		valorAnos = 2000 + (valorAnoDecenas * 10) + valorAnoUnidades;
		//Guardamos el valor del dia de la semana
		valorTextoSemana1 = (RTC->DR & RTC_DR_WDU_Msk) >> RTC_DR_WDU_Pos;

		sprintf(bufferData, "Date = %u%u/%u/%hd\n", valorDiasDecenas, valorDiasUnidades, valorMes, valorAnos);
		writeMsg(&handlerTerminal, bufferData);
		switch(valorTextoSemana1){
		case 1:
			writeMsg(&handlerTerminal, "Monday\n");
			break;
		case 2:
			writeMsg(&handlerTerminal, "Tuesday\n");
			break;
		case 3:
			writeMsg(&handlerTerminal, "Wednesday\n");
			break;
		case 4:
			writeMsg(&handlerTerminal, "Thursday\n");
			break;
		case 5:
			writeMsg(&handlerTerminal, "Friday\n");
			break;
		case 6:
			writeMsg(&handlerTerminal, "Saturday\n");
			break;
		case 7:
			writeMsg(&handlerTerminal, "Sunday\n");
			break;
		}
	}
	/*=====================
	 * Comandos para el ADC
	 * ====================*/
	//Comando para capturar los datos del ADC
	else if(strcmp(cmd, "capDataAdc") == 0){
		/*Con un perido de 50 se obtiene una señal de 19.38 KHz o 51.6 micro-seg*/
		writeMsg(&handlerTerminal, "CMD: capDataAdc\n");
		if((firstParameter < 1501) && (firstParameter > 799)){
			contadorAdc = 0;
			capDataAdc = 1;
			banderaListoAdc = 0;
			/*Activamos la señal*/
			enableOutput(&handlerPwmAdc);
			startPwmSignal(&handlerPwmAdc);
			uint16_t newFreq = ((100000/(firstParameter)) * 50) / 51;
			updateFrequency(&handlerPwmAdc, newFreq);
		}else{
			writeMsg(&handlerTerminal, "¡¡¡Error!!!\n");
			writeMsg(&handlerTerminal, "The introduce frequency is not valid\n");
		}
	}
	//Comando para visualizar los datos del ADC
	else if(strcmp(cmd, "showDataAdc") == 0){
		writeMsg(&handlerTerminal, "CMD: showDataAdc\n");
		if(firstParameter == 1){
			for(uint16_t i = 0; i < 257; i++){
				valorChannel1 = (datosAdc[(2*i)] * 3.3) / 4096.;
				sprintf(bufferData, "Data Canal 1: %#.2f\n", valorChannel1);
				writeMsg(&handlerTerminal, bufferData);
			}
		}else if(firstParameter == 2){
			for(uint16_t j = 0; j < 257; j++){
				valorChannel2 = (datosAdc[(2*j + 1)] * 3.3) / 4096.;
				sprintf(bufferData, "Data Canal 2: %#.2f\n", valorChannel2);
				writeMsg(&handlerTerminal, bufferData);
			}
		}else{
			writeMsg(&handlerTerminal, mensajeError);
		}
	}
	/*======================
	 * Comandos para el Acel
	 * ====================*/
	else if(strcmp(cmd, "capAcel") == 0){
		writeMsg(&handlerTerminal, "CMD: capaccel\n");
		//Ver si esta conectado el Accel
		sprintf(bufferData, "WHO_AM_I? (r)\n");
		writeMsg(&handlerTerminal, bufferData);

		i2cBuffer = i2c_readSingleRegister(&handlerAccelerometer, WHO_AM_I);
		sprintf(bufferData, "DataRead = 0x%x \n", (unsigned int) i2cBuffer);
		i2c_writeSingleRegister(&handlerAccelerometer, PWR_MGMT_1, 0x00);
		i2cBuffer = i2c_readSingleRegister(&handlerAccelerometer, PWR_MGMT_1);

		capturarDatos_accel = 1;
		muestreoListo_accel = 0;
		i_accel = 0;
		writeMsg(&handlerTerminal, "Starting Capture!!!\n");
	}
	else if(strcmp(cmd, "fftAcel") == 0){
		int i = 0;
		int j = 0;

		writeMsg(&handlerTerminal, "CMD: fftAcel\n");

		statusInitFFT = arm_rfft_fast_init_f32(&config_Rfft_fast_f32, fftSize);
		if(statusInitFFT == ARM_MATH_SUCCESS){
			sprintf(bufferData, "Initialization...SUCCESS!\n");
			writeMsg(&handlerTerminal, bufferData);
		}
		if(statusInitFFT == ARM_MATH_SUCCESS){
			arm_rfft_fast_f32(&config_Rfft_fast_f32, arregloEjeY, transformedSignal, ifftFlag);

			arm_abs_f32(transformedSignal, arregloEjeY, fftSize);

			for(i = 1; i < fftSize; i++){
				if(i % 2){
					sprintf(bufferData, "%u ; %#.6f\n", j, arregloEjeY[i]);
					writeMsg(&handlerTerminal, bufferData);
					j++;
				}
			}
			writeMsg(&handlerTerminal, "FFT Finished!!!");
		}
		else{
			writeMsg(&handlerTerminal, "FFT not Initialized...");
		}
	}
	else{
		writeMsg(&handlerTerminal, mensajeError);
	}
	//Limpiamos los registros
	firstParameter = 0;
	secondParameter = 0;
}

//Funcion para realizar el muestreo del accel
void muestreoAccel(void){
	if(startAccelMuestreo == 1){
		/*Se espera hasta que el usuario de el comando para poder comenzar la captura de los datos del
		 * acelerometro y guardarlos en los arreglos correspondientes a cada uno de los ejes.*/
		if(capturarDatos_accel == 1){
			uint8_t AccelX_low = i2c_readSingleRegister(&handlerAccelerometer, ACCEL_XOUT_L);
			uint8_t AccelX_high = i2c_readSingleRegister(&handlerAccelerometer, ACCEL_XOUT_H);
			muestreoEjeX =  ((AccelX_high << 8 | AccelX_low)/51200.f) * 9.78 - 3.24;
			uint8_t AccelY_low = i2c_readSingleRegister(&handlerAccelerometer, ACCEL_YOUT_L);
			uint8_t AccelY_high = i2c_readSingleRegister(&handlerAccelerometer, ACCEL_YOUT_H);
			muestreoEjeY = ((AccelY_high << 8 | AccelY_low)/51200.f) * 9.78 - 3.24;
			uint8_t AccelZ_low = i2c_readSingleRegister(&handlerAccelerometer, ACCEL_ZOUT_L);
			uint8_t AccelZ_high = i2c_readSingleRegister(&handlerAccelerometer, ACCEL_ZOUT_H);
			muestreoEjeZ = ((AccelZ_high << 8 | AccelZ_low)/51200.f) * 9.78 - 3.24;
			startAccelMuestreo = 0;
			/*Como la variable tiempo se encuentra en el Callback del timer y este incrementa cada 250 ms,
			 * se espera a que este llegue a 8, con lo que han pasado los dos segundos de captura.*/
			//Con este if se va a comenzar la captura de los datos del acelerometro durante dos segundos
			if(i_accel < NUMBER_DATA_FFT){
				arregloEjeX[i_accel] = muestreoEjeX;
				arregloEjeY[i_accel] = muestreoEjeY;
				arregloEjeZ[i_accel] = muestreoEjeZ;
				//Se aumenta la posicion en el arreglo para guardar el dato en dicha posicion
				i_accel++;
			}else{
				/*Una vez completada la captira de los datos se baja la bandera y se levanta la bandera de que
				*  el muestreo esta completo*/
				capturarDatos_accel = 0;
				muestreoListo_accel = 1;
			}

		}
	}
	//Condicional para verificar si el muestreo se realizo
	if(muestreoListo_accel == 1){
		writeMsgTX(&handlerTerminal, "Capture Finished!!!\n");
		muestreoListo_accel = 0;
	}
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
/*Funcion callback del Timer 5*/
void BasicTimer3_Callback(void){
	//Se cambia la bandera cada vez que
	startAccelMuestreo = 1;
}
/*Funcion callback del usart*/
void usart6Rx_Callback(void){
	//Almacenamos el dato que se envio  en la variable rxData
	rxData = getRxData();
}
/*Funcion callback del ADC*/
void adcComplete_Callback(void){
	//Se almacenan los datos en un arreglo
	datosAdc[contadorAdc] = getADC();
	//Una vez se llegue al tamaño del arreglo se para de realizar el muestreo ADC
	if(contadorAdc < NUMBER_DATA_ADC){
		contadorAdc++;
	}else{
		contadorAdc = 0;
		ADCISCOMPLETE = 1;
	}
}



