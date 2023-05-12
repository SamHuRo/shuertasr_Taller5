/*
 * PwmDriver.c
 *
 *  Created on: May 12, 2023
 *      Author: samuel
 */
#include "PwmDriver.h"
#include "BasicTimer.h"

/*
 * Es importante recordar que hay unos timer que utilizan cierta cantidad de bits:
 * - TIM2 TIM5 son de 32bits
 * - TIM1. TIM3, TIM4, TIM9, TIM10 y TIM11 son de 16bits
 */
void pwm_Config(PWM_Handler_t *ptrPwmHandler){

	/* 1. Activar la señal de reloj del periférico requerido */
	if(ptrPwmHandler->ptrTIMx == TIM2){
		/* agregue acá su código */
		RCC->APB1ENR &= ~RCC_APB1ENR_TIM2EN;
		RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;
	}
	else if(ptrPwmHandler->ptrTIMx == TIM3){
		/* agregue acá su código */
		RCC->APB1ENR &= ~RCC_APB1ENR_TIM3EN;
		RCC->APB1ENR |= RCC_APB1ENR_TIM3EN;
	}
	else if(ptrPwmHandler->ptrTIMx == TIM1){
		RCC->APB2ENR &= ~RCC_APB2ENR_TIM1EN;
		RCC->APB2ENR |= RCC_APB2ENR_TIM1EN;
	}
	else if(ptrPwmHandler->ptrTIMx == TIM4){
		RCC->APB2ENR &= ~RCC_APB1ENR_TIM4EN;
		RCC->APB2ENR |= RCC_APB1ENR_TIM4EN;
	}
	else if(ptrPwmHandler->ptrTIMx == TIM5){
		RCC->APB2ENR &= ~RCC_APB1ENR_TIM5EN;
		RCC->APB2ENR |= RCC_APB1ENR_TIM5EN;
	}
	else if(ptrPwmHandler->ptrTIMx == TIM9){
		RCC->APB2ENR &= ~RCC_APB2ENR_TIM9EN;
		RCC->APB2ENR |= RCC_APB2ENR_TIM9EN;
	}
	else if(ptrPwmHandler->ptrTIMx == TIM10){
		RCC->APB2ENR &= ~RCC_APB2ENR_TIM10EN;
		RCC->APB2ENR |= RCC_APB2ENR_TIM10EN;
	}
	else if(ptrPwmHandler->ptrTIMx == TIM11){
		RCC->APB2ENR &= ~RCC_APB2ENR_TIM11EN;
		RCC->APB2ENR |= RCC_APB2ENR_TIM11EN;
	}
	else{
		__NOP();
	}

	/* 1. Cargamos la frecuencia deseada */
	setFrequency(ptrPwmHandler);

	/* 2. Cargamos el valor del dutty-Cycle*/
	setDuttyCycle(ptrPwmHandler);

	/* 2a. Estamos en UP_Mode, el limite se carga en ARR y se comienza en 0 */
	/* agregue acá su código */
	//Se ponde el Timer en UP-Mode
	ptrPwmHandler->ptrTIMx->CR1  &= ~TIM_CR1_DIR;
	//Se pone el CNT (contador) en 0
	ptrPwmHandler->ptrTIMx->CNT = 0;
	/* 3. Configuramos los bits CCxS del registro TIMy_CCMR1, de forma que sea modo salida
	 * (para cada canal hay un conjunto CCxS)
	 *
	 * 4. Además, en el mismo "case" podemos configurar el modo del PWM, su polaridad...
	 *
	 * 5. Y además activamos el preload bit, para que cada vez que exista un update-event
	 * el valor cargado en el CCRx será recargado en el registro "shadow" del PWM */
	switch(ptrPwmHandler->config.channel){
	case PWM_CHANNEL_1:{
		// Seleccionamos como salida el canal
		/* agregue acá su código */
		ptrPwmHandler->ptrTIMx->CCMR1 &= ~TIM_CCMR1_CC1S;
		//Activamos el OC1FE
		ptrPwmHandler->ptrTIMx->CCMR1 |= TIM_CCMR1_OC1FE;
		// Configuramos el canal como PWM
		/* agregue acá su código */
		ptrPwmHandler->ptrTIMx->CCMR1 |= TIM_CCMR1_OC1M_1;
		ptrPwmHandler->ptrTIMx->CCMR1 |= TIM_CCMR1_OC1M_2;
		// Activamos la funcionalidad de pre-load
		/* agregue acá su código */
		ptrPwmHandler->ptrTIMx->CCMR1 |= TIM_CCMR1_OC1PE;
		break;
	}
	case PWM_CHANNEL_2:{
		// Seleccionamos como salida el canal
		/* agregue acá su código */
		ptrPwmHandler->ptrTIMx->CCMR1 &= ~TIM_CCMR1_CC2S;
		//Activamos el OC1FE
		ptrPwmHandler->ptrTIMx->CCMR1 |= TIM_CCMR1_OC2FE;
		// Configuramos el canal como PWM
		/* agregue acá su código */
		ptrPwmHandler->ptrTIMx->CCMR1 |= TIM_CCMR1_OC2M_1;
		ptrPwmHandler->ptrTIMx->CCMR1 |= TIM_CCMR1_OC2M_2;
		// Activamos la funcionalidad de pre-load
		/* agregue acá su código */
		ptrPwmHandler->ptrTIMx->CCMR1 |= TIM_CCMR1_OC2PE;
		break;
	}
	case PWM_CHANNEL_3: {
		// Seleccionamos como salida el canal
		/* agregue acá su código */
		ptrPwmHandler->ptrTIMx->CCMR2 &= ~TIM_CCMR2_CC3S;;
		//Activamos el OC1FE
		ptrPwmHandler->ptrTIMx->CCMR2 |= TIM_CCMR2_OC3FE;
		// Configuramos el canal como PWM
		/* agregue acá su código */
		ptrPwmHandler->ptrTIMx->CCMR2 |= TIM_CCMR2_OC3M_1;
		ptrPwmHandler->ptrTIMx->CCMR2 |= TIM_CCMR2_OC3M_2;
		// Activamos la funcionalidad de pre-load
		/* agregue acá su código */
		ptrPwmHandler->ptrTIMx->CCMR2 |= TIM_CCMR2_OC3PE;
		break;
	}
	case PWM_CHANNEL_4: {
		// Seleccionamos como salida el canal
		/* agregue acá su código */
		ptrPwmHandler->ptrTIMx->CCMR2 &= ~TIM_CCMR2_CC4S;;
		//Activamos el OC1FE
		ptrPwmHandler->ptrTIMx->CCMR2 |= TIM_CCMR2_OC4FE;
		// Configuramos el canal como PWM
		/* agregue acá su código */
		ptrPwmHandler->ptrTIMx->CCMR2 |= TIM_CCMR2_OC4M_1;
		ptrPwmHandler->ptrTIMx->CCMR2 |= TIM_CCMR2_OC4M_2;
		// Activamos la funcionalidad de pre-load
		/* agregue acá su código */
		ptrPwmHandler->ptrTIMx->CCMR2 |= TIM_CCMR2_OC4PE;
		break;
	}
	default:{
		break;
	}

	/* 6. Activamos la salida seleccionada */
	enableOutput(ptrPwmHandler);

	}// fin del switch-case
}

