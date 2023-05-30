/*
 * DisplayLCDDriver.h
 *
 *  Created on: May 26, 2023
 *      Author: samuel
 */

#ifndef DISPLAYLCDDRIVER_H_
#define DISPLAYLCDDRIVER_H_

#include <stm32f4xx.h>
#include "I2CDriver.h"
#include "SysTickDriver.h"

#define ADDRES_LCD 0x26 //Direccion del display LCD

void init_LCD(I2C_Handler_t *ptrHandlerI2C);
void sendCMD_toLCD(I2C_Handler_t *ptrHandlerI2C, char cmd);
void sendData_toLCD(I2C_Handler_t *ptrHandlerI2C, char data);
void sendSTR_toLCD(I2C_Handler_t *ptrHandlerI2C, char *str);
void clearLCD(I2C_Handler_t *ptrHandlerI2C);
void moveCursor_inLCD(I2C_Handler_t *ptrHandlerI2C, uint8_t x, uint8_t y);
void delay_50 (void);
void delay_5 (void);
void delay_1 (void);
void delay_10 (void);
void clearScreenLCD(I2C_Handler_t *ptrHandlerI2C);
void writeData_inLCD(I2C_Handler_t *ptrHandlerI2C, uint8_t dataToWrite);


#endif /* DISPLAYLCDDRIVER_H_ */
