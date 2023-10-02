/****************************************************************************/
/*  Groupe C                                                      ISIMA     */
/*  Octobre 2023                                                            */
/*                                                                          */
/*                                                                          */
/*                      Driver de la liaison I2C                            */
/*                                                                          */
/* Driver_I2C.c                     MPLAB X                    PIC 18F452   */
/****************************************************************************/

#define _Driver_I2C_C

#include <xc.h>
#include "TypesMacros.h" 

// Ouverture du port I2C en mode maitre, vitesse lente
void I2C_Init(void) {
    // Cablage liaison I2C est : SCL = PORTC(3)
    //                           SDA = PORTC(4)
    // Pas le choix, car 1 unique port I2C sur le PIC18F452
    // Cf. Datasheet PIC18F452, page 134
    //   Two pins are used for data transfer:
    //     • Serial clock (SCL) - RC3/SCK/SCL
    //     • Serial data (SDA) - RC4/SDI/SDA
    TRISCbits.RC3 = 0; // SCL en sortie
    TRISCbits.RC4 = 0; // SDA en sortie
    
    // Configuration du Registre de controle 1 : SSPCON1
    //     0 : WCOL sans collision
    //     0 : SSPOV sans overflow
    //     1 : SSPEN pour mise en marche du MSSP en autorisant SDA et SCL 
    //     0 : CKP pas utilise en mode maitre
    //     1000 : SSPM en mode maitre  (I2C Master mode)
    SSPCON1 = 0b00101000;

    // Configuration du Registre de controle 2 : SSPCON2
    SSPCON2 = 0x00; // Initialisation de tous les conditions 

    // Configuration du registre d'etat : SSPSTAT
    SSPSTAT = 0b10000000; // pas de Slew rate (vitesse lente 100kHz) 

    // Configuration du registre d'adresse / baud rate generator : SSPADD
    // Calcul du nombre permettant d'obtenir la bonne frequence sur SCL
	
    /* Configuration du baud rate generator : SSPADD
    (voir section 15.4.7)
    sachant que FOSC = 8Mhz, et la frequence clock 
    que l'on cherche a obtenir est = 100kHz */
    /* 1000 : clock = FOSC / (4 * (SSPADD+1)) */
    SSPADD = 0x13 ;
}

// Attente de l'acquittement en provenance de l'esclave
void I2C_AckSlave(void) {
    // Attente de la fin du transfert precedent: SSPSTAT.R_W
    // Attente de la reception de l'ack par l'esclave
    while ((SSPSTATbits.R_W) || (SSPCON2bits.ACKSTAT));
}

// Creation ACK par le maitre
void I2C_Ack(void) {
    SSPCON2bits.ACKDT = 0; // 0 = ACK
    SSPCON2bits.ACKEN = 1; // envoi de la valeur
    // Attente de la reception de l'ack par le maitre 
    while (SSPCON2bits.ACKEN);
}

// Creation NACK par le maitre
void I2C_NAck(void) {
    SSPCON2bits.ACKDT = 1; // 1 = NACK
    SSPCON2bits.ACKEN = 1; // envoi de la valeur
    // Attente de la reception de l'ack par le maitre 
    while (SSPCON2bits.ACKEN);
}

// Creation Start par le maitre 
void I2C_Start(void) {
    SSPCON2bits.SEN = 1; /* signal start */
    while (SSPCON2bits.SEN);
}

// Creation Stop par le maitre 
void I2C_Stop(void) {
    SSPCON2bits.PEN = 1; /* signal de stop */
    while (SSPCON2bits.PEN);
}

// Creation ReStart par le maitre 
void I2C_ReStart(void) {
    SSPCON2bits.RSEN = 1; /* signal restart */
    while (SSPCON2bits.RSEN); /* tant que le protocole n'est pas restart */
}

// Envoi d'un octet de donnee 
void I2C_Write(INT8U Data) {
    SSPBUF = Data; // Ecriture des donnees dans le buffer
    while (SSPSTATbits.BF); // Attendre tant que le buffer est plein
}

// Lecture d'un octet de donnee 
INT8U I2C_Read(void) {
    SSPCON2bits.RCEN = 1; // Autorise la reception sur bus I2C
    while (!SSPSTATbits.BF) ; // Attendre que le buffer se remplisse
    return (SSPBUF); // Lecture des donnees du buffer
}