/* Función para activar el Timer y activar todo el módulo PWM */
void startPwmSignal(PWM_Handler_t *ptrPwmHandler) {
	/* agregue acá su código */
	//Activamos el autoreload
	ptrPwmHandler->ptrTIMx->CR1 |= TIM_CR1_ARPE;
	//Se activa el CEN (counter enable)
	ptrPwmHandler->ptrTIMx->CR1 |= TIM_CR1_CEN;
}

/* Función para desactivar el Timer y detener todo el módulo PWM*/
void stopPwmSignal(PWM_Handler_t *ptrPwmHandler) {
	/* agregue acá su código */
	//Desactivamos el autoreload
	ptrPwmHandler->ptrTIMx->CR1 &= ~TIM_CR1_ARPE;
	//Desactivamos el CEN
	ptrPwmHandler->ptrTIMx->CR1 &= ~TIM_CR1_CEN;
}

/* Función encargada de activar cada uno de los canales con los que cuenta el TimerX */
void enableOutput(PWM_Handler_t *ptrPwmHandler) {
	switch (ptrPwmHandler->config.channel) {
	case PWM_CHANNEL_1: {
		// Activamos la salida del canal 1
		/* agregue acá su código */
		//Se activa si la salida va al pin
		ptrPwmHandler->ptrTIMx->CCER |= TIM_CCER_CC1E;
		break;
	}
	case PWM_CHANNEL_2: {
		ptrPwmHandler->ptrTIMx->CCER |= TIM_CCER_CC2E;
	}
	case PWM_CHANNEL_3: {
		ptrPwmHandler->ptrTIMx->CCER |= TIM_CCER_CC3E;
	}
	case PWM_CHANNEL_4: {
		ptrPwmHandler->ptrTIMx->CCER |= TIM_CCER_CC4E;
	}
	default: {
		break;
	}
	}
}

