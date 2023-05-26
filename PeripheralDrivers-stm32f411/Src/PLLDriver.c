/*
 * PLLDriver.c
 *
 *  Created on: May 16, 2023
 *      Author: samuel
 */
#include "PLLDriver.h"

/*====Configuracion del PLL====*/
void ConfigPLL(PLL_Config_t *pPLLHandler){
	//Configuracion de la velocidad de los buses APB1 y APB2
	//Prescaler para el bus APB1
	RCC->CFGR &= ~RCC_CFGR_PPRE1; //Limpiamos el registro
	RCC->CFGR |= RCC_CFGR_PPRE1_2; //Dividimos el reloj del AHB por 2

	//Presacaler para el bus APB2
	RCC->CFGR &= ~RCC_CFGR_PPRE2; //Limpiamos el registro
	RCC->CFGR &= ~RCC_CFGR_PPRE2; //No se divide el reloj del AHB por algun valor


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
		RCC->CFGR |= (0b100 << RCC_CFGR_MCO1PRE_Pos);
		break;
	}
	case 1:{
		RCC->CFGR |= (0b101 << RCC_CFGR_MCO1PRE_Pos);
		break;
	}
	case 2:{
		RCC->CFGR |= (0b110 << RCC_CFGR_MCO1PRE_Pos);
		break;
	}
	case 3:{
		RCC->CFGR |= (RCC_CFGR_MCO1PRE);
		break;
	}
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

	//le damos el valor a la frecuencia de entrada del PLL
	auxFreqIn = 16;

	//Guardo los valores del PLLM, PLLN y PLLP en sus respectivas varaibles
	//PLLM
	auxPLLM = (RCC->PLLCFGR >> RCC_PLLCFGR_PLLM_Pos);
	//Aplicamos la mascara para obtener solo el valor del registro PLLM
	auxPLLM &= (RCC_PLLCFGR_PLLM);

	//PLLN
	auxPLLN = (RCC->PLLCFGR >> RCC_PLLCFGR_PLLN_Pos);
//	//Aplicamos la mascara para obtener solo el valor del registro PLLN
//	auxPLLN &= (RCC_PLLCFGR_PLLN);

	//PLLP
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

	return auxVariable;
}
