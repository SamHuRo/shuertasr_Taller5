/*
 * PLLDriver.c
 *
 *  Created on: May 16, 2023
 *      Author: samuel
 */
#include "PLLDriver.h"
uint8_t pllOn = 0;

/*====Configuracion del PLL====*/
void ConfigPLL(PLL_Config_t *pPLLHandler){
	//Levantar la bandera si se encuentra activado
	if(pPLLHandler->PLL_ON == 1){
		pllOn = 1;
		//Configuracion de la velocidad de los buses APB1 y APB2
		//Prescaler para el bus APB1
		RCC->CFGR &= ~RCC_CFGR_PPRE1; //Limpiamos el registro
		switch(pPLLHandler->APB1_PRESC){
			case 0:
				RCC->CFGR |= 0b100 << RCC_CFGR_PPRE1_Pos; //Dividimos el reloj del APB1 por 2
				break;
			case 1:
				RCC->CFGR |= 0b101 << RCC_CFGR_PPRE1_Pos; //Dividimos el reloj del APB1 por 4
				break;
			case 2:
				RCC->CFGR |= 0b110 << RCC_CFGR_PPRE1_Pos; //Dividimos el reloj del APB1 por 8
				break;
			case 3:
				RCC->CFGR |= 0b111 << RCC_CFGR_PPRE1_Pos; //Dividimos el reloj del APB1 por 16
				break;
			default:
				RCC->CFGR |= 0b000 << RCC_CFGR_PPRE1_Pos; //No se divide el reloj del APB1
				break;
		}
		//Presacaler para el bus APB2
		RCC->CFGR &= ~RCC_CFGR_PPRE2; //Limpiamos el registro
		switch(pPLLHandler->APB2_PRESC){
			case 0:
				RCC->CFGR |= 0b100 << RCC_CFGR_PPRE2_Pos; //Dividimos el reloj del APB2 por 2
				break;
			case 1:
				RCC->CFGR |= 0b101 << RCC_CFGR_PPRE2_Pos; //Dividimos el reloj del APB2 por 4
				break;
			case 2:
				RCC->CFGR |= 0b110 << RCC_CFGR_PPRE2_Pos; //Dividimos el reloj del APB2 por 8
				break;
			case 3:
				RCC->CFGR |= 0b111 << RCC_CFGR_PPRE2_Pos; //Dividimos el reloj del APB2 por 16
				break;
			default:
				RCC->CFGR |= 0b000 << RCC_CFGR_PPRE2_Pos; //No se divide el reloj del APB2
				break;
		}
		//Aseguramos la entrada del PLL es el HSI
		RCC->PLLCFGR &= ~(RCC_PLLCFGR_PLLSRC); //Limpiamos el registro
		RCC->PLLCFGR |= (RCC_PLLCFGR_PLLSRC_HSI); //Cargamos

		//Limpiamos el registro
		RCC->PLLCFGR &= ~(RCC_PLLCFGR_PLLM);
		//Ponemos el valor en el PLLM
		RCC->PLLCFGR |= (pPLLHandler->PLL_PLLM << RCC_PLLCFGR_PLLM_Pos);

		//Ponemos el valor en el PLLN
		RCC->PLLCFGR &= ~(RCC_PLLCFGR_PLLN); //Limpiamos el registro
		RCC->PLLCFGR |= (pPLLHandler->PLL_PLLN << RCC_PLLCFGR_PLLN_Pos);

		//Ponemos eel valor en el PLLP
		//Limpiamos la posicion del PLLP
		RCC->PLLCFGR &= ~(RCC_PLLCFGR_PLLP);
		//Se mira cual es el valor que se va a poner al PLLP
		switch(pPLLHandler->PLL_PLLP){
			case 0:{
				RCC->PLLCFGR &= ~(RCC_PLLCFGR_PLLP); //Se pone el valor de 2
				break;
			}
			case 1:{
				RCC->PLLCFGR |= RCC_PLLCFGR_PLLP_0; //Se pone el valor de 4
				break;
			}
			case 2:{
				RCC->PLLCFGR |= RCC_PLLCFGR_PLLP_1; //Se pone el valor de 6
				break;
			}
			case 3: {
				RCC->PLLCFGR |= RCC_PLLCFGR_PLLP; //Sepone el valor de 8
				break;
			}
			default:
				break;
		}
		/*Cambiamos la latencia en FLASH_ACR para que el micro espere un poco para entrear la informacion en
		 * los registros*/
		FLASH->ACR &= ~(FLASH_ACR_LATENCY); //Limpiamos el registro
		//2 WS de latencia
		FLASH->ACR |= FLASH_ACR_LATENCY_2WS;

		//Limpiar el registro del MCO1
		RCC->CFGR &= ~(RCC_CFGR_MCO1);
		//Seleccionamos el PLL
		RCC->CFGR |= (RCC_CFGR_MCO1);

		//Limpiamos el reguistro del MCO1PRE
		RCC->CFGR &= ~(RCC_CFGR_MCO1PRE);
		//Utilizamos el prescaler para poder ver la señal en el osciloscopio
		switch(pPLLHandler->PLL_MCO1PRE){
			case 0:{
				RCC->CFGR |= (0b100 << RCC_CFGR_MCO1PRE_Pos); //Se divide por 2
				break;
			}
			case 1:{
				RCC->CFGR |= (0b101 << RCC_CFGR_MCO1PRE_Pos); //Se divide por 3
				break;
			}
			case 2:{
				RCC->CFGR |= (0b110 << RCC_CFGR_MCO1PRE_Pos); //Se divide por 4
				break;
			}
			case 3:{
				RCC->CFGR |= (RCC_CFGR_MCO1PRE); //Se divide por 5
				break;
			}
			default:
				RCC->CFGR |= (0b000 << RCC_CFGR_MCO1PRE_Pos); //No se realiza division
				break;
		}
		//Encender el PLL
		RCC->CR |= RCC_CR_PLLON;
		//Esperamos que el PLL se estabilice
		while(!(RCC->CR & RCC_CR_PLLRDY)){
			__NOP();
		}
		//Se cambia el reloj para que sea el PLL
		RCC->CFGR &= ~(RCC_CFGR_SW);
		RCC->CFGR |= (RCC_CFGR_SW_1);
	}
}

