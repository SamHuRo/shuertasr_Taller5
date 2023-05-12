/*
 * GPIOxDriver.c
 *
 *  Created on: May 12, 2023
 *      Author: samuel
 */

#include <stm32f4xx.h>
#include "GPIOxDriver.h"

/*
 * Siempre que se vaya a configurar un Pin se debe de activar su señal de reloj (relacionado al RCC),
 * a esto lo llamaremos “activar el periférico o activar la señal de reloj del periférico”.
 */

void GPIO_Config(GPIO_Handler_t *pGPIOHandler){
	//Variables para hacer todo paso a paso
	uint32_t auxConfig = 0;
	uint32_t auxPosition = 0;

	//1. Activar el periferico
	//Verificamos para GPIOA
	if(pGPIOHandler->pGPIOx == GPIOA){
		//Escribimos 1 (SET) en la posición correspondiente
		RCC->AHB1ENR |= (RCC_AHB1ENR_GPIOAEN);
	}
	//Verificamos para GPIOB
	else if(pGPIOHandler->pGPIOx == GPIOB){
		RCC->AHB1ENR |= (RCC_AHB1ENR_GPIOBEN);
	}
	//Verificamos para GPIOC
	else if(pGPIOHandler->pGPIOx == GPIOC){
			RCC->AHB1ENR |= (RCC_AHB1ENR_GPIOCEN);
		}
	//Verificamos para GPIOD
	else if(pGPIOHandler->pGPIOx == GPIOD){
			RCC->AHB1ENR |= (RCC_AHB1ENR_GPIODEN);
		}
	//Verificamos para GPIOE
	else if(pGPIOHandler->pGPIOx == GPIOE){
			RCC->AHB1ENR |= (RCC_AHB1ENR_GPIOEEN);
		}
	else if(pGPIOHandler->pGPIOx == GPIOH){
			RCC->AHB1ENR |= (RCC_AHB1ENR_GPIOHEN);
		}
	//Después de estar activado, podemos comenzar a configurar.

	//2. Configuración del registro GPIOx_MODER
	//Acá estamos leyendo la config, moviendo "PinNumber" veces hacia la izquierda ese valor
	//y todo eso lo cargamos en la variable auxConfig
	auxConfig = (pGPIOHandler->GPIO_PinConfig.GPIO_PinMode << 2 * pGPIOHandler->GPIO_PinConfig.GPIO_PinNumber);

	//Antes de cargar el nuevo valor, limpiamos los bits específicos de ese registro (0b00)
	// para lo cual aplicamos una máscara y una operación
	pGPIOHandler->pGPIOx->MODER &= ~(0b11 << 2 * pGPIOHandler->GPIO_PinConfig.GPIO_PinNumber);

	//Cargamos a auxConfig en el registro MODER
	pGPIOHandler->pGPIOx->MODER |= auxConfig;

	//3. Configuramos el registro GPIOx_OTYPER
	//Leemos y movemos el valor un numero "PinNumber" de veces
	auxConfig = (pGPIOHandler->GPIO_PinConfig.GPIO_PinOPType << pGPIOHandler->GPIO_PinConfig.GPIO_PinNumber);

	//Limpiamos antes de cargar
	pGPIOHandler->pGPIOx->OTYPER &= ~(SET << pGPIOHandler->GPIO_PinConfig.GPIO_PinNumber);

	//Cargamos el resultado sobre el registro adecuado
	pGPIOHandler->pGPIOx->OTYPER |= auxConfig;

	//4. Configurando ahora la velocidad
	auxConfig = (pGPIOHandler->GPIO_PinConfig.GPIO_PinSpeed << 2*pGPIOHandler->GPIO_PinConfig.GPIO_PinNumber);

	//Limpiamos la posición antes de cargar la nueva configuración
	pGPIOHandler->pGPIOx->OSPEEDR &= ~(0b11 << 2 * pGPIOHandler->GPIO_PinConfig.GPIO_PinNumber);

	//Cargamos el resultado sobre el registro adecuado
	pGPIOHandler->pGPIOx->OSPEEDR |= auxConfig;

	//5. Configuramos si se desea pull-up, pull-down o flotante
	auxConfig = (pGPIOHandler->GPIO_PinConfig.GPIO_PinPuPdControl << 2*pGPIOHandler->GPIO_PinConfig.GPIO_PinNumber);

	//Limpiamos la posición antes de cargar la nueva configuración
	pGPIOHandler->pGPIOx->PUPDR &= ~(0b11 << 2 * pGPIOHandler->GPIO_PinConfig.GPIO_PinNumber);

	//Cargamos el resultado sobre el registro adecuado
	pGPIOHandler->pGPIOx->PUPDR |= auxConfig;

	//Esta es la parte para la configuración de las funciones alternativas
	if(pGPIOHandler->GPIO_PinConfig.GPIO_PinMode == GPIO_MODE_ALTFN){

		//Seleccionamos primero si se debe utilizar el registro bajo (AFRL) o el alto (AFRH)
		if(pGPIOHandler->GPIO_PinConfig.GPIO_PinNumber < 8){
			//Estamos en el registro AFRL; que controla los pines del Pin_0 al Pin-7
			auxPosition = 4 * pGPIOHandler->GPIO_PinConfig.GPIO_PinNumber;

			//Limpiamos primero la posición del registro
			pGPIOHandler->pGPIOx->AFR[0] &= ~(0b1111 << auxPosition);

			//Escribimos el valor configurado en la posición seleccionada
			pGPIOHandler->pGPIOx->AFR[0] |= (pGPIOHandler->GPIO_PinConfig.GPIO_PinAltFunMode << auxPosition);
		}
		else{
			//Estamos en el registro AFRH, que controla los pines del Pin_8 al Pin_15
			auxPosition = 4 * (pGPIOHandler->GPIO_PinConfig.GPIO_PinNumber -8);

			//Limpiamos primero la posición del registroque deseamos escribir a continuación
			pGPIOHandler->pGPIOx->AFR[1] &= ~(0b1111 << auxPosition);

			//Escribimos el valor configurado en la posición seleccionada
			pGPIOHandler->pGPIOx->AFR[1] |= (pGPIOHandler->GPIO_PinConfig.GPIO_PinAltFunMode << auxPosition);
		}
	}
} //Fin dek GPIO_Config

