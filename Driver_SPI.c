/****************************************************************************/
/*  Groupe C                                                      ISIMA     */
/*  Octobre 2023                                                            */
/*                                                                          */
/*                                                                          */
/*                      Driver de la liaison SPI                            */
/*                                                                          */
/* Driver_SPI.c                     MPLAB X                    PIC 18F452   */
/****************************************************************************/

#define _Driver_SPI_C
#include <xc.h>
#include "Driver_SPI.h"
#include "TypesMacros.h"


// Ouverture du port SPI en mode totalement fonctionnel
void SPI_Init(void) {
    // Cablage liaison SPI est : SCK = PORTC(3)
    //                           SDI = PORTC(4)
    //                           SDO = PORTC(5)
    // Pas le choix, car 1 unique port SPI sur le PIC18F452
    // Cf. Datasheet PIC18F452, page 125
	
    LATCbits.LATC3 = 0; // Valeur par defaut
    LATCbits.LATC4 = 0;
    LATCbits.LATC5 = 0;
    
    // Serial Clock (SCK) - RC3/SCK/SCL/LVDIN
    // Serial Data In (SDI) - RC4/SDI/SDA : MISO
    // Serial Data Out (SDO) - RC5/SDO : MOSI
    TRISCbits.TRISC3 = 0; // Type d'ouverture des ports
    TRISCbits.TRISC4 = 1;
    TRISCbits.TRISC5 = 0;
 
	// Configuration du Registre de Status : SSPCON1
    //     0 : WCOL, no collision
    //     0 : SSPOV, No overflow
    //     0 : SSPEN,  Disables serial port and configures these pins as I/O port pins
    //     0 : CKP, IDLE state for clock is a low level
    //     0000 : SPI Master mode, clock = FOSC/4
	SSPCON1 = 0b00000000;
	
	
    // Configuration du Registre de Status : SSPSTAT
    //     0 : SMP, sample middle
    //     1 : CKE, Clock edge (Phase 0) pour transmission sur front montant
    //     0 : mode SPI (inutilise) : D/A
    //     0 : mode SPI (inutilise) : P
    //     0 : mode SPI (inutilise) : S
    //     0 : mode SPI (inutilise) : R/W
    //     0 : mode SPI (inutilise) : UA
    //     0 : BF, buffer full
    SSPSTAT = 0b01000000; // Mode_00, en milieu de cycle

    // Activation effective du module SSP
    SSPCON1bits.SSPEN = 1;
}

// Attente de fin de toutes les transmissions
void SPI_EndTransmission(void) {
    while (!SPI_BufferFull()); // On boucle tant que le buffer n'est pas plein
    // car toute transmission complete (lecture/ecriture) remplira le buffer
}

// Verification presence d'une donnee complete (8 bits) dans le buffer
BOOLEAN SPI_BufferFull(void) {
    return SSPSTATbits.BF; // verification du bit : Buffer Full Status bit
}

// Envoi d'un octet de donnee 
void SPI_Write(INT8U Data) {
  SSPBUF = Data; // on met la donnee a transmettre
  SPI_EndTransmission(); // on attend la fin de la transmission
}

// Lecture d'un octet de donnee 
INT8U SPI_Read(void) {
    SSPBUF = 0x00; // on ne transmet rien mais on s'assure qu il s actualise
    SPI_EndTransmission(); // on attend qu il ait fini de s actualiser
    return SSPBUF; // on lit ce qui est arrive dans le buffer
}

// Transfert complet en full duplex
INT8U SPI_ByteTransfert(INT8U Data) {
    SSPBUF = Data;         // Ecriture de la donnee a transmettre 
    SPI_EndTransmission(); // Attente de la fin de l'echange de donnees
    return(SSPBUF);        // Lecture effective de la donnee fournie
}


// Attention : certains PIC18 ne gere pas bien le flag BF ("Buffer Full")
// => A la place, utiliser le flag PIR1bits.SSPIF qui signale aussi la fin de transmission

//   PIR1bits.SSPIF = 0; // retrait du flag de la precedente transmission
//   SSPBUF = Data;      // Ecriture de la donnee a transmettre 
//   while (!PIR1bits.SSPIF); // Attente de la fin du cycle
//   return(SSPBUF);     // Lecture effective de la donnee