/*=====Funcion para entregar el estado de la configuracion del equipo=====*/
uint16_t getConfigPLL(void){
	//Variable auxiliar para guardar la configuracion del equipo
	uint16_t auxVariable = 0;
	//Variables para guardar los valores de PLLM, PLLN y PLLP
	uint8_t auxPLLM = 0;
	uint16_t auxPLLN = 0;
	uint8_t auxPLLP = 0;
	//Variable para guardar la frecuencia de entrada del PLL
	uint8_t auxFreqIn = 0;
	if(pllOn == 1){
		//le damos el valor a la frecuencia de entrada del PLL
		auxFreqIn = 16;
		//Guardo los valores del PLLM, PLLN y PLLP en sus respectivas varaibles
		//Se saca el valor del PLLM
		auxPLLM = (RCC->PLLCFGR >> RCC_PLLCFGR_PLLM_Pos);
		//Aplicamos la mascara para obtener solo el valor del registro PLLM
		auxPLLM &= (RCC_PLLCFGR_PLLM);

		//Se saca el valor del PLLN
		auxPLLN = (RCC->PLLCFGR >> RCC_PLLCFGR_PLLN_Pos);
		//Aplicamos la mascara para obtener solo el valor del registro PLLN
	//	auxPLLN &= (RCC_PLLCFGR_PLLN);

		//Se saca el valor del PLLP
		auxPLLP = (RCC->PLLCFGR >> RCC_PLLCFGR_PLLP_Pos);
		//Aplicamos la mascara para obtener solo el valor del registro PLLP
		auxPLLP &= (RCC_PLLCFGR_PLLP);
		//Hacemos un switch case para guardar el valor real del registro PLLP
		switch(auxPLLP){
		case 0:{
			auxPLLP = 2;
			break;
		}
		case 1:{
			auxPLLP = 4;
			break;
		}
		case 2:{
			auxPLLP = 6;
			break;
		}
		case 3:{
			auxPLLP = 8;
			break;
		}
		}

		//Calculando para el VCO clock
		auxVariable = auxFreqIn * (auxPLLN / (uint16_t) auxPLLM);
		//Calculando PLL general clock output
		auxVariable = auxVariable / auxPLLP;
	}else{
		auxVariable = 16;
	}

	return auxVariable;
}
