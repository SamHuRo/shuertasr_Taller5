/*
 * solucionTarea3.c
 *
 *  Created on: May 12, 2023
 *      Author: samuel
 */
#include <stm32f4xx.h>
#include <stdint.h>

#include "ExtiDriver.h"
#include "GPIOxDriver.h"
#include "BasicTimer.h"
/* #----Definicion de macros----# */
#define DESACTIVAR				1
#define ACTIVAR					0

/* #----Definicion de los handler----# */
GPIO_Handler_t BlinkyPin = {0};      //BlinkyPin (pin de estado)
//Display de 7 segmentos
GPIO_Handler_t handlerUnidades = {0};  //Activa las unidades en el display de 7 segmentos
GPIO_Handler_t handlerDecenas = {0};  //Activa las decenas en el display de 7 segmentos
GPIO_Handler_t handlerE = {0};  //Activa el led e
GPIO_Handler_t handlerDp = {0};	 //Activa el led dp
GPIO_Handler_t handlerC = {0};  //Activa el led c
GPIO_Handler_t handlerA = {0};  //Activa el led a
GPIO_Handler_t handlerB = {0};  //Activa el led b
GPIO_Handler_t handlerF = {0};  //Activa el led f
GPIO_Handler_t handlerG = {0}; //Activa el led g
GPIO_Handler_t handlerD = {0};  //Activa el led d
//Encoder
GPIO_Handler_t SW = {0};  //SW del Encoder
GPIO_Handler_t DT = {0};  //DT del Encoder
GPIO_Handler_t CLK = {0}; //CLK del Encoder


/* #----Definicion del Timer----# */
BasicTimer_Handler_t handlerTimerBlinkyPin = {0};  //Definicion del handler para el timer del blinkypin o pin de estado
BasicTimer_Handler_t handlerTimerDisplay = {0}; //Definicion del handler para el timer del display


/* #----Definicion de las interrupciones----# */
EXTI_Config_t handlerEXTI_SW = {0}; 	//Interrupcion del SW
EXTI_Config_t handlerEXTI_CLK = {0};	//Interrupcion del DT

/* #----Cabeceras de las de funciones----# */
//Funcion para inicializar el sistema, se definen los pines, timer y las interrupciones
void initSystem(void);
//Funcion para escribir en el display
void writeDisplay(uint32_t num);
//Funcion para escribir en el display la culebrita
void DisplayCulebrita(void);
//Funcion para escribir las decenas en el display
void DisplayDecenas(void);
//Funcion para escribir las unidades en el display
void DisplayUnidades(void);

/* #----Definicion de constantes----# */
uint8_t modo = 0; //Variable del modo en el que esta el display 0 "modo contador", 1 "modo culebrita"
uint8_t giro = 0; //Variable la cual nos va a decir para que lado esta girando el encoder
uint32_t contador = 0; //Cuenta el numero que se debe de mostrar en el display
uint8_t posicion = 0; //Muestra la posicion en la que esta la culebrita
uint32_t display[10] = {0x40, 0x79, 0x24, 0x30, 0x19, 0x12, 0x02, 0x78, 0x00, 0x18}; //Lista de los numeros del 0 al 9 en hexadecimal
uint32_t culebrita[12] = {0b1111110, 0b1111110, 0b1011111, 0b1101111, 0b1110111, 0b1101111, 0b1011111, 0b1111101, 0b1111011, 0b1110111, 0b1111011, 0b1111101}; //Valores que va a tomer el display cuando este en el modo de culebrita
uint8_t estadoDisplay = 0; //Variable para guardar el esto del pin encargado de activar o desactivar los leds de las unidades
char banderaDisplay = 0; //Variable para detectar
uint8_t unidades = 0; //Variable para guardar el numero que se va a mostorar en el display, la parte de las unidades
uint8_t decenas = 0; //Variable para guardar el numero que se va a mostrar en el display, la parte de las decenas

