/**
 ******************************************************************************
 * @file           : main.c
 * @author         : Samuel Huertas Rojas
 * @brief          : Main program body
 ******************************************************************************
 * Con este programa
 *
 ******************************************************************************
 */

#include <stdint.h>
#include "stm32f411xx_hal.h"
#include "GPIOxDriver.h"


#define INCREMENTE 			1;
#define DECREMENTE			0;
/*
 * ----Pregunta #1:----
 * La función GPIO_ReadPin() lo que hace es mover PinX veces hacia la derecha el registro que se encuentra en el PinX escogido.
 * Y devuelve el valor que se obtiene al realizar dicho desplazamiento.
 *
 * 	a. ¿Cual es el error?
 * El error que tiene la función GPIO_ReadPin() es que nos devuelve el valor total del desplazamiento de mover PinX veces el registro,
 * esto es, el valor del PinX más los otros registros que se encuentren a la izquierda del PinX, dando un valor mayor al valor real que tiene el PinX.
 *
 * 	b. ¿Como lo puede solucionar?
 * Este error se puede solucionar al aplicar una máscara la cual solo nos devuelva el valor real del PinX.
 * Esta máscara se puede aplicar después de realizar el desplazamiento PinX veces del registro, para que solo quede el valor real y elimine los otros registros.
 *
 */

