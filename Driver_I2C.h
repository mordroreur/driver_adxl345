/****************************************************************************/
/*  MESNARD Emmanuel                                              ISIMA     */
/*  Octobre 2020                                                            */
/*                                                                          */
/*                      Driver de la liaison I2C                            */
/*                                                                          */
/* Driver_I2C.h                     MPLAB X                    PIC 18F542   */
/****************************************************************************/


#ifndef	_Driver_I2C_H

#define _Driver_I2C_H

#include "TypesMacros.h"

// Ouverture du port I2C en mode maitre, vitesse lente
void I2C_Init(void);
#define I2C_Open I2C_Init

// Attente de l'acquittement en provenance de l'esclave
void I2C_AckSlave(void);

// Envoi Ack
void I2C_Ack(void);

// Envoi Nack
void I2C_NAck(void);

// Start I2C  
void I2C_Start(void);

// Re-Start I2C  
void I2C_ReStart(void);

// Stop I2C  
void I2C_Stop(void);

// Envoi d'un octet de donnee 
void I2C_Write(INT8U Data);

// Lecture d'un octet de donnee 
INT8U I2C_Read(void);

#endif
