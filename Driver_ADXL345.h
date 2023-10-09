

/* 
 * File: Driver_ADXL345.h
 * Author: EMPRIN Corentin
 * Comments:
 * Revision history: 
 */

// This is a guard condition so that contents of this file are not included
// more than once.  
#ifndef _Driver_ADXL345_H
#define	_Driver_ADXL345_H

#include <xc.h>   

#include "TypesMacros.h"

//Définition des registres pour l'acceleromètre

// todo
#define CHIP_Write 0x00
#define CHIP_Read 0x00
#define PE_CS 0x00

#define DEVID           0x00
#define THRESH_TAP      0x1D
#define OFSX            0x1E
#define OFSY            0x1F
#define OFSZ            0x20
#define DUR             0x21
#define Latent          0x22
#define Window          0x23
#define THRESH_ACT      0x24
#define THRESH_INACT    0x25
#define TIME_INACT      0x26
#define ACT_INACT_CTL   0x27
#define THRESH_FF       0x28
#define TIME_FF         0x29
#define TAP_AXES        0x2A
#define ACT_TAP_STATUS  0x2B
#define BW_RATE         0x2C
#define POWER_CTL       0x2D
#define INT_ENABLE      0x2E
#define INT_MAP         0x2F
#define INT_SOURCE      0x30
#define DATA_FORMAT     0x32
#define DATAX0          0x32
#define DATAX1          0x33
#define DATAY0          0x34
#define DATAY1          0x35
#define DATAZ0          0x36
#define DATAZ1          0x37
#define FIFO_CTL        0x38
#define FIFO_STATUS     0x39


#define mode_I2C 1

//Initialisation
void ADXL345_Init(void);


// Ecriture de donnees dans un registre
void ADXL345_SendCmd(INT8U Register, INT8U Data);
void ADXL345_Write8(INT8U Registre, INT8U Donnee);
void ADXL345_Write(INT8U Registre, INT16 Donnee);

// Lecture de donnees depuis un registre
INT8U ADXL345_Read8(INT8U Registre);
INT16 ADXL345_Read(INT8U Registre);

void ADXL345_GetAcceleration8(INT8U * DATAX0);


#endif	/* Driver_ADXL345_H */

