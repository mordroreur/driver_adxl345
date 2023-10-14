/*
 * File:   Driver_ADXL345.c
 * Author: coemprin
 *
 * Created on 2 octobre 2023, 16:47
 */


#define _Driver_ADXL345_C
#include <xc.h>
#include "TypesMacros.h"
#include "Driver_ADXL345.h"

void ADXL345_Init(void){

    I2C_Init();
    Delai_ms(500); // Delai de prise en compte, optionnel
    ADXL345_Write8(FIFO_CTL,0x9f);
    Delay_ms(10);    
    ADXL345_Write8(DATA_FORMAT,0x09);
    Delay_ms(10);  
    ADXL345_Write8(BW_RATE,0x0d);
    Delay_ms(10);        
    ADXL345_Write8(POWER_CTL,0x08);  // activate; //selection de la resolution: ici full resolution
    Delai_us(10);
    
}


void ADXL345_Write8(INT8U Registre, INT8U Donnee) {
  // Chronogramme d'ecriture d'un unique octet (page 19)
    
  I2C_Start();
  LATB = 0x01;
  I2C_Write(CHIP_Write);
  LATB = 0x03;
  I2C_AckSlave();
  LATB = 0x07;
  I2C_Write(Registre);
  LATB = 0x0F;
  I2C_AckSlave();
  LATB = 0x1F;
  I2C_Write(Donnee);
  LATB = 0x3F;
  I2C_AckSlave();
  LATB = 0x7F;
  I2C_Stop();
  LATB = 0xFF;
}

void ADXL345_Write(INT8U Registre, INT16 Donnee) {
// Chronogramme d'ecriture d'une donnee 16 bits
  I2C_Start();
  I2C_Write(CHIP_Write); 
  I2C_AckSlave(); 
  I2C_Write(Registre);
  I2C_AckSlave(); 
  I2C_Write((INT8U) Donnee >> 8);
  I2C_AckSlave();
  I2C_Write((INT8U) Donnee);
  I2C_AckSlave();
  I2C_Stop();

}

INT8U ADXL345_Read8(INT8U Registre) {
// Chronogramme de lecture d'un unique octet
    I2C_Start();
    I2C_Write(CHIP_Write);
    I2C_AckSlave();
    I2C_Write(Registre);
    I2C_AckSlave();
    I2C_ReStart();
    I2C_Write(CHIP_Read);
    I2C_AckSlave();
    INT8U ValeurLue = I2C_Read();
    I2C_NAck();
    I2C_Stop();
    
    return(ValeurLue);
}


INT16 ADXL345_Read(INT8U Registre) {
// Chronogramme de lecture d'une donnee 16 bits
    INT16 ValeurLue; // Valeur a retourner
  
    I2C_Start();
    I2C_Write(CHIP_Write);
    I2C_AckSlave();
    I2C_Write(Registre);
    I2C_AckSlave();
    I2C_ReStart();
    
    I2C_Write(CHIP_Read);
    I2C_AckSlave();
    ValeurLue = I2C_Read();
    I2C_Ack();
    ValeurLue = ValeurLue <<8;
    ValeurLue = ValeurLue | I2C_Read();
    I2C_NAck();
    I2C_Stop();
    return(ValeurLue);
}

void ADXL345_GetAcceleration8(INT16U * DATAx, INT16U * DATAy ,INT16U * DATAz){
    *DATAx = ADXL345_Read8(DATAX0);
    //*DATAx|= (ADXL345_Read8(DATAX1)<<8);
    /*
    *DATAy = ADXL345_Read8(DATAY0);
    *DATAy|= (ADXL345_Read8(DATAY1)<<8);
    
    *DATAz = ADXL345_Read8(DATAZ0);
    *DATAz|= (ADXL345_Read8(DATAZ1)<<8);
     */
}