//----Pregunta #3----
int main(void)
{
	//----#Variables#----
	//Variable para guardar el valor del Boton
	uint32_t valorDelUserBotton = 0;

	//Variables donde se van a guardar el nuemro binario
	uint8_t valorPA7 = 0;
	uint8_t valorPC8 = 0;
	uint8_t valorPC7 = 0;
	uint8_t valorPA6 = 0;
	uint8_t valorPB8 = 0;
	uint8_t valorPC6 = 0;
	uint8_t valorPC9 = 0;

	//Definición del contador, siempre va a empezar en el 0
	uint8_t contador = 0;

	//----#Definición de los Pines#----

	//Configurar cada uno de los pines como una salida de proposito general

	//----El primer pin a configurar es el PA7----

	//Definición del handler para el pin_PA7
	GPIO_Handler_t handlerPinPA7 = {0};

	//El pin_PA7 se va a trabajar con el GPIOA
	handlerPinPA7.pGPIOx = GPIOA;
	handlerPinPA7.GPIO_PinConfig.GPIO_PinNumber				= PIN_7;
	handlerPinPA7.GPIO_PinConfig.GPIO_PinMode				= GPIO_MODE_OUT;
	handlerPinPA7.GPIO_PinConfig.GPIO_PinOPType				= GPIO_OTYPER_PUSHPULL;
	handlerPinPA7.GPIO_PinConfig.GPIO_PinPuPdControl		= GPIO_PUPDR_NOTHING;
	handlerPinPA7.GPIO_PinConfig.GPIO_PinSpeed				= GPIO_OSPEED_MEDIUM;
	handlerPinPA7.GPIO_PinConfig.GPIO_PinAltFunMode			= AF0;

	//Cargamos la configuración al pin especifico
	GPIO_Config(&handlerPinPA7);

	//----El segundo pin a configurar es el PC8----

	//Definición del handler para el pin_PC8
	GPIO_Handler_t handlerPinPC8 = {0};

	//El pinPC8 se va a trabajar con el GPIOC
	handlerPinPC8.pGPIOx = GPIOC;
	handlerPinPC8.GPIO_PinConfig.GPIO_PinNumber				= PIN_8;
	handlerPinPC8.GPIO_PinConfig.GPIO_PinMode				= GPIO_MODE_OUT;
	handlerPinPC8.GPIO_PinConfig.GPIO_PinOPType				= GPIO_OTYPER_PUSHPULL;
	handlerPinPC8.GPIO_PinConfig.GPIO_PinPuPdControl		= GPIO_PUPDR_NOTHING;
	handlerPinPC8.GPIO_PinConfig.GPIO_PinSpeed				= GPIO_OSPEED_MEDIUM;
	handlerPinPC8.GPIO_PinConfig.GPIO_PinAltFunMode			= AF0;

	//Cargamos la configuración al pin especifico
	GPIO_Config(&handlerPinPC8);

	//----El tercer pin a configurar es el PC7----

	//Definición del handler para el pin_PC7
	GPIO_Handler_t handlerPinPC7 = {0};

	//El pinPC7 se va a trabajar con el GPIOC
	handlerPinPC7.pGPIOx = GPIOC;
	handlerPinPC7.GPIO_PinConfig.GPIO_PinNumber				= PIN_7;
	handlerPinPC7.GPIO_PinConfig.GPIO_PinMode				= GPIO_MODE_OUT;
	handlerPinPC7.GPIO_PinConfig.GPIO_PinOPType				= GPIO_OTYPER_PUSHPULL;
	handlerPinPC7.GPIO_PinConfig.GPIO_PinPuPdControl		= GPIO_PUPDR_NOTHING;
	handlerPinPC7.GPIO_PinConfig.GPIO_PinSpeed				= GPIO_OSPEED_MEDIUM;
	handlerPinPC7.GPIO_PinConfig.GPIO_PinAltFunMode			= AF0;

	//Cargamos la configuración al pin especifico
	GPIO_Config(&handlerPinPC7);

	//----El cuarto pin a configurar es el PA6----

	//Definición del handler para el pin_PA6
	GPIO_Handler_t handlerPinPA6 = {0};

	//El pinPA6 se va a trabajar con el GPIOA
	handlerPinPA6.pGPIOx = GPIOA;
	handlerPinPA6.GPIO_PinConfig.GPIO_PinNumber				= PIN_6;
	handlerPinPA6.GPIO_PinConfig.GPIO_PinMode				= GPIO_MODE_OUT;
	handlerPinPA6.GPIO_PinConfig.GPIO_PinOPType				= GPIO_OTYPER_PUSHPULL;
	handlerPinPA6.GPIO_PinConfig.GPIO_PinPuPdControl		= GPIO_PUPDR_NOTHING;
	handlerPinPA6.GPIO_PinConfig.GPIO_PinSpeed				= GPIO_OSPEED_MEDIUM;
	handlerPinPA6.GPIO_PinConfig.GPIO_PinAltFunMode			= AF0;

	//Cargamos la configuración al pin especifico
	GPIO_Config(&handlerPinPA6);

	//----El quinto pin a configurar es el PB8----

	//Definición del handler para el pin_PB8
	GPIO_Handler_t handlerPinPB8 = {0};

	//El pinPC8 se va a trabajar con el GPIOB
	handlerPinPB8.pGPIOx = GPIOB;
	handlerPinPB8.GPIO_PinConfig.GPIO_PinNumber				= PIN_8;
	handlerPinPB8.GPIO_PinConfig.GPIO_PinMode				= GPIO_MODE_OUT;
	handlerPinPB8.GPIO_PinConfig.GPIO_PinOPType				= GPIO_OTYPER_PUSHPULL;
	handlerPinPB8.GPIO_PinConfig.GPIO_PinPuPdControl		= GPIO_PUPDR_NOTHING;
	handlerPinPB8.GPIO_PinConfig.GPIO_PinSpeed				= GPIO_OSPEED_MEDIUM;
	handlerPinPB8.GPIO_PinConfig.GPIO_PinAltFunMode			= AF0;

	//Cargamos la configuración al pin especifico
	GPIO_Config(&handlerPinPB8);

	//----El sexto pin a configurar el es PC6----

	//Definición del handler para el pin_PC6
	GPIO_Handler_t handlerPinPC6 = {0};

	//El pinPC6 se va a trabajar con el GPIOC
	handlerPinPC6.pGPIOx = GPIOC;
	handlerPinPC6.GPIO_PinConfig.GPIO_PinNumber				= PIN_6;
	handlerPinPC6.GPIO_PinConfig.GPIO_PinMode				= GPIO_MODE_OUT;
	handlerPinPC6.GPIO_PinConfig.GPIO_PinOPType				= GPIO_OTYPER_PUSHPULL;
	handlerPinPC6.GPIO_PinConfig.GPIO_PinPuPdControl		= GPIO_PUPDR_NOTHING;
	handlerPinPC6.GPIO_PinConfig.GPIO_PinSpeed				= GPIO_OSPEED_MEDIUM;
	handlerPinPC6.GPIO_PinConfig.GPIO_PinAltFunMode			= AF0;

	//Cargamos la configuración al pin especifico
	GPIO_Config(&handlerPinPC6);

	//----El septimo pin a configurar es el PC9----

	//Definición del handler para el pin_PC9
	GPIO_Handler_t handlerPinPC9 = {0};

	//El pinPC9 se va a trabajar con el GPIOC
	handlerPinPC9.pGPIOx = GPIOC;
	handlerPinPC9.GPIO_PinConfig.GPIO_PinNumber				= PIN_9;
	handlerPinPC9.GPIO_PinConfig.GPIO_PinMode				= GPIO_MODE_OUT;
	handlerPinPC9.GPIO_PinConfig.GPIO_PinOPType				= GPIO_OTYPER_PUSHPULL;
	handlerPinPC9.GPIO_PinConfig.GPIO_PinPuPdControl		= GPIO_PUPDR_NOTHING;
	handlerPinPC9.GPIO_PinConfig.GPIO_PinSpeed				= GPIO_OSPEED_MEDIUM;
	handlerPinPC9.GPIO_PinConfig.GPIO_PinAltFunMode			= AF0;

	//Cargamos la configuración al pin especifico
	GPIO_Config(&handlerPinPC9);

	//----Configuración del USER_BOTTON----
	//Definición del hanlder para el USER_BOTTON
	GPIO_Handler_t handlerUserBotton = {0};

	//Configuración del pin para el USER_BOTTON
	handlerUserBotton.pGPIOx = GPIOC;
	handlerUserBotton.GPIO_PinConfig.GPIO_PinNumber			= PIN_13;
	handlerUserBotton.GPIO_PinConfig.GPIO_PinMode			= GPIO_MODE_IN;
	handlerUserBotton.GPIO_PinConfig.GPIO_PinOPType			= GPIO_OTYPER_OPENDRAIN;
	handlerUserBotton.GPIO_PinConfig.GPIO_PinSpeed			= GPIO_OSPEED_MEDIUM;
	handlerUserBotton.GPIO_PinConfig.GPIO_PinAltFunMode		= AF0;

	//Cargar la configuración del User_Botton
	GPIO_Config(&handlerUserBotton);

    /* Loop forever */
	while(1){

		//Ciclo for para crear el delay de 1 segundo
		for(uint32_t i = 0; i <= 1600000; i ++){
			NOP();
		}

		//Variable para guardar el estado del boton, se hace lectura del estado del boton
		valorDelUserBotton = GPIOReadPin(&handlerUserBotton);

		//Condicional para ver si el boton se encuentra presionado o no
		//haciendo que el conteo se incremente o decremente
		switch(valorDelUserBotton){
			case 1:
				contador += 1; //se pone el contador a incrementar de 1 si el boton no está precionado
				if(contador == 61){
					contador = 1; //el reinicio del contador cuando llegue a 60
				}
				break;
			case 0:
				contador -= 1; //se pone el contador a decrementar de 1 si el boton está precionado
				if(contador == 0){
					contador = 60; //el reinicio del contador cuando llegue a 1
				}
				break;
		}

		//Mover el bit del contador a su pin correspondinete
		valorPA7 = (contador);
		valorPC8 = (contador >> 1);
		valorPC7 = (contador >> 2);
		valorPA6 = (contador >> 3);
		valorPB8 = (contador >> 4);
		valorPC6 = (contador >> 5);
		valorPC9 = (contador >> 6);

		//Aplicar una mascara para sacar el bit que le corresponde
		valorPA7 &= (0b1);
		valorPC8 &= (0b1);
		valorPC7 &= (0b1);
		valorPA6 &= (0b1);
		valorPB8 &= (0b1);
		valorPC6 &= (0b1);
		valorPC9 &= (0b1);

		//Guardar el valor en cada uno de los pines
		GPIO_WritePin(&handlerPinPA7, valorPA7);
		GPIO_WritePin(&handlerPinPC8, valorPC8);
		GPIO_WritePin(&handlerPinPC7, valorPC7);
		GPIO_WritePin(&handlerPinPA6, valorPA6);
		GPIO_WritePin(&handlerPinPB8, valorPB8);
		GPIO_WritePin(&handlerPinPC6, valorPC6);
		GPIO_WritePin(&handlerPinPC9, valorPC9);

	}
	return 0;

}
