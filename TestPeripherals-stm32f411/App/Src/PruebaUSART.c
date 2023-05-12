/*
 * PruebaUSART.c
 *
 *  Created on: May 12, 2023
 *      Author: samuel
 */

#include <math.h>
#include <stm32f4xx.h>
#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include "GPIOxDriver.h"
#include "ExtiDriver.h"
#include "BasicTimer.h"
#include "USARTxDriver.h"
#include "SysTickDriver.h"

#define HSI_CLOCK_CONFIGURED	0 //16MHz
#define HSE_CLOCK_CONFIGURED	1
#define PLL_CLOCK_CONFIGURED	2

/*Funcion para cargar iniciar el MCU*/
void initSystem(void);

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
GPIO_Handler_t handlerUserBotton = {0};
BasicTimer_Handler_t handlerTimerBlinkyPin = {0};  //Definicion del handler para el timer del blinkypin o pin de estado
EXTI_Config_t handlerUserBottonExti = {0};

/*Variables de punto flotante*/
float valorA = 123.4567f;
float valorB = 98.8764f;
float valorC = 0.0f;

int main(void)
{

	//Se llama a la funcion initSystem para iniciar el sistema
	initSystem();
	//Configuracion del SysTick a 16MHz
	config_SysTick_ms(HSI_CLOCK_CONFIGURED);

    /* Loop forever */
	while(1){
		/*Activar el co-prcesador matematico o unidad de punto flotante*/
		SCB->CPACR |= (0xF << 20);

		/*Primer ejemplo del usart
		 * if(printMsg > 4){
			writeChar(&usart2Comm, 'H');
			writeChar(&usart2Comm, 'O');
			writeChar(&usart2Comm, 'L');
			writeChar(&usart2Comm, 'A');
			writeChar(&usart2Comm, ' ');
			writeChar(&usart2Comm, 'M');
			writeChar(&usart2Comm, 'U');
			writeChar(&usart2Comm, 'N');
			writeChar(&usart2Comm, 'D');
			writeChar(&usart2Comm, 'O');
			printMsg = 0;
		}
		*/
		/*Segundo ejemplo del usart*/
		//if(sendMsg > 4){
			//writeChar(&usart2Comm, 'G');

			//Primera opcion
			//writeMsg(&usart2Comm, mensaje);
			//Segunda opcion
			//writeMsg(&usart2Comm, &mensaje[0]);
			//Funcion sprintf(char *str, const char *format, variables.....)
			//sprintf(bufferMsg, "Valor de sendMsg = %d \n", sendMsg);
			//sprintf(bufferMsg, "Valor de PI = %#.3f, sendMsg = %d", M_PI, sendMsg);
			//writeMsg(&usart2Comm, bufferMsg);
			//sendMsg = 0;
		//}
		/*Se va a actuvar cuando se detecte la interrupcion, ya que una interrupcion no se puede demorar mas de 1ms, poniendo esta opcion en el main,
		 * el tiempo de la interrupcion se demora 5 micro-segundos*/
		//if(usart2DataReceived != '\0'){
			//sprintf(bufferMsg, "Recibido char = %c \n", usart2DataReceived);
			//writeMsg(&usart2Comm, bufferMsg);
			/*Se vuelve a poner la variable en nulo para que no se ejecute mas de una vez*/
			//usart2DataReceived = '\0';
		//}
		/*Verificacion del punto flotant*/
		//if(usart2DataReceived != '\0'){
			//valorC = valorA * valorB;
			//sprintf(bufferMsg, "El valorC es igual a: %#.3f \n", valorC);
			//writeMsg(&usart2Comm, bufferMsg);
			//usart2DataReceived = '\0';
		//}
		/*Verificacion del SysTick*/
		if(sendMsg > 4){
			writeMsg(&usart2Comm, "Hola mundo!");

			GPIO_TooglePin(&BlinkyPin);
			delay_ms(30);
			GPIO_TooglePin(&BlinkyPin);
			delay_ms(30);
			GPIO_TooglePin(&BlinkyPin);
			delay_ms(30);
			GPIO_TooglePin(&BlinkyPin);
			delay_ms(30);

			GPIO_TooglePin(&BlinkyPin);
			delay_ms(250);
			GPIO_TooglePin(&BlinkyPin);
			delay_ms(250);
			GPIO_TooglePin(&BlinkyPin);
			delay_ms(250);
			GPIO_TooglePin(&BlinkyPin);
			delay_ms(250);

			GPIO_TooglePin(&BlinkyPin);
			delay_ms(185);
			GPIO_TooglePin(&BlinkyPin);
			delay_ms(185);
			GPIO_TooglePin(&BlinkyPin);
			delay_ms(185);
			GPIO_TooglePin(&BlinkyPin);
			delay_ms(185);

			sendMsg = 0;
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
	usart2Comm.USART_Config.USART_enableIntRX		= USART_RX_INTERRUP_DISABLE;
	usart2Comm.USART_Config.USART_enableIntTX		= USART_TX_INTERRUP_DISABLE;
	//Cargar la configuracion del USART
	USART_Config(&usart2Comm);
}
/*Callback del timer2 -> Hace blinky al led*/
void BasicTimer2_Callback(void){
	//GPIO_TooglePin(&BlinkyPin);
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
	GPIO_WritePin(&BlinkyPin, SET); //Se prende el led
	usart2DataReceived = getRxData();
	GPIO_WritePin(&BlinkyPin, RESET); //Se apaga el led
}
