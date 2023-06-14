/*
 * stm32f411xx_hal.h
 *
 *  Created on: 2/03/2023
 *      Author: Samuel Huertas Rojas
 *
 * 	Este proyecto contiene la información mas basica del micro:
 * 	- Valores del reloj principal
 * 	- Distribución básica de la memoria
 * 	- Posiciones de memoria de los periféricos
 * 	- Incluye los otros registros del periféricos
 * 	- Definiciones de las constantes básicas
 */

#ifndef STM32F411XX_HAL_H_
#define STM32F411XX_HAL_H_

#include <stdint.h>
#include <stddef.h>

#define HSI_CLOCK_SPEED    16000000    //Value for the main clock signal (HSI -> High Speed Internal)
#define HSE_CLOCK_SPEED    4000000     //Value for the main clock signal (HSE -> High Speed External)

#define NOP()    asm("NOP")
#define __weak   __attribute((weak))

/*
 * Base addresses of Flash and SRAM memories
 * Datasheet, Memory Map, Figure 14
 * (Remember, 1KByte = 1024 bytes)
 */
#define FLASH_BASE_ADDR    0x08000000U    //Esta es la memoria del programa, 512KB
#define SRAM_BASE_ADDR     0x20000000U    //Esta es la memoria RAM, 128KB

/*
 * Ahora agregamos la dirección de memoria base para cada uno de los periféricos que posee el micro
 * En el data sheet del micro, encontramos el mapa de los buses:
 * 	- APB1
 * 	- APB2
 * 	- AHB1
 * 	- AHB2
 *
 * 	AHBx y APBx dirección de los periféricos
 */
#define APB1_BASE_ADDR    0x40000000U
#define APB2_BASE_ADDR    0x40010000U
#define AHB1_BASE_ADDR    0x40020000U
#define AHB2_BASE_ADDR    0x50000000U

/*
 * Ahora se debe de hacer lo mismo pero para cada una de las posiciones de memoria de cada uno de los
 * periféricos descritos en la tabla.
 */

/*
 * Posiciones de memoria para periféricos del AHB2
 */
#define USB_OTG_FS_BASE_ADDR    (AHB2_BASE_ADDR + 0x0000U)

/*
 * Posiciones de memoria para periféricos del AHB1
 * Observar que NO esta completa
 */
#define RCC_BASE_ADDR		(AHB1_BASE_ADDR + 0x3800U)
#define GPIOH_BASE_ADDR  	(AHB1_BASE_ADDR + 0x1C00U)
#define GPIOE_BASE_ADDR  	(AHB1_BASE_ADDR + 0x1000U)
#define GPIOD_BASE_ADDR  	(AHB1_BASE_ADDR + 0x0C00U)
#define GPIOC_BASE_ADDR		(AHB1_BASE_ADDR + 0x0800U)
#define GPIOB_BASE_ADDR		(AHB1_BASE_ADDR + 0x0400U)
#define GPIOA_BASE_ADDR		(AHB1_BASE_ADDR + 0x0000U)

/*
 * Marcos Genéricos
 */
#define ENABLE				1
#define DISABLE				0
#define SET					ENABLE
#define CLEAR				DISABLE
#define RESET				DISABLE
#define GPIO_PIN_SET		SET
#define GPIO_PIN_RESET		RESET
#define FLAG_SET			SET
#define FLAG_RESET			RESET
#define I2C_WRITE			0
#define I2C_READ			1

/*
 * Definición de la estructura de datos que representa a cada uno de los registros que componen el
 * periférico RCC
 */
typedef struct
{
	volatile uint32_t CR;					//Clock control register
	volatile uint32_t PLLCFGR;				//PLL configuration register
	volatile uint32_t CFGR;					//Clock configuration register
	volatile uint32_t CIR;					//clock interrupt register
	volatile uint32_t AHB1RSTR;				//AHB1 peripheral reset register
	volatile uint32_t AHB2RSTR;				//AHB2 peripheral reset register
	volatile uint32_t reserved0;			//reserved
	volatile uint32_t reserved1;			//reserved
	volatile uint32_t APB1RSTR;				//APB1 peripheral reset register
	volatile uint32_t APB2RSTR;				//APB2 peripheral reset register
	volatile uint32_t reserved2;			//reserved
	volatile uint32_t reserved3;			//reserved
	volatile uint32_t AHB1ENR;				//AHB1 peripheral clock enable register
	volatile uint32_t AHB2ENR;				//AHB2 peripheral clock enable register
	volatile uint32_t reserved4;			//reserved
	volatile uint32_t reserved5;			//reserved
	volatile uint32_t APB1ENR;				//APB1 peripheral clock enable register
	volatile uint32_t APB2ENR;				//APB2 peripheral clock enable register
	volatile uint32_t reserved6;			//reserved
	volatile uint32_t reserved7;			//reserved
	volatile uint32_t AHB1LPENR;			//AHB1 clock enable low power register
	volatile uint32_t AHB2LPENR;			//AHB2 clock enable low power register
	volatile uint32_t reserved8;			//reserved
	volatile uint32_t reserved9;			//reserved
	volatile uint32_t APB1LPENR;			//APB1 clock enable low power register
	volatile uint32_t APB2LPENR;			//APB2 clock enable low power register
	volatile uint32_t reserved10;			//reserved
	volatile uint32_t reserved11;			//reserved
	volatile uint32_t BDCR;					//Backup domain control register
	volatile uint32_t CSR;					//Clock control and status register
	volatile uint32_t reserved12;			//reserved
	volatile uint32_t reserved13;			//reserved
	volatile uint32_t SSCGR;				//Spread spectrum clock generation reg
	volatile uint32_t PLLI2SCFGR;			//PLLI2S configuration register
	volatile uint32_t reserved14;			//reserved
	volatile uint32_t DCKCFGR;				//Dedicated clocks configuration reg
} RCC_RegDef_t;

