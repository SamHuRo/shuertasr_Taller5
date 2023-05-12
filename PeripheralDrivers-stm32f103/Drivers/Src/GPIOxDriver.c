/*
 * GPIOxDriver.c
 *
 *  Created on: May 12, 2023
 *      Author: samuel
 */
#include <stm32f1xx.h>
#include "GPIOxDriver.h"

/*
 * Siempre que se vaya a configurar un Pin se debe de activar su señal de reloj (relacionado al RCC),
 * a esto lo llamaremos “activar el periférico o activar la señal de reloj del periférico”.
 */

void GPIO_Config(GPIO_Handler_t *pGPIOHandler){
	//Variables para hacer todo paso a paso
//	uint32_t auxConfig = 0;
//	uint32_t auxPosition = 0;

	//1. Activar el periferico
	//Verificamos para GPIOA
	if(pGPIOHandler->pGPIOx == GPIOA){
		//Escribimos 1 (SET) en la posición correspondiente
		RCC->APB2ENR |= (RCC_APB2ENR_IOPAEN);
	}
	//Verificamos para GPIOB
	else if(pGPIOHandler->pGPIOx == GPIOB){
		RCC->APB2ENR |= (RCC_APB2ENR_IOPBEN);
	}
	//Verificamos para GPIOC
	else if(pGPIOHandler->pGPIOx == GPIOC){
		RCC->APB2ENR |= (RCC_APB2ENR_IOPCEN);
	}
	//Verificamos para GPIOD
	else if(pGPIOHandler->pGPIOx == GPIOD){
		RCC->APB2ENR |= (RCC_APB2ENR_IOPDEN);
	}
	//Verificamos para GPIOE
	else if(pGPIOHandler->pGPIOx == GPIOE){
		RCC->APB2ENR |= (RCC_APB2ENR_IOPEEN);
	}
	//Después de estar activado, podemos comenzar a configurar.
	/*Se realiza con los pine en CRL*/
	//Limpiamos la posicion del MODE
	pGPIOHandler->pGPIOx->CRL &= ~(0b11 << (pGPIOHandler->GPIO_PinConfig.GPIO_PinNumber*4));
	//Limpiamos la posicion del CNF
	pGPIOHandler->pGPIOx->CRL &= ~(0b11 << (pGPIOHandler->GPIO_PinConfig.GPIO_PinNumber*4 + 2));
	/*Se realiza con lis pines en CRH*/
	//Limpiamos la posicion del MODE
	pGPIOHandler->pGPIOx->CRH &= ~(0b11 << (pGPIOHandler->GPIO_PinConfig.GPIO_PinNumber*4 - 32));
	//Limpiamos la posicion del CNF
	pGPIOHandler->pGPIOx->CRH &= ~(0b11 << (pGPIOHandler->GPIO_PinConfig.GPIO_PinNumber*4 - 30));


	//2. Configuración del registro para el modo del pin
	switch(pGPIOHandler->GPIO_PinConfig.GPIO_PinNumber){
	//Miramos el pin que se escogio
	case 0:
		if(pGPIOHandler->GPIO_PinConfig.GPIO_PinMode == GPIO_MODE_IN){
			//Se pone los registros del MODE en 00
			pGPIOHandler->pGPIOx->CRL &= ~GPIO_CRL_MODE0;
			//Se escoge el modo en el que se quiere el pin si analogo, flotante, pull-down o pull-up
			if(pGPIOHandler->GPIO_PinConfig.GPIO_PinPuPdControl == GPIO_PUPDR_ANALOG){
				pGPIOHandler->pGPIOx->CRL &= ~GPIO_CRL_CNF0;
			}else if(pGPIOHandler->GPIO_PinConfig.GPIO_PinPuPdControl == GPIO_PUPDR_FLOATING){
				pGPIOHandler->pGPIOx->CRL |= GPIO_CRL_CNF0_0;
			}else if(pGPIOHandler->GPIO_PinConfig.GPIO_PinPuPdControl == GPIO_PUPDR_PULLDOWN){
				pGPIOHandler->pGPIOx->CRL |= GPIO_CRL_CNF0_1;
			}else if(pGPIOHandler->GPIO_PinConfig.GPIO_PinPuPdControl == GPIO_PUPDR_PULLUP){
				pGPIOHandler->pGPIOx->CRL |= GPIO_CRL_CNF0_1;
			}
		}else if(pGPIOHandler->GPIO_PinConfig.GPIO_PinMode == GPIO_MODE_OUT){
			//Se va a escoger cual es la velocidad de salida del pin
			if(pGPIOHandler->GPIO_PinConfig.GPIO_PinSpeed == GPIO_OSPEED_10MHz){
				pGPIOHandler->pGPIOx->CRL |= GPIO_CRL_MODE0_0 ;
			}else if(pGPIOHandler->GPIO_PinConfig.GPIO_PinSpeed == GPIO_OSPEED_2MHz){
				pGPIOHandler->pGPIOx->CRL |= GPIO_CRL_MODE0_1;
			}else if(pGPIOHandler->GPIO_PinConfig.GPIO_PinSpeed == GPIO_OSPEED_50MHz){
				pGPIOHandler->pGPIOx->CRL |= GPIO_CRL_MODE0;
			}else{
				pGPIOHandler->pGPIOx->CRL |= GPIO_CRL_MODE0_0;
			}
			//Se escoge el modo que se quiere poner el pin
			if(pGPIOHandler->GPIO_PinConfig.GPIO_PinOPType == GPIO_OTYPER_PUSHPULL){
				pGPIOHandler->pGPIOx->CRL &= ~GPIO_CRL_CNF0;
			}else if(pGPIOHandler->GPIO_PinConfig.GPIO_PinOPType == GPIO_OTYPER_OPENDRAIN){
				pGPIOHandler->pGPIOx->CRL |= GPIO_CRL_CNF0_0;
			}
		}else if(pGPIOHandler->GPIO_PinConfig.GPIO_PinMode == GPIO_MODE_AF){
			//Se va a escoger cual es la velocidad de salida del pin
			if(pGPIOHandler->GPIO_PinConfig.GPIO_PinSpeed == GPIO_OSPEED_10MHz){
				pGPIOHandler->pGPIOx->CRL |= GPIO_CRL_MODE0_0;
			}else if(pGPIOHandler->GPIO_PinConfig.GPIO_PinSpeed == GPIO_OSPEED_2MHz){
				pGPIOHandler->pGPIOx->CRL |= GPIO_CRL_MODE0_1;
			}else if(pGPIOHandler->GPIO_PinConfig.GPIO_PinSpeed == GPIO_OSPEED_50MHz){
				pGPIOHandler->pGPIOx->CRL |= GPIO_CRL_MODE0;
			}else{
				pGPIOHandler->pGPIOx->CRL |= GPIO_CRL_MODE0_0;
			}
			//Se escoge el modo que se quiere poner el pin
			if(pGPIOHandler->GPIO_PinConfig.GPIO_PinOPType == GPIO_OTYPER_PUSHPULL){
				pGPIOHandler->pGPIOx->CRL |= GPIO_CRL_CNF0_1;
				pGPIOHandler->pGPIOx->CRL &= ~GPIO_CRL_CNF0_0;
			}else if(pGPIOHandler->GPIO_PinConfig.GPIO_PinOPType == GPIO_OTYPER_OPENDRAIN){
				pGPIOHandler->pGPIOx->CRL |= GPIO_CRL_CNF0;
			}
		}
		break;
	case 1:
		if(pGPIOHandler->GPIO_PinConfig.GPIO_PinMode == GPIO_MODE_IN){
			//Se pone los registros del MODE en 00
			pGPIOHandler->pGPIOx->CRL &= ~GPIO_CRL_MODE1;
			//Se escoge el modo en el que se quiere el pin si analogo, flotante, pull-down o pull-up
			if(pGPIOHandler->GPIO_PinConfig.GPIO_PinPuPdControl == GPIO_PUPDR_ANALOG){
				pGPIOHandler->pGPIOx->CRL &= ~GPIO_CRL_CNF1;
			}else if(pGPIOHandler->GPIO_PinConfig.GPIO_PinPuPdControl == GPIO_PUPDR_FLOATING){
				pGPIOHandler->pGPIOx->CRL |= GPIO_CRL_CNF1_0;
			}else if(pGPIOHandler->GPIO_PinConfig.GPIO_PinPuPdControl == GPIO_PUPDR_PULLDOWN){
				pGPIOHandler->pGPIOx->CRL |= GPIO_CRL_CNF1_1;
			}else if(pGPIOHandler->GPIO_PinConfig.GPIO_PinPuPdControl == GPIO_PUPDR_PULLUP){
				pGPIOHandler->pGPIOx->CRL |= GPIO_CRL_CNF1_1;
			}
		}else if(pGPIOHandler->GPIO_PinConfig.GPIO_PinMode == GPIO_MODE_OUT){
			//Se va a escoger cual es la velocidad de salida del pin
			if(pGPIOHandler->GPIO_PinConfig.GPIO_PinSpeed == GPIO_OSPEED_10MHz){
				pGPIOHandler->pGPIOx->CRL |= GPIO_CRL_MODE1_0 ;
			}else if(pGPIOHandler->GPIO_PinConfig.GPIO_PinSpeed == GPIO_OSPEED_2MHz){
				pGPIOHandler->pGPIOx->CRL |= GPIO_CRL_MODE1_1;
			}else if(pGPIOHandler->GPIO_PinConfig.GPIO_PinSpeed == GPIO_OSPEED_50MHz){
				pGPIOHandler->pGPIOx->CRL |= GPIO_CRL_MODE1;
			}else{
				pGPIOHandler->pGPIOx->CRL |= GPIO_CRL_MODE1_0;
			}
			//Se escoge el modo que se quiere poner el pin
			if(pGPIOHandler->GPIO_PinConfig.GPIO_PinOPType == GPIO_OTYPER_PUSHPULL){
				pGPIOHandler->pGPIOx->CRL &= ~GPIO_CRL_CNF1;
			}else if(pGPIOHandler->GPIO_PinConfig.GPIO_PinOPType == GPIO_OTYPER_OPENDRAIN){
				pGPIOHandler->pGPIOx->CRL |= GPIO_CRL_CNF1_0;
			}
		}else if(pGPIOHandler->GPIO_PinConfig.GPIO_PinMode == GPIO_MODE_AF){
			//Se va a escoger cual es la velocidad de salida del pin
			if(pGPIOHandler->GPIO_PinConfig.GPIO_PinSpeed == GPIO_OSPEED_10MHz){
				pGPIOHandler->pGPIOx->CRL |= GPIO_CRL_MODE1_0;
			}else if(pGPIOHandler->GPIO_PinConfig.GPIO_PinSpeed == GPIO_OSPEED_2MHz){
				pGPIOHandler->pGPIOx->CRL |= GPIO_CRL_MODE1_1;
			}else if(pGPIOHandler->GPIO_PinConfig.GPIO_PinSpeed == GPIO_OSPEED_50MHz){
				pGPIOHandler->pGPIOx->CRL |= GPIO_CRL_MODE1;
			}else{
				pGPIOHandler->pGPIOx->CRL |= GPIO_CRL_MODE1_0;
			}
			//Se escoge el modo que se quiere poner el pin
			if(pGPIOHandler->GPIO_PinConfig.GPIO_PinOPType == GPIO_OTYPER_PUSHPULL){
				pGPIOHandler->pGPIOx->CRL |= GPIO_CRL_CNF1_1;
				pGPIOHandler->pGPIOx->CRL &= ~GPIO_CRL_CNF1_0;
			}else if(pGPIOHandler->GPIO_PinConfig.GPIO_PinOPType == GPIO_OTYPER_OPENDRAIN){
				pGPIOHandler->pGPIOx->CRL |= GPIO_CRL_CNF1;
			}
		}
		break;
	case 2:
		if(pGPIOHandler->GPIO_PinConfig.GPIO_PinMode == GPIO_MODE_IN){
			//Se pone los registros del MODE en 00
			pGPIOHandler->pGPIOx->CRL &= ~GPIO_CRL_MODE2;
			//Se escoge el modo en el que se quiere el pin si analogo, flotante, pull-down o pull-up
			if(pGPIOHandler->GPIO_PinConfig.GPIO_PinPuPdControl == GPIO_PUPDR_ANALOG){
				pGPIOHandler->pGPIOx->CRL &= ~GPIO_CRL_CNF2;
			}else if(pGPIOHandler->GPIO_PinConfig.GPIO_PinPuPdControl == GPIO_PUPDR_FLOATING){
				pGPIOHandler->pGPIOx->CRL |= GPIO_CRL_CNF2_0;
			}else if(pGPIOHandler->GPIO_PinConfig.GPIO_PinPuPdControl == GPIO_PUPDR_PULLDOWN){
				pGPIOHandler->pGPIOx->CRL |= GPIO_CRL_CNF2_1;
			}else if(pGPIOHandler->GPIO_PinConfig.GPIO_PinPuPdControl == GPIO_PUPDR_PULLUP){
				pGPIOHandler->pGPIOx->CRL |= GPIO_CRL_CNF2_1;
			}
		}else if(pGPIOHandler->GPIO_PinConfig.GPIO_PinMode == GPIO_MODE_OUT){
			//Se va a escoger cual es la velocidad de salida del pin
			if(pGPIOHandler->GPIO_PinConfig.GPIO_PinSpeed == GPIO_OSPEED_10MHz){
				pGPIOHandler->pGPIOx->CRL |= GPIO_CRL_MODE2_0 ;
			}else if(pGPIOHandler->GPIO_PinConfig.GPIO_PinSpeed == GPIO_OSPEED_2MHz){
				pGPIOHandler->pGPIOx->CRL |= GPIO_CRL_MODE2_1;
			}else if(pGPIOHandler->GPIO_PinConfig.GPIO_PinSpeed == GPIO_OSPEED_50MHz){
				pGPIOHandler->pGPIOx->CRL |= GPIO_CRL_MODE2;
			}else{
				pGPIOHandler->pGPIOx->CRL |= GPIO_CRL_MODE2_0;
			}
			//Se escoge el modo que se quiere poner el pin
			if(pGPIOHandler->GPIO_PinConfig.GPIO_PinOPType == GPIO_OTYPER_PUSHPULL){
				pGPIOHandler->pGPIOx->CRL &= ~GPIO_CRL_CNF2;
			}else if(pGPIOHandler->GPIO_PinConfig.GPIO_PinOPType == GPIO_OTYPER_OPENDRAIN){
				pGPIOHandler->pGPIOx->CRL |= GPIO_CRL_CNF2_0;
			}
		}else if(pGPIOHandler->GPIO_PinConfig.GPIO_PinMode == GPIO_MODE_AF){
			//Se va a escoger cual es la velocidad de salida del pin
			if(pGPIOHandler->GPIO_PinConfig.GPIO_PinSpeed == GPIO_OSPEED_10MHz){
				pGPIOHandler->pGPIOx->CRL |= GPIO_CRL_MODE2_0;
			}else if(pGPIOHandler->GPIO_PinConfig.GPIO_PinSpeed == GPIO_OSPEED_2MHz){
				pGPIOHandler->pGPIOx->CRL |= GPIO_CRL_MODE2_1;
			}else if(pGPIOHandler->GPIO_PinConfig.GPIO_PinSpeed == GPIO_OSPEED_50MHz){
				pGPIOHandler->pGPIOx->CRL |= GPIO_CRL_MODE2;
			}else{
				pGPIOHandler->pGPIOx->CRL |= GPIO_CRL_MODE2_0;
			}
			//Se escoge el modo que se quiere poner el pin
			if(pGPIOHandler->GPIO_PinConfig.GPIO_PinOPType == GPIO_OTYPER_PUSHPULL){
				pGPIOHandler->pGPIOx->CRL |= GPIO_CRL_CNF2_1;
				pGPIOHandler->pGPIOx->CRL &= ~GPIO_CRL_CNF2_0;
			}else if(pGPIOHandler->GPIO_PinConfig.GPIO_PinOPType == GPIO_OTYPER_OPENDRAIN){
				pGPIOHandler->pGPIOx->CRL |= GPIO_CRL_CNF2;
			}
		}
		break;
	case 3:
		if(pGPIOHandler->GPIO_PinConfig.GPIO_PinMode == GPIO_MODE_IN){
			//Se pone los registros del MODE en 00
			pGPIOHandler->pGPIOx->CRL &= ~GPIO_CRL_MODE3;
			//Se escoge el modo en el que se quiere el pin si analogo, flotante, pull-down o pull-up
			if(pGPIOHandler->GPIO_PinConfig.GPIO_PinPuPdControl == GPIO_PUPDR_ANALOG){
				pGPIOHandler->pGPIOx->CRL &= ~GPIO_CRL_CNF3;
			}else if(pGPIOHandler->GPIO_PinConfig.GPIO_PinPuPdControl == GPIO_PUPDR_FLOATING){
				pGPIOHandler->pGPIOx->CRL |= GPIO_CRL_CNF3_0;
			}else if(pGPIOHandler->GPIO_PinConfig.GPIO_PinPuPdControl == GPIO_PUPDR_PULLDOWN){
				pGPIOHandler->pGPIOx->CRL |= GPIO_CRL_CNF3_1;
			}else if(pGPIOHandler->GPIO_PinConfig.GPIO_PinPuPdControl == GPIO_PUPDR_PULLUP){
				pGPIOHandler->pGPIOx->CRL |= GPIO_CRL_CNF3_1;
			}
		}else if(pGPIOHandler->GPIO_PinConfig.GPIO_PinMode == GPIO_MODE_OUT){
			//Se va a escoger cual es la velocidad de salida del pin
			if(pGPIOHandler->GPIO_PinConfig.GPIO_PinSpeed == GPIO_OSPEED_10MHz){
				pGPIOHandler->pGPIOx->CRL |= GPIO_CRL_MODE3_0 ;
			}else if(pGPIOHandler->GPIO_PinConfig.GPIO_PinSpeed == GPIO_OSPEED_2MHz){
				pGPIOHandler->pGPIOx->CRL |= GPIO_CRL_MODE3_1;
			}else if(pGPIOHandler->GPIO_PinConfig.GPIO_PinSpeed == GPIO_OSPEED_50MHz){
				pGPIOHandler->pGPIOx->CRL |= GPIO_CRL_MODE3;
			}else{
				pGPIOHandler->pGPIOx->CRL |= GPIO_CRL_MODE3_0;
			}
			//Se escoge el modo que se quiere poner el pin
			if(pGPIOHandler->GPIO_PinConfig.GPIO_PinOPType == GPIO_OTYPER_PUSHPULL){
				pGPIOHandler->pGPIOx->CRL &= ~GPIO_CRL_CNF3;
			}else if(pGPIOHandler->GPIO_PinConfig.GPIO_PinOPType == GPIO_OTYPER_OPENDRAIN){
				pGPIOHandler->pGPIOx->CRL |= GPIO_CRL_CNF3_0;
			}
		}else if(pGPIOHandler->GPIO_PinConfig.GPIO_PinMode == GPIO_MODE_AF){
			//Se va a escoger cual es la velocidad de salida del pin
			if(pGPIOHandler->GPIO_PinConfig.GPIO_PinSpeed == GPIO_OSPEED_10MHz){
				pGPIOHandler->pGPIOx->CRL |= GPIO_CRL_MODE3_0;
			}else if(pGPIOHandler->GPIO_PinConfig.GPIO_PinSpeed == GPIO_OSPEED_2MHz){
				pGPIOHandler->pGPIOx->CRL |= GPIO_CRL_MODE3_1;
			}else if(pGPIOHandler->GPIO_PinConfig.GPIO_PinSpeed == GPIO_OSPEED_50MHz){
				pGPIOHandler->pGPIOx->CRL |= GPIO_CRL_MODE3;
			}else{
				pGPIOHandler->pGPIOx->CRL |= GPIO_CRL_MODE3_0;
			}
			//Se escoge el modo que se quiere poner el pin
			if(pGPIOHandler->GPIO_PinConfig.GPIO_PinOPType == GPIO_OTYPER_PUSHPULL){
				pGPIOHandler->pGPIOx->CRL |= GPIO_CRL_CNF3_1;
				pGPIOHandler->pGPIOx->CRL &= ~GPIO_CRL_CNF3_0;
			}else if(pGPIOHandler->GPIO_PinConfig.GPIO_PinOPType == GPIO_OTYPER_OPENDRAIN){
				pGPIOHandler->pGPIOx->CRL |= GPIO_CRL_CNF3;
			}
		}
		break;
	case 4:
		if(pGPIOHandler->GPIO_PinConfig.GPIO_PinMode == GPIO_MODE_IN){
			//Se pone los registros del MODE en 00
			pGPIOHandler->pGPIOx->CRL &= ~GPIO_CRL_MODE4;
			//Se escoge el modo en el que se quiere el pin si analogo, flotante, pull-down o pull-up
			if(pGPIOHandler->GPIO_PinConfig.GPIO_PinPuPdControl == GPIO_PUPDR_ANALOG){
				pGPIOHandler->pGPIOx->CRL &= ~GPIO_CRL_CNF4;
			}else if(pGPIOHandler->GPIO_PinConfig.GPIO_PinPuPdControl == GPIO_PUPDR_FLOATING){
				pGPIOHandler->pGPIOx->CRL |= GPIO_CRL_CNF4_0;
			}else if(pGPIOHandler->GPIO_PinConfig.GPIO_PinPuPdControl == GPIO_PUPDR_PULLDOWN){
				pGPIOHandler->pGPIOx->CRL |= GPIO_CRL_CNF4_1;
			}else if(pGPIOHandler->GPIO_PinConfig.GPIO_PinPuPdControl == GPIO_PUPDR_PULLUP){
				pGPIOHandler->pGPIOx->CRL |= GPIO_CRL_CNF4_1;
			}
		}else if(pGPIOHandler->GPIO_PinConfig.GPIO_PinMode == GPIO_MODE_OUT){
			//Se va a escoger cual es la velocidad de salida del pin
			if(pGPIOHandler->GPIO_PinConfig.GPIO_PinSpeed == GPIO_OSPEED_10MHz){
				pGPIOHandler->pGPIOx->CRL |= GPIO_CRL_MODE4_0 ;
			}else if(pGPIOHandler->GPIO_PinConfig.GPIO_PinSpeed == GPIO_OSPEED_2MHz){
				pGPIOHandler->pGPIOx->CRL |= GPIO_CRL_MODE4_1;
			}else if(pGPIOHandler->GPIO_PinConfig.GPIO_PinSpeed == GPIO_OSPEED_50MHz){
				pGPIOHandler->pGPIOx->CRL |= GPIO_CRL_MODE4;
			}else{
				pGPIOHandler->pGPIOx->CRL |= GPIO_CRL_MODE4_0;
			}
			//Se escoge el modo que se quiere poner el pin
			if(pGPIOHandler->GPIO_PinConfig.GPIO_PinOPType == GPIO_OTYPER_PUSHPULL){
				pGPIOHandler->pGPIOx->CRL &= ~GPIO_CRL_CNF4;
			}else if(pGPIOHandler->GPIO_PinConfig.GPIO_PinOPType == GPIO_OTYPER_OPENDRAIN){
				pGPIOHandler->pGPIOx->CRL |= GPIO_CRL_CNF4_0;
			}
		}else if(pGPIOHandler->GPIO_PinConfig.GPIO_PinMode == GPIO_MODE_AF){
			//Se va a escoger cual es la velocidad de salida del pin
			if(pGPIOHandler->GPIO_PinConfig.GPIO_PinSpeed == GPIO_OSPEED_10MHz){
				pGPIOHandler->pGPIOx->CRL |= GPIO_CRL_MODE4_0;
			}else if(pGPIOHandler->GPIO_PinConfig.GPIO_PinSpeed == GPIO_OSPEED_2MHz){
				pGPIOHandler->pGPIOx->CRL |= GPIO_CRL_MODE4_1;
			}else if(pGPIOHandler->GPIO_PinConfig.GPIO_PinSpeed == GPIO_OSPEED_50MHz){
				pGPIOHandler->pGPIOx->CRL |= GPIO_CRL_MODE4;
			}else{
				pGPIOHandler->pGPIOx->CRL |= GPIO_CRL_MODE4_0;
			}
			//Se escoge el modo que se quiere poner el pin
			if(pGPIOHandler->GPIO_PinConfig.GPIO_PinOPType == GPIO_OTYPER_PUSHPULL){
				pGPIOHandler->pGPIOx->CRL |= GPIO_CRL_CNF4_1;
				pGPIOHandler->pGPIOx->CRL &= ~GPIO_CRL_CNF4_0;
			}else if(pGPIOHandler->GPIO_PinConfig.GPIO_PinOPType == GPIO_OTYPER_OPENDRAIN){
				pGPIOHandler->pGPIOx->CRL |= GPIO_CRL_CNF4;
			}
		}
		break;
	case 5:
		if(pGPIOHandler->GPIO_PinConfig.GPIO_PinMode == GPIO_MODE_IN){
			//Se pone los registros del MODE en 00
			pGPIOHandler->pGPIOx->CRL &= ~GPIO_CRL_MODE5;
			//Se escoge el modo en el que se quiere el pin si analogo, flotante, pull-down o pull-up
			if(pGPIOHandler->GPIO_PinConfig.GPIO_PinPuPdControl == GPIO_PUPDR_ANALOG){
				pGPIOHandler->pGPIOx->CRL &= ~GPIO_CRL_CNF5;
			}else if(pGPIOHandler->GPIO_PinConfig.GPIO_PinPuPdControl == GPIO_PUPDR_FLOATING){
				pGPIOHandler->pGPIOx->CRL |= GPIO_CRL_CNF5_0;
			}else if(pGPIOHandler->GPIO_PinConfig.GPIO_PinPuPdControl == GPIO_PUPDR_PULLDOWN){
				pGPIOHandler->pGPIOx->CRL |= GPIO_CRL_CNF5_1;
			}else if(pGPIOHandler->GPIO_PinConfig.GPIO_PinPuPdControl == GPIO_PUPDR_PULLUP){
				pGPIOHandler->pGPIOx->CRL |= GPIO_CRL_CNF5_1;
			}
		}else if(pGPIOHandler->GPIO_PinConfig.GPIO_PinMode == GPIO_MODE_OUT){
			//Se va a escoger cual es la velocidad de salida del pin
			if(pGPIOHandler->GPIO_PinConfig.GPIO_PinSpeed == GPIO_OSPEED_10MHz){
				pGPIOHandler->pGPIOx->CRL |= GPIO_CRL_MODE5_0 ;
			}else if(pGPIOHandler->GPIO_PinConfig.GPIO_PinSpeed == GPIO_OSPEED_2MHz){
				pGPIOHandler->pGPIOx->CRL |= GPIO_CRL_MODE5_1;
			}else if(pGPIOHandler->GPIO_PinConfig.GPIO_PinSpeed == GPIO_OSPEED_50MHz){
				pGPIOHandler->pGPIOx->CRL |= GPIO_CRL_MODE5;
			}else{
				pGPIOHandler->pGPIOx->CRL |= GPIO_CRL_MODE5_0;
			}
			//Se escoge el modo que se quiere poner el pin
			if(pGPIOHandler->GPIO_PinConfig.GPIO_PinOPType == GPIO_OTYPER_PUSHPULL){
				pGPIOHandler->pGPIOx->CRL &= ~GPIO_CRL_CNF5;
			}else if(pGPIOHandler->GPIO_PinConfig.GPIO_PinOPType == GPIO_OTYPER_OPENDRAIN){
				pGPIOHandler->pGPIOx->CRL |= GPIO_CRL_CNF5_0;
			}
		}else if(pGPIOHandler->GPIO_PinConfig.GPIO_PinMode == GPIO_MODE_AF){
			//Se va a escoger cual es la velocidad de salida del pin
			if(pGPIOHandler->GPIO_PinConfig.GPIO_PinSpeed == GPIO_OSPEED_10MHz){
				pGPIOHandler->pGPIOx->CRL |= GPIO_CRL_MODE5_0;
			}else if(pGPIOHandler->GPIO_PinConfig.GPIO_PinSpeed == GPIO_OSPEED_2MHz){
				pGPIOHandler->pGPIOx->CRL |= GPIO_CRL_MODE5_1;
			}else if(pGPIOHandler->GPIO_PinConfig.GPIO_PinSpeed == GPIO_OSPEED_50MHz){
				pGPIOHandler->pGPIOx->CRL |= GPIO_CRL_MODE5;
			}else{
				pGPIOHandler->pGPIOx->CRL |= GPIO_CRL_MODE5_0;
			}
			//Se escoge el modo que se quiere poner el pin
			if(pGPIOHandler->GPIO_PinConfig.GPIO_PinOPType == GPIO_OTYPER_PUSHPULL){
				pGPIOHandler->pGPIOx->CRL |= GPIO_CRL_CNF5_1;
				pGPIOHandler->pGPIOx->CRL &= ~GPIO_CRL_CNF5_0;
			}else if(pGPIOHandler->GPIO_PinConfig.GPIO_PinOPType == GPIO_OTYPER_OPENDRAIN){
				pGPIOHandler->pGPIOx->CRL |= GPIO_CRL_CNF5;
			}
		}
		break;
	case 6:
		if(pGPIOHandler->GPIO_PinConfig.GPIO_PinMode == GPIO_MODE_IN){
			//Se pone los registros del MODE en 00
			pGPIOHandler->pGPIOx->CRL &= ~GPIO_CRL_MODE6;
			//Se escoge el modo en el que se quiere el pin si analogo, flotante, pull-down o pull-up
			if(pGPIOHandler->GPIO_PinConfig.GPIO_PinPuPdControl == GPIO_PUPDR_ANALOG){
				pGPIOHandler->pGPIOx->CRL &= ~GPIO_CRL_CNF6;
			}else if(pGPIOHandler->GPIO_PinConfig.GPIO_PinPuPdControl == GPIO_PUPDR_FLOATING){
				pGPIOHandler->pGPIOx->CRL |= GPIO_CRL_CNF6_0;
			}else if(pGPIOHandler->GPIO_PinConfig.GPIO_PinPuPdControl == GPIO_PUPDR_PULLDOWN){
				pGPIOHandler->pGPIOx->CRL |= GPIO_CRL_CNF6_1;
			}else if(pGPIOHandler->GPIO_PinConfig.GPIO_PinPuPdControl == GPIO_PUPDR_PULLUP){
				pGPIOHandler->pGPIOx->CRL |= GPIO_CRL_CNF6_1;
			}
		}else if(pGPIOHandler->GPIO_PinConfig.GPIO_PinMode == GPIO_MODE_OUT){
			//Se va a escoger cual es la velocidad de salida del pin
			if(pGPIOHandler->GPIO_PinConfig.GPIO_PinSpeed == GPIO_OSPEED_10MHz){
				pGPIOHandler->pGPIOx->CRL |= GPIO_CRL_MODE6_0 ;
			}else if(pGPIOHandler->GPIO_PinConfig.GPIO_PinSpeed == GPIO_OSPEED_2MHz){
				pGPIOHandler->pGPIOx->CRL |= GPIO_CRL_MODE6_1;
			}else if(pGPIOHandler->GPIO_PinConfig.GPIO_PinSpeed == GPIO_OSPEED_50MHz){
				pGPIOHandler->pGPIOx->CRL |= GPIO_CRL_MODE6;
			}else{
				pGPIOHandler->pGPIOx->CRL |= GPIO_CRL_MODE6_0;
			}
			//Se escoge el modo que se quiere poner el pin
			if(pGPIOHandler->GPIO_PinConfig.GPIO_PinOPType == GPIO_OTYPER_PUSHPULL){
				pGPIOHandler->pGPIOx->CRL &= ~GPIO_CRL_CNF6;
			}else if(pGPIOHandler->GPIO_PinConfig.GPIO_PinOPType == GPIO_OTYPER_OPENDRAIN){
				pGPIOHandler->pGPIOx->CRL |= GPIO_CRL_CNF6_0;
			}
		}else if(pGPIOHandler->GPIO_PinConfig.GPIO_PinMode == GPIO_MODE_AF){
			//Se va a escoger cual es la velocidad de salida del pin
			if(pGPIOHandler->GPIO_PinConfig.GPIO_PinSpeed == GPIO_OSPEED_10MHz){
				pGPIOHandler->pGPIOx->CRL |= GPIO_CRL_MODE6_0;
			}else if(pGPIOHandler->GPIO_PinConfig.GPIO_PinSpeed == GPIO_OSPEED_2MHz){
				pGPIOHandler->pGPIOx->CRL |= GPIO_CRL_MODE6_1;
			}else if(pGPIOHandler->GPIO_PinConfig.GPIO_PinSpeed == GPIO_OSPEED_50MHz){
				pGPIOHandler->pGPIOx->CRL |= GPIO_CRL_MODE6;
			}else{
				pGPIOHandler->pGPIOx->CRL |= GPIO_CRL_MODE6_0;
			}
			//Se escoge el modo que se quiere poner el pin
			if(pGPIOHandler->GPIO_PinConfig.GPIO_PinOPType == GPIO_OTYPER_PUSHPULL){
				pGPIOHandler->pGPIOx->CRL |= GPIO_CRL_CNF6_1;
				pGPIOHandler->pGPIOx->CRL &= ~GPIO_CRL_CNF6_0;
			}else if(pGPIOHandler->GPIO_PinConfig.GPIO_PinOPType == GPIO_OTYPER_OPENDRAIN){
				pGPIOHandler->pGPIOx->CRL |= GPIO_CRL_CNF6;
			}
		}
		break;
	case 7:
		if(pGPIOHandler->GPIO_PinConfig.GPIO_PinMode == GPIO_MODE_IN){
			//Se pone los registros del MODE en 00
			pGPIOHandler->pGPIOx->CRL &= ~GPIO_CRL_MODE7;
			//Se escoge el modo en el que se quiere el pin si analogo, flotante, pull-down o pull-up
			if(pGPIOHandler->GPIO_PinConfig.GPIO_PinPuPdControl == GPIO_PUPDR_ANALOG){
				pGPIOHandler->pGPIOx->CRL &= ~GPIO_CRL_CNF7;
			}else if(pGPIOHandler->GPIO_PinConfig.GPIO_PinPuPdControl == GPIO_PUPDR_FLOATING){
				pGPIOHandler->pGPIOx->CRL |= GPIO_CRL_CNF7_0;
			}else if(pGPIOHandler->GPIO_PinConfig.GPIO_PinPuPdControl == GPIO_PUPDR_PULLDOWN){
				pGPIOHandler->pGPIOx->CRL |= GPIO_CRL_CNF7_1;
			}else if(pGPIOHandler->GPIO_PinConfig.GPIO_PinPuPdControl == GPIO_PUPDR_PULLUP){
				pGPIOHandler->pGPIOx->CRL |= GPIO_CRL_CNF7_1;
			}
		}else if(pGPIOHandler->GPIO_PinConfig.GPIO_PinMode == GPIO_MODE_OUT){
			//Se va a escoger cual es la velocidad de salida del pin
			if(pGPIOHandler->GPIO_PinConfig.GPIO_PinSpeed == GPIO_OSPEED_10MHz){
				pGPIOHandler->pGPIOx->CRL |= GPIO_CRL_MODE7_0 ;
			}else if(pGPIOHandler->GPIO_PinConfig.GPIO_PinSpeed == GPIO_OSPEED_2MHz){
				pGPIOHandler->pGPIOx->CRL |= GPIO_CRL_MODE7_1;
			}else if(pGPIOHandler->GPIO_PinConfig.GPIO_PinSpeed == GPIO_OSPEED_50MHz){
				pGPIOHandler->pGPIOx->CRL |= GPIO_CRL_MODE7;
			}else{
				pGPIOHandler->pGPIOx->CRL |= GPIO_CRL_MODE7_0;
			}
			//Se escoge el modo que se quiere poner el pin
			if(pGPIOHandler->GPIO_PinConfig.GPIO_PinOPType == GPIO_OTYPER_PUSHPULL){
				pGPIOHandler->pGPIOx->CRL &= ~GPIO_CRL_CNF7;
			}else if(pGPIOHandler->GPIO_PinConfig.GPIO_PinOPType == GPIO_OTYPER_OPENDRAIN){
				pGPIOHandler->pGPIOx->CRL |= GPIO_CRL_CNF7_0;
			}
		}else if(pGPIOHandler->GPIO_PinConfig.GPIO_PinMode == GPIO_MODE_AF){
			//Se va a escoger cual es la velocidad de salida del pin
			if(pGPIOHandler->GPIO_PinConfig.GPIO_PinSpeed == GPIO_OSPEED_10MHz){
				pGPIOHandler->pGPIOx->CRL |= GPIO_CRL_MODE7_0;
			}else if(pGPIOHandler->GPIO_PinConfig.GPIO_PinSpeed == GPIO_OSPEED_2MHz){
				pGPIOHandler->pGPIOx->CRL |= GPIO_CRL_MODE7_1;
			}else if(pGPIOHandler->GPIO_PinConfig.GPIO_PinSpeed == GPIO_OSPEED_50MHz){
				pGPIOHandler->pGPIOx->CRL |= GPIO_CRL_MODE0;
			}else{
				pGPIOHandler->pGPIOx->CRL |= GPIO_CRL_MODE7_0;
			}
			//Se escoge el modo que se quiere poner el pin
			if(pGPIOHandler->GPIO_PinConfig.GPIO_PinOPType == GPIO_OTYPER_PUSHPULL){
				pGPIOHandler->pGPIOx->CRL |= GPIO_CRL_CNF7_1;
				pGPIOHandler->pGPIOx->CRL &= ~GPIO_CRL_CNF7_0;
			}else if(pGPIOHandler->GPIO_PinConfig.GPIO_PinOPType == GPIO_OTYPER_OPENDRAIN){
				pGPIOHandler->pGPIOx->CRL |= GPIO_CRL_CNF7;
			}
		}
		break;
	case 8:
		if(pGPIOHandler->GPIO_PinConfig.GPIO_PinMode == GPIO_MODE_IN){
			//Se pone los registros del MODE en 00
			pGPIOHandler->pGPIOx->CRH &= ~GPIO_CRH_MODE8;
			//Se escoge el modo en el que se quiere el pin si analogo, flotante, pull-down o pull-up
			if(pGPIOHandler->GPIO_PinConfig.GPIO_PinPuPdControl == GPIO_PUPDR_ANALOG){
				pGPIOHandler->pGPIOx->CRH &= ~GPIO_CRH_CNF8;
			}else if(pGPIOHandler->GPIO_PinConfig.GPIO_PinPuPdControl == GPIO_PUPDR_FLOATING){
				pGPIOHandler->pGPIOx->CRH |= GPIO_CRH_CNF8_0;
			}else if(pGPIOHandler->GPIO_PinConfig.GPIO_PinPuPdControl == GPIO_PUPDR_PULLDOWN){
				pGPIOHandler->pGPIOx->CRH |= GPIO_CRH_CNF8_1;
			}else if(pGPIOHandler->GPIO_PinConfig.GPIO_PinPuPdControl == GPIO_PUPDR_PULLUP){
				pGPIOHandler->pGPIOx->CRH |= GPIO_CRH_CNF8_1;
			}
		}else if(pGPIOHandler->GPIO_PinConfig.GPIO_PinMode == GPIO_MODE_OUT){
			//Se va a escoger cual es la velocidad de salida del pin
			if(pGPIOHandler->GPIO_PinConfig.GPIO_PinSpeed == GPIO_OSPEED_10MHz){
				pGPIOHandler->pGPIOx->CRH |= GPIO_CRH_MODE8_0;
			}else if(pGPIOHandler->GPIO_PinConfig.GPIO_PinSpeed == GPIO_OSPEED_2MHz){
				pGPIOHandler->pGPIOx->CRH |= GPIO_CRH_MODE8_1;
			}else if(pGPIOHandler->GPIO_PinConfig.GPIO_PinSpeed == GPIO_OSPEED_50MHz){
				pGPIOHandler->pGPIOx->CRH |= GPIO_CRH_MODE8;
			}else{
				pGPIOHandler->pGPIOx->CRH |= GPIO_CRH_MODE8_0;
			}
			//Se escoge el modo que se quiere poner el pin
			if(pGPIOHandler->GPIO_PinConfig.GPIO_PinOPType == GPIO_OTYPER_PUSHPULL){
				pGPIOHandler->pGPIOx->CRH &= ~GPIO_CRH_CNF8;
			}else if(pGPIOHandler->GPIO_PinConfig.GPIO_PinOPType == GPIO_OTYPER_OPENDRAIN){
				pGPIOHandler->pGPIOx->CRH |= GPIO_CRH_CNF8_0;
			}
		}else if(pGPIOHandler->GPIO_PinConfig.GPIO_PinMode == GPIO_MODE_AF){
			//Se va a escoger cual es la velocidad de salida del pin
			if(pGPIOHandler->GPIO_PinConfig.GPIO_PinSpeed == GPIO_OSPEED_10MHz){
				pGPIOHandler->pGPIOx->CRH |= GPIO_CRH_MODE8_0;
			}else if(pGPIOHandler->GPIO_PinConfig.GPIO_PinSpeed == GPIO_OSPEED_2MHz){
				pGPIOHandler->pGPIOx->CRH |= GPIO_CRH_MODE8_1;
			}else if(pGPIOHandler->GPIO_PinConfig.GPIO_PinSpeed == GPIO_OSPEED_50MHz){
				pGPIOHandler->pGPIOx->CRH |= GPIO_CRH_MODE8;
			}else{
				pGPIOHandler->pGPIOx->CRH |= GPIO_CRH_MODE8_0;
			}
			//Se escoge el modo que se quiere poner el pin
			if(pGPIOHandler->GPIO_PinConfig.GPIO_PinOPType == GPIO_OTYPER_PUSHPULL){
				pGPIOHandler->pGPIOx->CRH |= GPIO_CRH_CNF8_1;
				pGPIOHandler->pGPIOx->CRH &= ~GPIO_CRH_CNF8_0;
			}else if(pGPIOHandler->GPIO_PinConfig.GPIO_PinOPType == GPIO_OTYPER_OPENDRAIN){
				pGPIOHandler->pGPIOx->CRH |= GPIO_CRH_CNF8;
			}
		}
		break;
	case 9:
		if(pGPIOHandler->GPIO_PinConfig.GPIO_PinMode == GPIO_MODE_IN){
			//Se pone los registros del MODE en 00
			pGPIOHandler->pGPIOx->CRH &= ~GPIO_CRH_MODE9;
			//Se escoge el modo en el que se quiere el pin si analogo, flotante, pull-down o pull-up
			if(pGPIOHandler->GPIO_PinConfig.GPIO_PinPuPdControl == GPIO_PUPDR_ANALOG){
				pGPIOHandler->pGPIOx->CRH &= ~GPIO_CRH_CNF9;
			}else if(pGPIOHandler->GPIO_PinConfig.GPIO_PinPuPdControl == GPIO_PUPDR_FLOATING){
				pGPIOHandler->pGPIOx->CRH |= GPIO_CRH_CNF9_0;
			}else if(pGPIOHandler->GPIO_PinConfig.GPIO_PinPuPdControl == GPIO_PUPDR_PULLDOWN){
				pGPIOHandler->pGPIOx->CRH |= GPIO_CRH_CNF9_1;
			}else if(pGPIOHandler->GPIO_PinConfig.GPIO_PinPuPdControl == GPIO_PUPDR_PULLUP){
				pGPIOHandler->pGPIOx->CRH |= GPIO_CRH_CNF9_1;
			}
		}else if(pGPIOHandler->GPIO_PinConfig.GPIO_PinMode == GPIO_MODE_OUT){
			//Se va a escoger cual es la velocidad de salida del pin
			if(pGPIOHandler->GPIO_PinConfig.GPIO_PinSpeed == GPIO_OSPEED_10MHz){
				pGPIOHandler->pGPIOx->CRH |= GPIO_CRH_MODE9_0;
			}else if(pGPIOHandler->GPIO_PinConfig.GPIO_PinSpeed == GPIO_OSPEED_2MHz){
				pGPIOHandler->pGPIOx->CRH |= GPIO_CRH_MODE9_1;
			}else if(pGPIOHandler->GPIO_PinConfig.GPIO_PinSpeed == GPIO_OSPEED_50MHz){
				pGPIOHandler->pGPIOx->CRH |= GPIO_CRH_MODE9;
			}else{
				pGPIOHandler->pGPIOx->CRH |= GPIO_CRH_MODE9_0;
			}
			//Se escoge el modo que se quiere poner el pin
			if(pGPIOHandler->GPIO_PinConfig.GPIO_PinOPType == GPIO_OTYPER_PUSHPULL){
				pGPIOHandler->pGPIOx->CRH &= ~GPIO_CRH_CNF9;
			}else if(pGPIOHandler->GPIO_PinConfig.GPIO_PinOPType == GPIO_OTYPER_OPENDRAIN){
				pGPIOHandler->pGPIOx->CRH |= GPIO_CRH_CNF9_0;
			}
		}else if(pGPIOHandler->GPIO_PinConfig.GPIO_PinMode == GPIO_MODE_AF){
			//Se va a escoger cual es la velocidad de salida del pin
			if(pGPIOHandler->GPIO_PinConfig.GPIO_PinSpeed == GPIO_OSPEED_10MHz){
				pGPIOHandler->pGPIOx->CRH |= GPIO_CRH_MODE9_0;
			}else if(pGPIOHandler->GPIO_PinConfig.GPIO_PinSpeed == GPIO_OSPEED_2MHz){
				pGPIOHandler->pGPIOx->CRH |= GPIO_CRH_MODE9_1;
			}else if(pGPIOHandler->GPIO_PinConfig.GPIO_PinSpeed == GPIO_OSPEED_50MHz){
				pGPIOHandler->pGPIOx->CRH |= GPIO_CRH_MODE9;
			}else{
				pGPIOHandler->pGPIOx->CRH |= GPIO_CRH_MODE9_0;
			}
			//Se escoge el modo que se quiere poner el pin
			if(pGPIOHandler->GPIO_PinConfig.GPIO_PinOPType == GPIO_OTYPER_PUSHPULL){
				pGPIOHandler->pGPIOx->CRH |= GPIO_CRH_CNF9_1;
				pGPIOHandler->pGPIOx->CRH &= ~GPIO_CRH_CNF9_0;
			}else if(pGPIOHandler->GPIO_PinConfig.GPIO_PinOPType == GPIO_OTYPER_OPENDRAIN){
				pGPIOHandler->pGPIOx->CRH |= GPIO_CRH_CNF9;
			}
		}
		break;
	case 10:
		if(pGPIOHandler->GPIO_PinConfig.GPIO_PinMode == GPIO_MODE_IN){
			//Se pone los registros del MODE en 00
			pGPIOHandler->pGPIOx->CRH &= ~GPIO_CRH_MODE10;
			//Se escoge el modo en el que se quiere el pin si analogo, flotante, pull-down o pull-up
			if(pGPIOHandler->GPIO_PinConfig.GPIO_PinPuPdControl == GPIO_PUPDR_ANALOG){
				pGPIOHandler->pGPIOx->CRH &= ~GPIO_CRH_CNF10;
			}else if(pGPIOHandler->GPIO_PinConfig.GPIO_PinPuPdControl == GPIO_PUPDR_FLOATING){
				pGPIOHandler->pGPIOx->CRH |= GPIO_CRH_CNF10_0;
			}else if(pGPIOHandler->GPIO_PinConfig.GPIO_PinPuPdControl == GPIO_PUPDR_PULLDOWN){
				pGPIOHandler->pGPIOx->CRH |= GPIO_CRH_CNF10_1;
			}else if(pGPIOHandler->GPIO_PinConfig.GPIO_PinPuPdControl == GPIO_PUPDR_PULLUP){
				pGPIOHandler->pGPIOx->CRH |= GPIO_CRH_CNF10_1;
			}
		}else if(pGPIOHandler->GPIO_PinConfig.GPIO_PinMode == GPIO_MODE_OUT){
			//Se va a escoger cual es la velocidad de salida del pin
			if(pGPIOHandler->GPIO_PinConfig.GPIO_PinSpeed == GPIO_OSPEED_10MHz){
				pGPIOHandler->pGPIOx->CRH |= GPIO_CRH_MODE10_0;
			}else if(pGPIOHandler->GPIO_PinConfig.GPIO_PinSpeed == GPIO_OSPEED_2MHz){
				pGPIOHandler->pGPIOx->CRH |= GPIO_CRH_MODE10_1;
			}else if(pGPIOHandler->GPIO_PinConfig.GPIO_PinSpeed == GPIO_OSPEED_50MHz){
				pGPIOHandler->pGPIOx->CRH |= GPIO_CRH_MODE10;
			}else{
				pGPIOHandler->pGPIOx->CRH |= GPIO_CRH_MODE10_0;
			}
			//Se escoge el modo que se quiere poner el pin
			if(pGPIOHandler->GPIO_PinConfig.GPIO_PinOPType == GPIO_OTYPER_PUSHPULL){
				pGPIOHandler->pGPIOx->CRH &= ~GPIO_CRH_CNF10;
			}else if(pGPIOHandler->GPIO_PinConfig.GPIO_PinOPType == GPIO_OTYPER_OPENDRAIN){
				pGPIOHandler->pGPIOx->CRH |= GPIO_CRH_CNF10_0;
			}
		}else if(pGPIOHandler->GPIO_PinConfig.GPIO_PinMode == GPIO_MODE_AF){
			//Se va a escoger cual es la velocidad de salida del pin
			if(pGPIOHandler->GPIO_PinConfig.GPIO_PinSpeed == GPIO_OSPEED_10MHz){
				pGPIOHandler->pGPIOx->CRH |= GPIO_CRH_MODE10_0;
			}else if(pGPIOHandler->GPIO_PinConfig.GPIO_PinSpeed == GPIO_OSPEED_2MHz){
				pGPIOHandler->pGPIOx->CRH |= GPIO_CRH_MODE10_1;
			}else if(pGPIOHandler->GPIO_PinConfig.GPIO_PinSpeed == GPIO_OSPEED_50MHz){
				pGPIOHandler->pGPIOx->CRH |= GPIO_CRH_MODE10;
			}else{
				pGPIOHandler->pGPIOx->CRH |= GPIO_CRH_MODE10_0;
			}
			//Se escoge el modo que se quiere poner el pin
			if(pGPIOHandler->GPIO_PinConfig.GPIO_PinOPType == GPIO_OTYPER_PUSHPULL){
				pGPIOHandler->pGPIOx->CRH |= GPIO_CRH_CNF10_1;
				pGPIOHandler->pGPIOx->CRH &= ~GPIO_CRH_CNF10_0;
			}else if(pGPIOHandler->GPIO_PinConfig.GPIO_PinOPType == GPIO_OTYPER_OPENDRAIN){
				pGPIOHandler->pGPIOx->CRH |= GPIO_CRH_CNF10;
			}
		}
		break;
	case 11:
		if(pGPIOHandler->GPIO_PinConfig.GPIO_PinMode == GPIO_MODE_IN){
			//Se pone los registros del MODE en 00
			pGPIOHandler->pGPIOx->CRH &= ~GPIO_CRH_MODE11;
			//Se escoge el modo en el que se quiere el pin si analogo, flotante, pull-down o pull-up
			if(pGPIOHandler->GPIO_PinConfig.GPIO_PinPuPdControl == GPIO_PUPDR_ANALOG){
				pGPIOHandler->pGPIOx->CRH &= ~GPIO_CRH_CNF11;
			}else if(pGPIOHandler->GPIO_PinConfig.GPIO_PinPuPdControl == GPIO_PUPDR_FLOATING){
				pGPIOHandler->pGPIOx->CRH |= GPIO_CRH_CNF11_0;
			}else if(pGPIOHandler->GPIO_PinConfig.GPIO_PinPuPdControl == GPIO_PUPDR_PULLDOWN){
				pGPIOHandler->pGPIOx->CRH |= GPIO_CRH_CNF11_1;
			}else if(pGPIOHandler->GPIO_PinConfig.GPIO_PinPuPdControl == GPIO_PUPDR_PULLUP){
				pGPIOHandler->pGPIOx->CRH |= GPIO_CRH_CNF11_1;
			}
		}else if(pGPIOHandler->GPIO_PinConfig.GPIO_PinMode == GPIO_MODE_OUT){
			//Se va a escoger cual es la velocidad de salida del pin
			if(pGPIOHandler->GPIO_PinConfig.GPIO_PinSpeed == GPIO_OSPEED_10MHz){
				pGPIOHandler->pGPIOx->CRH |= GPIO_CRH_MODE11_0;
			}else if(pGPIOHandler->GPIO_PinConfig.GPIO_PinSpeed == GPIO_OSPEED_2MHz){
				pGPIOHandler->pGPIOx->CRH |= GPIO_CRH_MODE11_1;
			}else if(pGPIOHandler->GPIO_PinConfig.GPIO_PinSpeed == GPIO_OSPEED_50MHz){
				pGPIOHandler->pGPIOx->CRH |= GPIO_CRH_MODE11;
			}else{
				pGPIOHandler->pGPIOx->CRH |= GPIO_CRH_MODE11_0;
			}
			//Se escoge el modo que se quiere poner el pin
			if(pGPIOHandler->GPIO_PinConfig.GPIO_PinOPType == GPIO_OTYPER_PUSHPULL){
				pGPIOHandler->pGPIOx->CRH &= ~GPIO_CRH_CNF11;
			}else if(pGPIOHandler->GPIO_PinConfig.GPIO_PinOPType == GPIO_OTYPER_OPENDRAIN){
				pGPIOHandler->pGPIOx->CRH |= GPIO_CRH_CNF11_0;
			}
		}else if(pGPIOHandler->GPIO_PinConfig.GPIO_PinMode == GPIO_MODE_AF){
			//Se va a escoger cual es la velocidad de salida del pin
			if(pGPIOHandler->GPIO_PinConfig.GPIO_PinSpeed == GPIO_OSPEED_10MHz){
				pGPIOHandler->pGPIOx->CRH |= GPIO_CRH_MODE11_0;
			}else if(pGPIOHandler->GPIO_PinConfig.GPIO_PinSpeed == GPIO_OSPEED_2MHz){
				pGPIOHandler->pGPIOx->CRH |= GPIO_CRH_MODE11_1;
			}else if(pGPIOHandler->GPIO_PinConfig.GPIO_PinSpeed == GPIO_OSPEED_50MHz){
				pGPIOHandler->pGPIOx->CRH |= GPIO_CRH_MODE11;
			}else{
				pGPIOHandler->pGPIOx->CRH |= GPIO_CRH_MODE11_0;
			}
			//Se escoge el modo que se quiere poner el pin
			if(pGPIOHandler->GPIO_PinConfig.GPIO_PinOPType == GPIO_OTYPER_PUSHPULL){
				pGPIOHandler->pGPIOx->CRH |= GPIO_CRH_CNF11_1;
				pGPIOHandler->pGPIOx->CRH &= ~GPIO_CRH_CNF11_0;
			}else if(pGPIOHandler->GPIO_PinConfig.GPIO_PinOPType == GPIO_OTYPER_OPENDRAIN){
				pGPIOHandler->pGPIOx->CRH |= GPIO_CRH_CNF11;
			}
		}
		break;
	case 12:
		if(pGPIOHandler->GPIO_PinConfig.GPIO_PinMode == GPIO_MODE_IN){
			//Se pone los registros del MODE en 00
			pGPIOHandler->pGPIOx->CRH &= ~GPIO_CRH_MODE12;
			//Se escoge el modo en el que se quiere el pin si analogo, flotante, pull-down o pull-up
			if(pGPIOHandler->GPIO_PinConfig.GPIO_PinPuPdControl == GPIO_PUPDR_ANALOG){
				pGPIOHandler->pGPIOx->CRH &= ~GPIO_CRH_CNF12;
			}else if(pGPIOHandler->GPIO_PinConfig.GPIO_PinPuPdControl == GPIO_PUPDR_FLOATING){
				pGPIOHandler->pGPIOx->CRH |= GPIO_CRH_CNF12_0;
			}else if(pGPIOHandler->GPIO_PinConfig.GPIO_PinPuPdControl == GPIO_PUPDR_PULLDOWN){
				pGPIOHandler->pGPIOx->CRH |= GPIO_CRH_CNF12_1;
			}else if(pGPIOHandler->GPIO_PinConfig.GPIO_PinPuPdControl == GPIO_PUPDR_PULLUP){
				pGPIOHandler->pGPIOx->CRH |= GPIO_CRH_CNF12_1;
			}
		}else if(pGPIOHandler->GPIO_PinConfig.GPIO_PinMode == GPIO_MODE_OUT){
			//Se va a escoger cual es la velocidad de salida del pin
			if(pGPIOHandler->GPIO_PinConfig.GPIO_PinSpeed == GPIO_OSPEED_10MHz){
				pGPIOHandler->pGPIOx->CRH |= GPIO_CRH_MODE12_0;
			}else if(pGPIOHandler->GPIO_PinConfig.GPIO_PinSpeed == GPIO_OSPEED_2MHz){
				pGPIOHandler->pGPIOx->CRH |= GPIO_CRH_MODE12_1;
			}else if(pGPIOHandler->GPIO_PinConfig.GPIO_PinSpeed == GPIO_OSPEED_50MHz){
				pGPIOHandler->pGPIOx->CRH |= GPIO_CRH_MODE12;
			}else{
				pGPIOHandler->pGPIOx->CRH |= GPIO_CRH_MODE12_0;
			}
			//Se escoge el modo que se quiere poner el pin
			if(pGPIOHandler->GPIO_PinConfig.GPIO_PinOPType == GPIO_OTYPER_PUSHPULL){
				pGPIOHandler->pGPIOx->CRH &= ~GPIO_CRH_CNF12;
			}else if(pGPIOHandler->GPIO_PinConfig.GPIO_PinOPType == GPIO_OTYPER_OPENDRAIN){
				pGPIOHandler->pGPIOx->CRH |= GPIO_CRH_CNF12_0;
			}
		}else if(pGPIOHandler->GPIO_PinConfig.GPIO_PinMode == GPIO_MODE_AF){
			//Se va a escoger cual es la velocidad de salida del pin
			if(pGPIOHandler->GPIO_PinConfig.GPIO_PinSpeed == GPIO_OSPEED_10MHz){
				pGPIOHandler->pGPIOx->CRH |= GPIO_CRH_MODE12_0;
			}else if(pGPIOHandler->GPIO_PinConfig.GPIO_PinSpeed == GPIO_OSPEED_2MHz){
				pGPIOHandler->pGPIOx->CRH |= GPIO_CRH_MODE12_1;
			}else if(pGPIOHandler->GPIO_PinConfig.GPIO_PinSpeed == GPIO_OSPEED_50MHz){
				pGPIOHandler->pGPIOx->CRH |= GPIO_CRH_MODE12;
			}else{
				pGPIOHandler->pGPIOx->CRH |= GPIO_CRH_MODE12_0;
			}
			//Se escoge el modo que se quiere poner el pin
			if(pGPIOHandler->GPIO_PinConfig.GPIO_PinOPType == GPIO_OTYPER_PUSHPULL){
				pGPIOHandler->pGPIOx->CRH |= GPIO_CRH_CNF12_1;
				pGPIOHandler->pGPIOx->CRH &= ~GPIO_CRH_CNF12_0;
			}else if(pGPIOHandler->GPIO_PinConfig.GPIO_PinOPType == GPIO_OTYPER_OPENDRAIN){
				pGPIOHandler->pGPIOx->CRH |= GPIO_CRH_CNF12;
			}
		}
		break;
		case 13:
			if(pGPIOHandler->GPIO_PinConfig.GPIO_PinMode == GPIO_MODE_IN){
				//Se pone los registros del MODE en 00
				pGPIOHandler->pGPIOx->CRH &= ~GPIO_CRH_MODE13;
				//Se escoge el modo en el que se quiere el pin si analogo, flotante, pull-down o pull-up
				if(pGPIOHandler->GPIO_PinConfig.GPIO_PinPuPdControl == GPIO_PUPDR_ANALOG){
					pGPIOHandler->pGPIOx->CRH &= ~GPIO_CRH_CNF13;
				}else if(pGPIOHandler->GPIO_PinConfig.GPIO_PinPuPdControl == GPIO_PUPDR_FLOATING){
					pGPIOHandler->pGPIOx->CRH |= GPIO_CRH_CNF13_0;
				}else if(pGPIOHandler->GPIO_PinConfig.GPIO_PinPuPdControl == GPIO_PUPDR_PULLDOWN){
					pGPIOHandler->pGPIOx->CRH |= GPIO_CRH_CNF13_1;
				}else if(pGPIOHandler->GPIO_PinConfig.GPIO_PinPuPdControl == GPIO_PUPDR_PULLUP){
					pGPIOHandler->pGPIOx->CRH |= GPIO_CRH_CNF13_1;
				}
			}else if(pGPIOHandler->GPIO_PinConfig.GPIO_PinMode == GPIO_MODE_OUT){
				//Se va a escoger cual es la velocidad de salida del pin
				if(pGPIOHandler->GPIO_PinConfig.GPIO_PinSpeed == GPIO_OSPEED_10MHz){
					pGPIOHandler->pGPIOx->CRH |= GPIO_CRH_MODE13_0;
				}else if(pGPIOHandler->GPIO_PinConfig.GPIO_PinSpeed == GPIO_OSPEED_2MHz){
					pGPIOHandler->pGPIOx->CRH |= GPIO_CRH_MODE13_1;
				}else if(pGPIOHandler->GPIO_PinConfig.GPIO_PinSpeed == GPIO_OSPEED_50MHz){
					pGPIOHandler->pGPIOx->CRH |= GPIO_CRH_MODE13;
				}else{
					pGPIOHandler->pGPIOx->CRH |= GPIO_CRH_MODE13_0;
				}
				//Se escoge el modo que se quiere poner el pin
				if(pGPIOHandler->GPIO_PinConfig.GPIO_PinOPType == GPIO_OTYPER_PUSHPULL){
					pGPIOHandler->pGPIOx->CRH &= ~GPIO_CRH_CNF13;
				}else if(pGPIOHandler->GPIO_PinConfig.GPIO_PinOPType == GPIO_OTYPER_OPENDRAIN){
					pGPIOHandler->pGPIOx->CRH |= GPIO_CRH_CNF13_0;
				}
			}else if(pGPIOHandler->GPIO_PinConfig.GPIO_PinMode == GPIO_MODE_AF){
				//Se va a escoger cual es la velocidad de salida del pin
				if(pGPIOHandler->GPIO_PinConfig.GPIO_PinSpeed == GPIO_OSPEED_10MHz){
					pGPIOHandler->pGPIOx->CRH |= GPIO_CRH_MODE13_0;
				}else if(pGPIOHandler->GPIO_PinConfig.GPIO_PinSpeed == GPIO_OSPEED_2MHz){
					pGPIOHandler->pGPIOx->CRH |= GPIO_CRH_MODE13_1;
				}else if(pGPIOHandler->GPIO_PinConfig.GPIO_PinSpeed == GPIO_OSPEED_50MHz){
					pGPIOHandler->pGPIOx->CRH |= GPIO_CRH_MODE13;
				}else{
					pGPIOHandler->pGPIOx->CRH |= GPIO_CRH_MODE13_0;
				}
				//Se escoge el modo que se quiere poner el pin
				if(pGPIOHandler->GPIO_PinConfig.GPIO_PinOPType == GPIO_OTYPER_PUSHPULL){
					pGPIOHandler->pGPIOx->CRH |= GPIO_CRH_CNF13_1;
					pGPIOHandler->pGPIOx->CRH &= ~GPIO_CRH_CNF13_0;
				}else if(pGPIOHandler->GPIO_PinConfig.GPIO_PinOPType == GPIO_OTYPER_OPENDRAIN){
					pGPIOHandler->pGPIOx->CRH |= GPIO_CRH_CNF13;
				}
			}
		case 14:
			if(pGPIOHandler->GPIO_PinConfig.GPIO_PinMode == GPIO_MODE_IN){
				//Se pone los registros del MODE en 00
				pGPIOHandler->pGPIOx->CRH &= ~GPIO_CRH_MODE14;
				//Se escoge el modo en el que se quiere el pin si analogo, flotante, pull-down o pull-up
				if(pGPIOHandler->GPIO_PinConfig.GPIO_PinPuPdControl == GPIO_PUPDR_ANALOG){
					pGPIOHandler->pGPIOx->CRH &= ~GPIO_CRH_CNF14;
				}else if(pGPIOHandler->GPIO_PinConfig.GPIO_PinPuPdControl == GPIO_PUPDR_FLOATING){
					pGPIOHandler->pGPIOx->CRH |= GPIO_CRH_CNF14_0;
				}else if(pGPIOHandler->GPIO_PinConfig.GPIO_PinPuPdControl == GPIO_PUPDR_PULLDOWN){
					pGPIOHandler->pGPIOx->CRH |= GPIO_CRH_CNF14_1;
				}else if(pGPIOHandler->GPIO_PinConfig.GPIO_PinPuPdControl == GPIO_PUPDR_PULLUP){
					pGPIOHandler->pGPIOx->CRH |= GPIO_CRH_CNF14_1;
				}
			}else if(pGPIOHandler->GPIO_PinConfig.GPIO_PinMode == GPIO_MODE_OUT){
				//Se va a escoger cual es la velocidad de salida del pin
				if(pGPIOHandler->GPIO_PinConfig.GPIO_PinSpeed == GPIO_OSPEED_10MHz){
					pGPIOHandler->pGPIOx->CRH |= GPIO_CRH_MODE14_0;
				}else if(pGPIOHandler->GPIO_PinConfig.GPIO_PinSpeed == GPIO_OSPEED_2MHz){
					pGPIOHandler->pGPIOx->CRH |= GPIO_CRH_MODE14_1;
				}else if(pGPIOHandler->GPIO_PinConfig.GPIO_PinSpeed == GPIO_OSPEED_50MHz){
					pGPIOHandler->pGPIOx->CRH |= GPIO_CRH_MODE14;
				}else{
					pGPIOHandler->pGPIOx->CRH |= GPIO_CRH_MODE14_0;
				}
				//Se escoge el modo que se quiere poner el pin
				if(pGPIOHandler->GPIO_PinConfig.GPIO_PinOPType == GPIO_OTYPER_PUSHPULL){
					pGPIOHandler->pGPIOx->CRH &= ~GPIO_CRH_CNF14;
				}else if(pGPIOHandler->GPIO_PinConfig.GPIO_PinOPType == GPIO_OTYPER_OPENDRAIN){
					pGPIOHandler->pGPIOx->CRH |= GPIO_CRH_CNF14_0;
				}
			}else if(pGPIOHandler->GPIO_PinConfig.GPIO_PinMode == GPIO_MODE_AF){
				//Se va a escoger cual es la velocidad de salida del pin
				if(pGPIOHandler->GPIO_PinConfig.GPIO_PinSpeed == GPIO_OSPEED_10MHz){
					pGPIOHandler->pGPIOx->CRH |= GPIO_CRH_MODE14_0;
				}else if(pGPIOHandler->GPIO_PinConfig.GPIO_PinSpeed == GPIO_OSPEED_2MHz){
					pGPIOHandler->pGPIOx->CRH |= GPIO_CRH_MODE14_1;
				}else if(pGPIOHandler->GPIO_PinConfig.GPIO_PinSpeed == GPIO_OSPEED_50MHz){
					pGPIOHandler->pGPIOx->CRH |= GPIO_CRH_MODE14;
				}else{
					pGPIOHandler->pGPIOx->CRH |= GPIO_CRH_MODE14_0;
				}
				//Se escoge el modo que se quiere poner el pin
				if(pGPIOHandler->GPIO_PinConfig.GPIO_PinOPType == GPIO_OTYPER_PUSHPULL){
					pGPIOHandler->pGPIOx->CRH |= GPIO_CRH_CNF14_1;
					pGPIOHandler->pGPIOx->CRH &= ~GPIO_CRH_CNF14_0;
				}else if(pGPIOHandler->GPIO_PinConfig.GPIO_PinOPType == GPIO_OTYPER_OPENDRAIN){
					pGPIOHandler->pGPIOx->CRH |= GPIO_CRH_CNF14;
				}
			}
			break;
		case 15:
			if(pGPIOHandler->GPIO_PinConfig.GPIO_PinMode == GPIO_MODE_IN){
				//Se pone los registros del MODE en 00
				pGPIOHandler->pGPIOx->CRH &= ~GPIO_CRH_MODE15;
				//Se escoge el modo en el que se quiere el pin si analogo, flotante, pull-down o pull-up
				if(pGPIOHandler->GPIO_PinConfig.GPIO_PinPuPdControl == GPIO_PUPDR_ANALOG){
					pGPIOHandler->pGPIOx->CRH &= ~GPIO_CRH_CNF15;
				}else if(pGPIOHandler->GPIO_PinConfig.GPIO_PinPuPdControl == GPIO_PUPDR_FLOATING){
					pGPIOHandler->pGPIOx->CRH |= GPIO_CRH_CNF15_0;
				}else if(pGPIOHandler->GPIO_PinConfig.GPIO_PinPuPdControl == GPIO_PUPDR_PULLDOWN){
					pGPIOHandler->pGPIOx->CRH |= GPIO_CRH_CNF15_1;
				}else if(pGPIOHandler->GPIO_PinConfig.GPIO_PinPuPdControl == GPIO_PUPDR_PULLUP){
					pGPIOHandler->pGPIOx->CRH |= GPIO_CRH_CNF15_1;
				}
			}else if(pGPIOHandler->GPIO_PinConfig.GPIO_PinMode == GPIO_MODE_OUT){
				//Se va a escoger cual es la velocidad de salida del pin
				if(pGPIOHandler->GPIO_PinConfig.GPIO_PinSpeed == GPIO_OSPEED_10MHz){
					pGPIOHandler->pGPIOx->CRH |= GPIO_CRH_MODE15_0;
				}else if(pGPIOHandler->GPIO_PinConfig.GPIO_PinSpeed == GPIO_OSPEED_2MHz){
					pGPIOHandler->pGPIOx->CRH |= GPIO_CRH_MODE15_1;
				}else if(pGPIOHandler->GPIO_PinConfig.GPIO_PinSpeed == GPIO_OSPEED_50MHz){
					pGPIOHandler->pGPIOx->CRH |= GPIO_CRH_MODE15;
				}else{
					pGPIOHandler->pGPIOx->CRH |= GPIO_CRH_MODE15_0;
				}
				//Se escoge el modo que se quiere poner el pin
				if(pGPIOHandler->GPIO_PinConfig.GPIO_PinOPType == GPIO_OTYPER_PUSHPULL){
					pGPIOHandler->pGPIOx->CRH &= ~GPIO_CRH_CNF15;
				}else if(pGPIOHandler->GPIO_PinConfig.GPIO_PinOPType == GPIO_OTYPER_OPENDRAIN){
					pGPIOHandler->pGPIOx->CRH |= GPIO_CRH_CNF15_0;
				}
			}else if(pGPIOHandler->GPIO_PinConfig.GPIO_PinMode == GPIO_MODE_AF){
				//Se va a escoger cual es la velocidad de salida del pin
				if(pGPIOHandler->GPIO_PinConfig.GPIO_PinSpeed == GPIO_OSPEED_10MHz){
					pGPIOHandler->pGPIOx->CRH |= GPIO_CRH_MODE15_0;
				}else if(pGPIOHandler->GPIO_PinConfig.GPIO_PinSpeed == GPIO_OSPEED_2MHz){
					pGPIOHandler->pGPIOx->CRH |= GPIO_CRH_MODE15_1;
				}else if(pGPIOHandler->GPIO_PinConfig.GPIO_PinSpeed == GPIO_OSPEED_50MHz){
					pGPIOHandler->pGPIOx->CRH |= GPIO_CRH_MODE15;
				}else{
					pGPIOHandler->pGPIOx->CRH |= GPIO_CRH_MODE15_0;
				}
				//Se escoge el modo que se quiere poner el pin
				if(pGPIOHandler->GPIO_PinConfig.GPIO_PinOPType == GPIO_OTYPER_PUSHPULL){
					pGPIOHandler->pGPIOx->CRH |= GPIO_CRH_CNF15_1;
					pGPIOHandler->pGPIOx->CRH &= ~GPIO_CRH_CNF15_0;
				}else if(pGPIOHandler->GPIO_PinConfig.GPIO_PinOPType == GPIO_OTYPER_OPENDRAIN){
					pGPIOHandler->pGPIOx->CRH |= GPIO_CRH_CNF15;
				}
			}
			break;
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

