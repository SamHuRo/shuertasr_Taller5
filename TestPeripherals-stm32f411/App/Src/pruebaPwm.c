/*
 * pruebaPwm.c
 *
 *  Created on: May 12, 2023
 *      Author: samuel
 */

#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>

#include <stm32f4xx.h>

#include "GPIOxDriver.h"
#include "ExtiDriver.h"
#include "BasicTimer.h"
#include "USARTxDriver.h"
#include "SysTickDriver.h"
#include "PwmDriver.h"

/*----Funcion para cargar iniciar el MCU----*/
void initSystem(void);

/*----Definicion de los handlers----*/
GPIO_Handler_t BlinkyPin = {0};      //BlinkyPin (pin de estado)

GPIO_Handler_t handlerUserBotton = {0};
EXTI_Config_t handlerUserBottonExti = {0};

BasicTimer_Handler_t handlerTimerBlinkyPin = {0};  //Definicion del handler para el timer del blinkypin o pin de estado

/*Elementos para la comunicacion serial*/
GPIO_Handler_t handlerPinTX = {0};
GPIO_Handler_t handlerPinRX = {0};
USART_Handler_t usart2Comm = {0};
uint8_t sendMsg = 0;
uint8_t usart2DataReceived = 0;

/*Elementos para el PWM*/
GPIO_Handler_t handlerPinPwmChannel = {0};
PWM_Handler_t handlerSignalPWM = {0};
uint16_t duttyValue = 1500;

char bufferMsg[64] = {0};

/*Funcion main*/
int main(void){
	/*Activamos la FPU*/
	SCB->CPACR |= (0xF << 20);

	//Inicializamos los elementos
	initSystem();

	while(1){
		//Verificando el PWM
		if(usart2DataReceived != '\0'){
			if(usart2DataReceived == 'd'){
				//Bajar
				duttyValue -= 10;
				updateDuttyCycle(&handlerSignalPWM, duttyValue);
			}
			//Para probar el seno
			if(usart2DataReceived == 'u'){
				//Subir
				duttyValue += 100;
				updateDuttyCycle(&handlerSignalPWM, duttyValue);
			}
			//Imprimimos el mensaje
			sprintf(bufferMsg, "Dutty = %u \n", (unsigned int)duttyValue);
			writeMsg(&usart2Comm, bufferMsg);
			//Cambiamos el valor de control de entrada
			usart2DataReceived = '\0';
		}
	}
	return 0;
}


void initSystem(void){
	/* ----Configuracion del Blinky Pin---- */
	BlinkyPin.pGPIOx										= GPIOA;
	BlinkyPin.GPIO_PinConfig.GPIO_PinNumber					= PIN_5;
	BlinkyPin.GPIO_PinConfig.GPIO_PinMode					= GPIO_MODE_OUT;
	BlinkyPin.GPIO_PinConfig.GPIO_PinOPType					= GPIO_OTYPER_PUSHPULL;
	BlinkyPin.GPIO_PinConfig.GPIO_PinSpeed					= GPIO_OSPEED_HIGH;

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

	/*Configuracion de la comunicacion serial*/
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

	/*Cionfiguracion del PWM*/
	handlerPinPwmChannel.pGPIOx 							= GPIOC;
	handlerPinPwmChannel.GPIO_PinConfig.GPIO_PinNumber		= PIN_7;
	handlerPinPwmChannel.GPIO_PinConfig.GPIO_PinMode		= GPIO_MODE_ALTFN;
	handlerPinPwmChannel.GPIO_PinConfig.GPIO_PinOPType		= GPIO_OTYPER_PUSHPULL;
	handlerPinPwmChannel.GPIO_PinConfig.GPIO_PinPuPdControl	= GPIO_PUPDR_NOTHING;
	handlerPinPwmChannel.GPIO_PinConfig.GPIO_PinSpeed		= GPIO_OSPEED_FAST;
	handlerPinPwmChannel.GPIO_PinConfig.GPIO_PinAltFunMode	= AF2;
	//Cargamos la configuracion
	GPIO_Config(&handlerPinPwmChannel);

	//Configuramos el Timer para la señal PWM
	handlerSignalPWM.ptrTIMx								= TIM3;
	handlerSignalPWM.config.channel							= PWM_CHANNEL_2;
	handlerSignalPWM.config.duttyCicle						= duttyValue;
	handlerSignalPWM.config.periodo							= 20000;
	handlerSignalPWM.config.prescaler						= 16;
	//Cargamos la configuracion
	pwm_Config(&handlerSignalPWM);

	/*Activamos la señal*/
	enableOutput(&handlerSignalPWM);
	startPwmSignal(&handlerSignalPWM);
}
/*Callback del timer2 -> Hace blinky al led*/
void BasicTimer2_Callback(void){
	GPIO_TooglePin(&BlinkyPin);
	sendMsg++;
}
/*Callback del user botton -> No hace nada*/
void callback_extInt13(void){
	__NOP();
}
/*Esta funcion se ejecuta cada vez que un caracter es recibido
 * por el puerto USART2*/
void usart2Rx_Callback(void){
	/*Verificacion del tiempo que se demora la interrupcion*/
	usart2DataReceived = getRxData();
}