/*
 * Puntero a RCC_RegDef_t queapunta a la posición excata del periférico RCC, de forma que cada uno de los SFR en
 * la memoria del MCU.
 */
#define RCC				((RCC_RegDef_t *) RCC_BASE_ADDR)

/*
 * 6.3.9 RCC_AHB1ENR
 */
#define RCC_AHB1ENR_GPIOA_EN		0
#define RCC_AHB1ENR_GPIOB_EN		1
#define RCC_AHB1ENR_GPIOC_EN		2
#define RCC_AHB1ENR_GPIOD_EN		3
#define RCC_AHB1ENR_GPIOE_EN		4
#define RCC_AHB1ENR_GPIOH_EN		7
#define RCC_AHB1ENR_CRCEN			12
#define RCC_AHB1ENR_DMA1_EN			21
#define RCC_AHB1ENR_DMA2_EN			22

/* ===== Fin de la descripción de los elementos que componen el periférico RCC =====*/

/*==== Inicio de la descripción de los elementos que componen el periférico GPIOx ====*/
/*
 * Definición de la estructura de datos que representa a cada uno de los registros que componen el
 * periférico GPIO
 */
typedef struct
{
	volatile uint32_t MODER;				//Port mode register
	volatile uint32_t OTYPER;				//Port output  type register
	volatile uint32_t OSPEEDR;				//Port output speed register
	volatile uint32_t PUPDR;				//Port pull-up/pull-down register
	volatile uint32_t IDR;					//Port input data register
	volatile uint32_t ODR;					//Port output data register
	volatile uint32_t BSRR;					//Port bit set/reset register
	volatile uint32_t LCKR;					//Port configuration lock register
	volatile uint32_t AFRL;					//Alternate function low register
	volatile uint32_t AFRH;					//Alternate function high register
} GPIOx_RegDef_t;

/*
 * Al igual que con el RCC, creamos un puntero a la estructura que define GPIOx y debemos hacer
 * que cada GPIOx quede ubicado exactamentesobre la posición de memoria correcta.
 */
#define GPIOA				((GPIOx_RegDef_t *) GPIOA_BASE_ADDR)
#define GPIOB				((GPIOx_RegDef_t *) GPIOB_BASE_ADDR)
#define GPIOC				((GPIOx_RegDef_t *) GPIOC_BASE_ADDR)
#define GPIOD				((GPIOx_RegDef_t *) GPIOD_BASE_ADDR)
#define GPIOE				((GPIOx_RegDef_t *) GPIOE_BASE_ADDR)
#define GPIOH				((GPIOx_RegDef_t *) GPIOH_BASE_ADDR)

/*
 * Valores estándar para las configuraciones
 * 8.4.1 GPIOx_MODER (dos bit por cada PIN)
 */
#define GPIO_MODE_IN		0
#define GPIO_MODE_OUT		1
#define GPIO_MODE_ALTFN		2
#define GPIO_MODE_ANALOG	3

/*
 * 8.4.2 GPIOx_OTYPER (un bit por PIN)
 */
#define GPIO_OTYPER_PUSHPULL	0
#define GPIO_OTYPER_OPENDRAIN	1

/*
 * GPIOx_OSPEEDR (dos bit por PIN)
 */
#define GPIO_OSPEED_LOW		0
#define GPIO_OSPEED_MEDIUM	1
#define GPIO_OSPEED_FAST	2
#define GPIO_OSPEED_HIGH	3

/*
 * GPIOx_PUPDR (dos bit por PIN)
 */
#define GPIO_PUPDR_NOTHING		0
#define GPIO_PUPDR_PULLUP		1
#define GPIO_PUPDR_PULLDOWN		2
#define GPIO_PUPDR_RESERVED		3

/*
 * Definición de los nombres de los pines
 */
#define PIN_0			0
#define PIN_1			1
#define PIN_2			2
#define PIN_3			3
#define PIN_4			4
#define PIN_5			5
#define PIN_6			6
#define PIN_7			7
#define PIN_8			8
#define PIN_9			9
#define PIN_10			10
#define PIN_11			11
#define PIN_12			12
#define PIN_13			13
#define PIN_14			14
#define PIN_15			15

/*Definición de las funciones alternativas*/
#define AF0				0b0000
#define AF1				0b0001
#define AF2				0b0010
#define AF3				0b0011
#define AF4				0b0100
#define AF5				0b0101
#define AF6				0b0110
#define AF7				0b0111
#define AF8				0b1000
#define AF9				0b1001
#define AF10			0b1010
#define AF11			0b1011
#define AF12			0b1100
#define AF13			0b1101
#define AF14			0b1110
#define AF15			0b1111

/*
 * typedef struct
 * {
 * 		volatile unit32_t dummy;    /// Dummy example register ADDR_OFFSET:   0x00
 * } DUMMY_RegDef_t;
 */

#endif /* STM32F411XX_HAL_H_ */