/* #----Definicion de la funcion main----# */
int main(void){
	initSystem();

	while(1){
		//Se realiza una verificacion de que el contador no pase del 99, si ese es el caso va a regresar al 0
		if(contador > 99){
			contador = 0;
		}
		//Se realiza una verificacion de que la posicion no supere los 11, ya que el arreglo de culebrita solo tiene 12 datos
		if(posicion > 11){
			posicion = 0;
		}
		if(modo == 0){
			/*El modo se detecto como 0, por lo que va a entrar al modo contador*/
			/*Se va a realizar un if para ver hacia donde giro el encoder, si para la izquierda o derecha.
			 * la variable giro se va a poner como 1, si el giro del encoder es clockwise el valor del pin
			 * DT va a ser igual a 5, el cual va a estar almacenado en la variable valorDT. Si este es el caso el giro
			 * se realizo a la derecha, si este no es el caso el giro se realizo a la izquierda (counter clockwise)*/
			if(giro && (GPIOReadPin(&DT) == 1)){
				//Se aumenta el contador una unidad
				contador += 1;
				//Limpiamos la variable giro
				giro = 0; //Se regresa el valor del giro a 0 para volver a detectar un nuevo giro
			}else if(giro && (GPIOReadPin(&DT) == 0)){
				//Se disminuye el contador una unidad
				contador -= 1;
				//Limpiamos la variable giro
				giro = 0;
			}
			if(banderaDisplay){
				/*Se mira el estado de los pines correspondientes para las unidades y las decenas, en el caso que este en 1 el pin correspondientes a las unidades
				 * se utiliza la funcion DisplayUnidades() para copiar en el display en numero correspondiente, este mismo razonamiento se utiliza para cuando el pin
				 * correspondiente para las decenas.*/
				if((GPIOReadPin(&handlerUnidades) == 1) && (GPIOReadPin(&handlerDecenas) == 0)){
					DisplayUnidades();
				}else if((GPIOReadPin(&handlerUnidades) == 0) && (GPIOReadPin(&handlerDecenas) == 1)){
					DisplayDecenas();
				}
				banderaDisplay = 0;
			}
		}else if(modo == 1){
			/*El modo se detecta como otro valor, por lo que va a entrar al modo culebrita.
			 * Se tiene el mismo razonamiento que en el caso que el modo es igual a 0.*/
			if(giro && (GPIOReadPin(&DT) == 1)){
				//Se aumenta el contador una unidad
				posicion += 1;
				//Limpiamos la variable giro
				giro = 0;
			}else if(giro && (GPIOReadPin(&DT) == 0)){
				//Se disminuye el contador una unidad
				posicion -= 1;
				//Limpiamos la variable giro
				giro = 0;
			}
			DisplayCulebrita();
		}
	}
}