/*
 * Función utilizada para cambiar de estado del pin entregado en el handler, asignado
 * el valor entregado en la varible newState
 */
void GPIO_WritePin(GPIO_Handler_t *pPinHandler, uint8_t newState){
	//Limpiamos la posición deseada
	//pPinHandler->pGPIOx->ODR &= ~(set << pPinHandler->GPIO_PinConfig.GPIO_PinNumber);
	if(newState == SET){
		//Trabajando con la parte baja del registro
		pPinHandler->pGPIOx->BSRR |= (SET << pPinHandler->GPIO_PinConfig.GPIO_PinNumber);
	}
	else{
		//Trabajando con la parte alta del registro
		pPinHandler->pGPIOx->BSRR |= (SET << (pPinHandler->GPIO_PinConfig.GPIO_PinNumber + 16));
	}
}//Fin de la función GPIO_WritePin

/*
 * Función para leer el estado de un pin especifico
 */
uint32_t GPIOReadPin(GPIO_Handler_t *pPinHandler){
	//Creamos una variable auxiliar la cual luego retornaremos
	uint32_t pinValue = 0;

	//Cargamos el valor del registro IDR, desplazado a derecha tantas veces como la ubiación
	// del Pin especifico
	pinValue = (pPinHandler->pGPIOx->IDR >> pPinHandler->GPIO_PinConfig.GPIO_PinNumber);

	//Aplicando la mascara al valor del pinValue
	pinValue &= (0b1);

	return pinValue;
}//Fin de la función GPIO_ReadPin

/*
 * Funcion para cambiar el estado de un pin en especifico
 */
void GPIO_TooglePin(GPIO_Handler_t *pPinHandler){

	//Cambiar el estado del pin
	pPinHandler->pGPIOx->ODR ^= (0b1 << pPinHandler->GPIO_PinConfig.GPIO_PinNumber);

}//Fin de la funcion GPIO_TooglePin

