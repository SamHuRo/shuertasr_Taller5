/*
 * MPUxDriver.h
 *
 *  Created on: May 26, 2023
 *      Author: samuel
 */

#ifndef MPUXDRIVER_H_
#define MPUXDRIVER_H_

/*Definicion de macros de las posiciones de memoria*/
#define ACCEL_ADDRESS	0b1101001; //0xD2 -> Direccion del accel con Logic_1
#define ACCEL_XOUT_H	59 //0x3B
#define ACCEL_XOUT_L	60 //0x3C
#define ACCEL_YOUT_H	61 //0x3D
#define ACCEL_YOUT_L	62 //0x3E
#define ACCEL_ZOUT_H	63 //0x3F
#define ACCEL_ZOUT_L	64 //0x40

#define PWR_MGMT_1		107 //Registros del power_manager_1, mantiene el equipo en reset
#define WHO_AM_I		117 //Registros del who_am_i el cual es del MPU-6050 y es 0x68 (identificar que es el sensor)

/*----Definicion de variables----*/
#define ESCALA			2
#define GRAVEDAD 		9.8
#define	CANTIDAD_BITS	32767

void muestreoAccel(void);

#endif /* MPUXDRIVER_H_ */