/* #----Definicion de la funcion initSystem----# */
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
	handlerTimerBlinkyPin.TIMx_Config.TIMx_period			= 250;
	handlerTimerBlinkyPin.TIMx_Config.TIMx_speed			= BTIMER_SPEED_1ms;
	handlerTimerBlinkyPin.TIMx_Config.TIMx_interruptEnable	= SET;

	//Cargar la configuracion del Timer
	BasicTimer_Config(&handlerTimerBlinkyPin);

	/* ----Configuracion del display---- */
	/* Configuracion de los pines */
	//El pin PB10 -> Unidades
	handlerUnidades.pGPIOx = GPIOB;
	handlerUnidades.GPIO_PinConfig.GPIO_PinNumber			= PIN_10;
	handlerUnidades.GPIO_PinConfig.GPIO_PinMode				= GPIO_MODE_OUT;
	handlerUnidades.GPIO_PinConfig.GPIO_PinOPType			= GPIO_OTYPER_PUSHPULL;
	handlerUnidades.GPIO_PinConfig.GPIO_PinPuPdControl		= GPIO_PUPDR_NOTHING;
	handlerUnidades.GPIO_PinConfig.GPIO_PinSpeed			= GPIO_OSPEED_HIGH;
	handlerUnidades.GPIO_PinConfig.GPIO_PinAltFunMode		= AF0;

	//Cargamos la configuración al pin especifico
	GPIO_Config(&handlerUnidades);
	GPIO_WritePin(&handlerUnidades, 1);

	//El pin PB4 -> Decenas
	handlerDecenas.pGPIOx = GPIOB;
	handlerDecenas.GPIO_PinConfig.GPIO_PinNumber			= PIN_4;
	handlerDecenas.GPIO_PinConfig.GPIO_PinMode				= GPIO_MODE_OUT;
	handlerDecenas.GPIO_PinConfig.GPIO_PinOPType			= GPIO_OTYPER_PUSHPULL;
	handlerDecenas.GPIO_PinConfig.GPIO_PinPuPdControl		= GPIO_PUPDR_NOTHING;
	handlerDecenas.GPIO_PinConfig.GPIO_PinSpeed				= GPIO_OSPEED_HIGH;
	handlerDecenas.GPIO_PinConfig.GPIO_PinAltFunMode		= AF0;

	//Cargamos la configuración al pin especifico
	GPIO_Config(&handlerDecenas);

	//El pin PB15 -> d
	handlerD.pGPIOx = GPIOB;
	handlerD.GPIO_PinConfig.GPIO_PinNumber				= PIN_15;
	handlerD.GPIO_PinConfig.GPIO_PinMode				= GPIO_MODE_OUT;
	handlerD.GPIO_PinConfig.GPIO_PinOPType				= GPIO_OTYPER_PUSHPULL;
	handlerD.GPIO_PinConfig.GPIO_PinPuPdControl			= GPIO_PUPDR_NOTHING;
	handlerD.GPIO_PinConfig.GPIO_PinSpeed				= GPIO_OSPEED_HIGH;
	handlerD.GPIO_PinConfig.GPIO_PinAltFunMode			= AF0;

	//Cargamos la configuración al pin especifico
	GPIO_Config(&handlerD);

	//El pin PC8 -> e
	handlerE.pGPIOx = GPIOC;
	handlerE.GPIO_PinConfig.GPIO_PinNumber				= PIN_8;
	handlerE.GPIO_PinConfig.GPIO_PinMode				= GPIO_MODE_OUT;
	handlerE.GPIO_PinConfig.GPIO_PinOPType				= GPIO_OTYPER_PUSHPULL;
	handlerE.GPIO_PinConfig.GPIO_PinPuPdControl			= GPIO_PUPDR_NOTHING;
	handlerE.GPIO_PinConfig.GPIO_PinSpeed				= GPIO_OSPEED_HIGH;
	handlerE.GPIO_PinConfig.GPIO_PinAltFunMode			= AF0;

	//Cargamos la configuracion el pin especifico
	GPIO_Config(&handlerE);

	//El pin PC6 -> dp
	handlerDp.pGPIOx = GPIOB;
	handlerDp.GPIO_PinConfig.GPIO_PinNumber				= PIN_6;
	handlerDp.GPIO_PinConfig.GPIO_PinMode				= GPIO_MODE_OUT;
	handlerDp.GPIO_PinConfig.GPIO_PinOPType				= GPIO_OTYPER_PUSHPULL;
	handlerDp.GPIO_PinConfig.GPIO_PinPuPdControl		= GPIO_PUPDR_NOTHING;
	handlerDp.GPIO_PinConfig.GPIO_PinSpeed				= GPIO_OSPEED_HIGH;
	handlerDp.GPIO_PinConfig.GPIO_PinAltFunMode			= AF0;

	//Cargamos la configuracion el pin especifico
	GPIO_Config(&handlerDp);

	//El pin PC5 -> c
	handlerC.pGPIOx = GPIOC;
	handlerC.GPIO_PinConfig.GPIO_PinNumber				= PIN_5;
	handlerC.GPIO_PinConfig.GPIO_PinMode				= GPIO_MODE_OUT;
	handlerC.GPIO_PinConfig.GPIO_PinOPType				= GPIO_OTYPER_PUSHPULL;
	handlerC.GPIO_PinConfig.GPIO_PinPuPdControl			= GPIO_PUPDR_NOTHING;
	handlerC.GPIO_PinConfig.GPIO_PinSpeed				= GPIO_OSPEED_HIGH;
	handlerC.GPIO_PinConfig.GPIO_PinAltFunMode			= AF0;

	//Cargamos la configuracion el pin especifico
	GPIO_Config(&handlerC);

	//El pin PA12 -> a
	handlerA.pGPIOx = GPIOA;
	handlerA.GPIO_PinConfig.GPIO_PinNumber				= PIN_12;
	handlerA.GPIO_PinConfig.GPIO_PinMode				= GPIO_MODE_OUT;
	handlerA.GPIO_PinConfig.GPIO_PinOPType				= GPIO_OTYPER_PUSHPULL;
	handlerA.GPIO_PinConfig.GPIO_PinPuPdControl			= GPIO_PUPDR_NOTHING;
	handlerA.GPIO_PinConfig.GPIO_PinSpeed				= GPIO_OSPEED_HIGH;
	handlerA.GPIO_PinConfig.GPIO_PinAltFunMode			= AF0;

	//Cargamos la configuracion el pin especifico
	GPIO_Config(&handlerA);

	//El pin PA11 -> b
	handlerB.pGPIOx = GPIOA;
	handlerB.GPIO_PinConfig.GPIO_PinNumber				= PIN_11;
	handlerB.GPIO_PinConfig.GPIO_PinMode				= GPIO_MODE_OUT;
	handlerB.GPIO_PinConfig.GPIO_PinOPType				= GPIO_OTYPER_PUSHPULL;
	handlerB.GPIO_PinConfig.GPIO_PinPuPdControl			= GPIO_PUPDR_NOTHING;
	handlerB.GPIO_PinConfig.GPIO_PinSpeed				= GPIO_OSPEED_HIGH;
	handlerB.GPIO_PinConfig.GPIO_PinAltFunMode			= AF0;

	//Cargamos la configuracion el pin especifico
	GPIO_Config(&handlerB);

	//El pin PB2 -> f
	handlerF.pGPIOx = GPIOB;
	handlerF.GPIO_PinConfig.GPIO_PinNumber				= PIN_2;
	handlerF.GPIO_PinConfig.GPIO_PinMode				= GPIO_MODE_OUT;
	handlerF.GPIO_PinConfig.GPIO_PinOPType				= GPIO_OTYPER_PUSHPULL;
	handlerF.GPIO_PinConfig.GPIO_PinPuPdControl			= GPIO_PUPDR_NOTHING;
	handlerF.GPIO_PinConfig.GPIO_PinSpeed				= GPIO_OSPEED_HIGH;
	handlerF.GPIO_PinConfig.GPIO_PinAltFunMode			= AF0;

	//Cargamos la configuracion el pin especifico
	GPIO_Config(&handlerF);

	//El pin PB1 -> g
	handlerG.pGPIOx = GPIOB;
	handlerG.GPIO_PinConfig.GPIO_PinNumber				= PIN_1;
	handlerG.GPIO_PinConfig.GPIO_PinMode				= GPIO_MODE_OUT;
	handlerG.GPIO_PinConfig.GPIO_PinOPType				= GPIO_OTYPER_PUSHPULL;
	handlerG.GPIO_PinConfig.GPIO_PinPuPdControl			= GPIO_PUPDR_NOTHING;
	handlerG.GPIO_PinConfig.GPIO_PinSpeed				= GPIO_OSPEED_HIGH;
	handlerG.GPIO_PinConfig.GPIO_PinAltFunMode			= AF0;

	//Cargamos la configuracion el pin especifico
	GPIO_Config(&handlerG);

	/* Definicion del timer para el display */
	//Seleccionar el timer que se va a usar -> Unidades
	handlerTimerDisplay.ptrTIMx								= TIM3;

	//Configuracion con la cual se maneja el timer
	handlerTimerDisplay.TIMx_Config.TIMx_mode				= BTIMER_MODE_UP;
	handlerTimerDisplay.TIMx_Config.TIMx_speed				= BTIMER_SPEED_1ms;
	handlerTimerDisplay.TIMx_Config.TIMx_period				= 5;
	handlerTimerDisplay.TIMx_Config.TIMx_interruptEnable	= SET;

	//Cargar la configuracion del Timer
	BasicTimer_Config(&handlerTimerDisplay);

	/* ----Configuracion del encoder---- */
	/* Configuracion de los pines */
	//Configuracion del SW - pin PA131
	SW.pGPIOx = GPIOC;
	SW.GPIO_PinConfig.GPIO_PinNumber			= PIN_10;
	SW.GPIO_PinConfig.GPIO_PinMode				= GPIO_MODE_IN;
	SW.GPIO_PinConfig.GPIO_PinPuPdControl		= GPIO_PUPDR_NOTHING;
	SW.GPIO_PinConfig.GPIO_PinSpeed				= GPIO_OSPEED_LOW;
	SW.GPIO_PinConfig.GPIO_PinAltFunMode		= AF0;

	//Configuracion del DT - pin PA14
	DT.pGPIOx = GPIOC;
	DT.GPIO_PinConfig.GPIO_PinNumber			= PIN_11;
	DT.GPIO_PinConfig.GPIO_PinMode				= GPIO_MODE_IN;
	DT.GPIO_PinConfig.GPIO_PinPuPdControl		= GPIO_PUPDR_NOTHING;
	DT.GPIO_PinConfig.GPIO_PinOPType            = GPIO_OTYPER_PUSHPULL;
	DT.GPIO_PinConfig.GPIO_PinSpeed				= GPIO_OSPEED_LOW;
	DT.GPIO_PinConfig.GPIO_PinAltFunMode		= AF0;
	GPIO_Config(&DT);


	//Configuracion del CLK - pin PA15
	CLK.pGPIOx = GPIOD;
	CLK.GPIO_PinConfig.GPIO_PinNumber			= PIN_2;
	CLK.GPIO_PinConfig.GPIO_PinMode				= GPIO_MODE_IN;
	CLK.GPIO_PinConfig.GPIO_PinPuPdControl		= GPIO_PUPDR_NOTHING;
	CLK.GPIO_PinConfig.GPIO_PinOPType           = GPIO_OTYPER_PUSHPULL;
	CLK.GPIO_PinConfig.GPIO_PinSpeed			= GPIO_OSPEED_LOW;

	//Interrupcion para cambiar de modo -> SW
	handlerEXTI_SW.edgeType						= EXTERNAL_INTERRUPT_FALLING_EDGE;
	handlerEXTI_SW.pGPIOHandler					= &SW;
	//Cargar la configuracion
	extInt_Config(&handlerEXTI_SW);

	//Interrupcion para detectar el giro del encoder -> CLK
	handlerEXTI_CLK.edgeType					= EXTERNAL_INTERRUPT_FALLING_EDGE;
	handlerEXTI_CLK.pGPIOHandler				= &CLK;
	//Cargar la configuracion
	extInt_Config(&handlerEXTI_CLK);
}


