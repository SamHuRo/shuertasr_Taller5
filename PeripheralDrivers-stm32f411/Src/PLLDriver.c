/*
 * PLLDriver.c
 *
 *  Created on: May 16, 2023
 *      Author: samuel
 */
#include "PLLDriver.h"

/*====Configuracion del PLL====*/
void ConfigPLL(PLL_Config_t *pPLLHandler){
	//Aseguramos la entrada del PLL es el HSI
	RCC->PLLCFGR &= ~(RCC_PLLCFGR_PLLSRC);

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
	}
	case 1:{
		RCC->PLLCFGR |= RCC_PLLCFGR_PLLP_0; //Se pone el valor de 4
	}
	case 2:{
		RCC->PLLCFGR |= RCC_PLLCFGR_PLLP_1; //Se pone el valor de 6
	}
	case 3: {
		RCC->PLLCFGR |= RCC_PLLCFGR_PLLP; //Sepone el valor de 8
	}
	}

	/*Cambiamos la latencia en FLASH_ACR para que el micro espere un poco para entrear la informacion en
	 * los registros*/

	FLASH->ACR &= ~(FLASH_ACR_LATENCY); //Limpiamos el registro
	//3WS de latencia
	FLASH->ACR |= FLASH_ACR_LATENCY_1WS;
	FLASH->ACR |= FLASH_ACR_LATENCY_2WS;

	//Seleccionamos el PLL
	RCC->CFGR |= (RCC_CFGR_MCO1);

	//Utilizamos el prescaler para poder ver la señal en el osciloscopio
	RCC->CFGR |= (RCC_CFGR_MCO1PRE_0);
	RCC->CFGR |= (RCC_CFGR_MCO1PRE_1);
	RCC->CFGR |= (RCC_CFGR_MCO1PRE_2);
	//Encender el PLL
	RCC->CR |= RCC_CR_PLLON;

	//Esperamos que el PLL se estabilice
	while(!(RCC->CR & RCC_CR_PLLRDY)){
		__NOP();
	}
	//Cambiamos el CPU clock source cambiamoando los }SW bits
	RCC->CFGR &= ~(RCC_CFGR_SW);
	RCC->CFGR |= (RCC_CFGR_SWS_1);
}

//Funcion para entregar el estado de la configuracion del equipo
void getConfigPLL(void){

}
