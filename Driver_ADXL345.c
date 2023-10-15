/****************************************************************************/
/*  Groupe C                                                      ISIMA     */
/*  Octobre 2023                                                            */
/*                                                                          */
/*                                                                          */
/*                      Driver de l'accelerometre ADXL345                   */
/*                                                                          */
/* Driver_ADXL345.c                 MPLAB X                    PIC 18F452   */
/****************************************************************************/

#define _Driver_ADXL345_C
#include <xc.h>
#include "TypesMacros.h"
#include "Driver_ADXL345.h"

static INT8U value8u;
static INT16U value16u;

/*
 * Init the ADXL345 with I2C
 */
void ADXL345_Init(void)
{
    I2C_Init(); // Initialisation de la liaison I2C
    Delay_ms(10); // Delai de prise en compte, optionnel

    /* Configuration des registres de controle de l'accelerometre */
    ADXL345_Write8(FIFO_CTL,0x9f); // FIFO mode stream
    Delay_ms(10); // Delai de prise en compte, optionnel
    ADXL345_Write8(DATA_FORMAT,0x09); // full resolution
    Delay_ms(10); // Delai de prise en compte, optionnel
    ADXL345_Write8(BW_RATE,0x0d); // 100Hz
    Delay_ms(10); // Delai de prise en compte, optionnel
    ADXL345_Write8(POWER_CTL,0x08);  // activate
    Delay_ms(10); // Delai de prise en compte, optionnel
}

/*
 * Write a register of the ADXL345 with the data given with I2C
 * Registre : the register to read
 * Donnee : the data to write
 */
void ADXL345_Write8(INT8U Registre, INT8U Donnee)
{
    // Chronogramme d'ecriture d'un unique octet (page 19)
    
    I2C_Start();
    LATB = 0x01; // Start OK -> first led on
    I2C_Write(CHIP_Write);
    LATB = 0x03; // Chip write OK -> second led on

    /* !!! No Acknowledge from Slave */
  
    I2C_AckSlave();
    LATB = 0x07; // AckSlave OK -> third led on
    I2C_Write(Registre);
    LATB = 0x0F; // Write OK -> fourth led on
    I2C_AckSlave();
    LATB = 0x1F; // AckSlave OK -> fifth led on
    I2C_Write(Donnee);
    LATB = 0x3F; // Write OK -> sixth led on
    I2C_AckSlave();
    LATB = 0x7F; // AckSlave OK -> seventh led on
    I2C_Stop();
    LATB = 0xFF; // Stop OK -> eighth led on
}

/*
 * Write a register of the ADXL345 with the data (2 bytes) given with I2C
 * Registre : the register to write
 * Donnee : 2 bytes data to write
 */
void ADXL345_Write(INT8U Registre, INT16 Donnee)
{
    // Chronogramme d'ecriture d'une donnee 16 bits
    I2C_Start();
    I2C_Write(CHIP_Write); 
    I2C_AckSlave(); 
    I2C_Write(Registre);
    I2C_AckSlave(); 
    I2C_Write((INT8U) Donnee >> 8); // MSB first
    I2C_AckSlave();
    I2C_Write((INT8U) Donnee); // LSB second
    I2C_AckSlave();
    I2C_Stop();

}

/*
 * Read a register of the ADXL345 with I2C
 * Registre : the register to read
 * return the data read
 */
INT8U ADXL345_Read8(INT8U Registre)
{
    // Chronogramme de lecture d'un unique octet
    I2C_Start();
    I2C_Write(CHIP_Write);
    I2C_AckSlave();
    I2C_Write(Registre);
    I2C_AckSlave();
    I2C_ReStart();
    I2C_Write(CHIP_Read);
    I2C_AckSlave();
    value8u = I2C_Read();
    I2C_NAck();
    I2C_Stop();

    return value8u;
}

/*
 * Read two consecutive registers of the ADXL345 with I2C
 * Registre : the first register to read
 * return the 2 bytes data read
 */
INT16 ADXL345_Read(INT8U Registre)
{
    // Chronogramme de lecture d'une donnee 16 bits

    /* Demande lecture sur 2 octets */
    I2C_Start();
    I2C_Write(CHIP_Write);
    I2C_AckSlave();
    I2C_Write(Registre);
    I2C_AckSlave();
    I2C_ReStart();

    /* Lecture des donnees */
    I2C_Write(CHIP_Read);
    I2C_AckSlave();
    value16u = I2C_Read();
    I2C_Ack();
    value16u = value16u << 8; // MSB first
    value16u = value16u | I2C_Read(); // LSB second
    I2C_NAck();
    I2C_Stop();
    
    return value16u;
}

/*
 * Read the acceleration data register of the ADXL345
 * ax : the data read on the X axis
 * ay : the data read on the Y axis
 * az : the data read on the Z axis
 */
void ADXL345_GetAcceleration8(INT16U *ax, INT16U *ay, INT16U *az)
{
    // Lecture des donnees sur 2 octets pour l'axe X
    *ax = ADXL345_Read8(DATAX0);
    *ax|= (ADXL345_Read8(DATAX1)<<8);

    // Lecture des donnees sur 2 octets pour l'axe Y
    *ay = ADXL345_Read8(DATAY0);
    *ay|= (ADXL345_Read8(DATAY1)<<8);

    // Lecture des donnees sur 2 octets pour l'axe Z
    *az = ADXL345_Read8(DATAZ0);
    *az|= (ADXL345_Read8(DATAZ1)<<8);
}
