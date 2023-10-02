/****************************************************************************/
/*  Groupe C                                                      ISIMA     */
/*  Octobre 2023                                                            */
/*                                                                          */
/*                                                                          */
/*                      Driver de la liaison SPI                            */
/*                                                                          */
/* Driver_SPI.h                     MPLAB X                    PIC 18F452   */
/****************************************************************************/


#ifndef	_Driver_GROUPEC_SPI_H

#define _Driver_GROUPEC_SPI_H

#include "TypesMacros.h"


//================================================================   
//              Fonctions de gestion du port SPI
//================================================================

// Ouverture du port SPI en mode totalement fonctionnel
void SPI_Init(void);

// Attente de fin de toutes les transmissions
void SPI_EndTransmission(void);

// Verification presence d'une donnee dans le buffer
BOOLEAN SPI_BufferFull(void);

// Envoi d'un octet de donnee 
void SPI_Write(INT8U Data);

// Lecture d'un octet de donnee 
INT8U SPI_Read(void);

// Transfert complet en full duplex
INT8U SPI_ByteTransfert(INT8U Data);

//Remapping des fonction avec des noms parfois utilises
#define SPI_Open SPI_Init
#define SPI_Exchange SPI_ByteTransfert 
#define SPI_Transfert SPI_ByteTransfert 
#define SPI_ReadWrite SPI_ByteTransfert


#endif /*_Driver_GROUPEC_SPI_H*/