/*
 * La frecuencia es definida por el conjunto formado por el preescaler (PSC)
 * y el valor límite al que llega el Timer (ARR), con estos dos se establece
 * la frecuencia.
 * */
void setFrequency(PWM_Handler_t *ptrPwmHandler){

	// Cargamos el valor del prescaler, nos define la velocidad (en ns) a la cual
	// se incrementa el Timer
	/* agregue acá su código */
	switch(ptrPwmHandler->config.prescaler){
	case(BTIMER_SPEED_10us):{
		ptrPwmHandler->ptrTIMx->PSC = 160;
		break;
	}
	case(BTIMER_SPEED_100us):{
		ptrPwmHandler->ptrTIMx->PSC = 1600;
		break;
	}
	case(BTIMER_SPEED_1ms):{
		ptrPwmHandler->ptrTIMx->PSC = 16000;
		break;
	}
	default:{
		ptrPwmHandler->ptrTIMx->PSC = ptrPwmHandler->config.prescaler;
		break;
	}
	}
	// Cargamos el valor del ARR, el cual es el límite de incrementos del Timer
	// antes de hacer un update y reload.
	/* agregue acá su código */
	ptrPwmHandler->ptrTIMx->ARR = ptrPwmHandler->config.periodo;

}


/* Función para actualizar la frecuencia, funciona de la mano con setFrequency */
void updateFrequency(PWM_Handler_t *ptrPwmHandler, uint16_t newFreq){
	// Actualizamos el registro que manipula el periodo
    /* agregue acá su código */
	ptrPwmHandler->config.periodo = newFreq;
	// Llamamos a la fucnión que cambia la frecuencia
	/* agregue acá su código */
	setFrequency(&*ptrPwmHandler);
}

/* El valor del dutty debe estar dado en valores de %, entre 0% y 100%*/
void setDuttyCycle(PWM_Handler_t *ptrPwmHandler){

	// Seleccionamos el canal para configurar su dutty
	switch(ptrPwmHandler->config.channel){
	case PWM_CHANNEL_1:{
		ptrPwmHandler->ptrTIMx->CCR1 = (ptrPwmHandler->config.duttyCicle);
		break;
	}
	case PWM_CHANNEL_2: {
		ptrPwmHandler->ptrTIMx->CCR2 = (ptrPwmHandler->config.duttyCicle);
		break;
	}
	case PWM_CHANNEL_3: {
		ptrPwmHandler->ptrTIMx->CCR3 = (ptrPwmHandler->config.duttyCicle);
		break;
	}
	case PWM_CHANNEL_4: {
		ptrPwmHandler->ptrTIMx->CCR4 = (ptrPwmHandler->config.duttyCicle);
		break;
	}
	default:{
		break;
	}
	}// fin del switch-case

}


/* Función para actualizar el Dutty, funciona de la mano con setDuttyCycle */
void updateDuttyCycle(PWM_Handler_t *ptrPwmHandler, uint16_t newDutty){
	// Actualizamos el registro que manipula el dutty
    /* agregue acá su código */
	ptrPwmHandler->config.duttyCicle = newDutty;
	// Llamamos a la fucnión que cambia el dutty y cargamos el nuevo valor
	/* agregue acá su código */
	setDuttyCycle(&*ptrPwmHandler);
}