/* #----Definicion de la funcion callback del timer para el blinkypin----# */
void BasicTimer2_Callback(void){
	GPIO_TooglePin(&BlinkyPin); //Se cambia el estado del pin
}
/* #----Definicion de la funcion callback del timer para el display----# */
void BasicTimer3_Callback(void){
	banderaDisplay = 1; //Se coloca en 1 la bandera, indicando el switcheo de los transistores
	GPIO_TooglePin(&handlerUnidades); //Se cambia el valor del pin de las unidades
	estadoDisplay = GPIOReadPin(&handlerUnidades); //Se guarda el estado del pin encargado de activar las unidades en el display
	GPIO_WritePin(&handlerDecenas, !estadoDisplay); //Le doy el valor contrario al pin encargado de activar o desactivar las decenas en el display
}
/* #----Definicion del callback de la interrupcion del SW----# */
void callback_extInt10(void){
	modo ^= 1; //Se cambia el valor del modo
}
/* #----Definicion del callback de la interrupcion del CLK----# */
void callback_extInt2(void){
	giro = 1; //Cuando se detecta la interrupcion de coloca la variable giro igual a 1
}

/* #----Definicion de la funcion para escribir en el display----# */
 void writeDisplay(uint32_t num){

	/* Definicion donde se van a guardar el bit que le corresponde en el display*/
	uint8_t valor_a = 0; //Variable para guardar el valor del led a
	uint8_t valor_b = 0; //Variable para guardar el valor del led b
	uint8_t valor_c = 0; //Variable para guardar el valor del led c
	uint8_t valor_d = 0; //Variable para guardar el valor del led d
	uint8_t valor_e = 0; //Variable para guardar el valor del led e
	uint8_t valor_f = 0; //Variable para guardar el valor del led f
	uint8_t valor_g = 0; //Variable para guardar el valor del led g

	//Al numero hexadecimal se le aplica la mascara y se guarda su valor en el led correspondiente
	valor_a = 0b1 & num;
	valor_b = 0b1 & (num >> 1);
	valor_c = 0b1 & (num >> 2);
	valor_d = 0b1 & (num >> 3);
	valor_e = 0b1 & (num >> 4);
	valor_f = 0b1 & (num >> 5);
	valor_g = 0b1 & (num >> 6);

	//Se escribe el valor del led en su respectivo pin
	GPIO_WritePin(&handlerA, valor_a);
	GPIO_WritePin(&handlerB, valor_b);
	GPIO_WritePin(&handlerC, valor_c);
	GPIO_WritePin(&handlerD, valor_d);
	GPIO_WritePin(&handlerE, valor_e);
	GPIO_WritePin(&handlerF, valor_f);
	GPIO_WritePin(&handlerG, valor_g);
}

 /* #----Funcion para encender y apagar los leds en el modo de la culebrita----# */
 void DisplayCulebrita(void){
 	switch(posicion){
 		/*Dependiendo del valor de la posicion que tiene la culebrita se va a imprimir el resultado en el display*/
 	//La posicion inicial de la culebrita
 		case 0:
 			GPIO_WritePin(&handlerUnidades, ACTIVAR); //Se activa el display de unidades
 			GPIO_WritePin(&handlerDecenas, DESACTIVAR); //Se desactiva el display de decenas
 			writeDisplay(culebrita[posicion]); //Se escribe el led que se quiere prender
 			break;
 		//Segunda posicion
 		case 1:
 			GPIO_WritePin(&handlerUnidades, DESACTIVAR);
 			GPIO_WritePin(&handlerDecenas, ACTIVAR);
 			writeDisplay(culebrita[posicion]);
 			break;
 		//Tercera posicion
 		case 2:
 			GPIO_WritePin(&handlerUnidades, DESACTIVAR);
 			GPIO_WritePin(&handlerDecenas, ACTIVAR);
 			writeDisplay(culebrita[posicion]);
 			break;
 		//Cuarta posicion
 		case 3:
 			GPIO_WritePin(&handlerUnidades, DESACTIVAR);
 			GPIO_WritePin(&handlerDecenas, ACTIVAR);
 			writeDisplay(culebrita[posicion]);
 			break;
 		//Quinta posicion
 		case 4:
 			GPIO_WritePin(&handlerUnidades, DESACTIVAR);
 			GPIO_WritePin(&handlerDecenas, ACTIVAR);
 			writeDisplay(culebrita[posicion]);
 			break;
 		//Sexta posicion
 		case 5:
 			GPIO_WritePin(&handlerUnidades, ACTIVAR);
 			GPIO_WritePin(&handlerDecenas, DESACTIVAR);
 			writeDisplay(culebrita[posicion]);
 			break;
 		//Septima posicion
 		case 6:
 			GPIO_WritePin(&handlerUnidades, ACTIVAR);
 			GPIO_WritePin(&handlerDecenas, DESACTIVAR);
 			writeDisplay(culebrita[posicion]);
 			break;
 		//Octava posicion
 		case 7:
 			GPIO_WritePin(&handlerUnidades, DESACTIVAR);
 			GPIO_WritePin(&handlerDecenas, ACTIVAR);
 			writeDisplay(culebrita[posicion]);
 			break;
 		//Novena posicion
 		case 8:
 			GPIO_WritePin(&handlerUnidades, DESACTIVAR);
 			GPIO_WritePin(&handlerDecenas, ACTIVAR);
 			writeDisplay(culebrita[posicion]);
 			break;
 		//Decima posicion
 		case 9:
 			GPIO_WritePin(&handlerUnidades, ACTIVAR);
 			GPIO_WritePin(&handlerDecenas, DESACTIVAR);
 			writeDisplay(culebrita[posicion]);
 			break;
 		//Posicion numero 11
 		case 10:
 			GPIO_WritePin(&handlerUnidades, ACTIVAR);
 			GPIO_WritePin(&handlerDecenas, DESACTIVAR);
 			writeDisplay(culebrita[posicion]);
 			break;
 		//Posicion numero 12
 		case 11:
 			GPIO_WritePin(&handlerUnidades, ACTIVAR);
 			GPIO_WritePin(&handlerDecenas, DESACTIVAR);
 			writeDisplay(culebrita[posicion]);
 			break;
 		default:
 			break;
 		}
 }

 /* #----Definicion de la funcion para visualizar las decenas en el display----# */
 void DisplayDecenas(void){
 	decenas = contador / 10; //Se divide el numero que esta en el contador y se guarda en la variable i

 	writeDisplay(display[decenas]); //Copio en el display el numero que aparece en la posicion i del arreglo display
 }

 /* #----Definicion de la funcion visualizar las unidades en el display----# */
 void DisplayUnidades(void){
 	unidades = contador % 10; //Saco el residuo de la variable contador y la guardo en i

 	writeDisplay(display[unidades]); //Copio en el display el numero que aparece en la posicion i del arreglo display
 }

