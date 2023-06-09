/*
 * BasicTimer.h
 *
 *  Created on: May 12, 2023
 *      Author: samuel
 */

#ifndef INC_BASICTIMER_H_
#define INC_BASICTIMER_H_
//Incluir el archivo stm32f1xx.h
#include <stm32f1xx.h>

#define BTIMER_MODE_UP		0
#define BTIMER_MODE_DOWN	1

#define BTIMER_SPEED_10us	80
#define BTIMER_SPEED_100us	800
#define BTIMER_SPEED_1ms	8000



/* Estructura que contiene la configuración mínima necesaria para el manejo del Timer.*/
typedef struct
{
	uint8_t		TIMx_mode; 		// Up or dowm
	uint32_t	TIMx_speed;		// A qué velocidad se incrementa el Timer
	uint32_t	TIMx_period;	// Valor en ms del periodo del Timer
	uint8_t		TIMx_interruptEnable;	// Activa o desactiva el modo interrupción del timer.
}BasicTimer_Config_t;

/* Handler para el Timer*/
typedef struct
{
	TIM_TypeDef			*ptrTIMx;
	BasicTimer_Config_t	TIMx_Config;
}BasicTimer_Handler_t;

void BasicTimer_Config(BasicTimer_Handler_t *ptrBTimerHandler);
void BasicTimer1_Callback(void);
void BasicTimer2_Callback(void); /* Esta función debe ser sobre-escrita en el main para que el sistema funcione*/
void BasicTimer3_Callback(void);
void BasicTimer4_Callback(void);
void BasicTimer5_Callback(void);
void BasicTimer6_Callback(void);
void BasicTimer7_Callback(void);
void BasicTimer8_Callback(void);


#endif /* INC_BASICTIMER_H_ */
