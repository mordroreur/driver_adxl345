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
    
    // MODE I2C
    if(mode_I2C){
        I2C_Init();
        Delai_ms(5); // Delai de prise en compte, optionnel
        
    }
    else { //MODE SPI
        SPI_Init();
    }
    
}


void ADXL345_Write8(INT8U Registre, INT8U Donnee) {
  // Chronogramme d'ecriture d'un unique octet (page 19)
  I2C_Start();
  I2C_Write(CHIP_Write); 
  I2C_AckSlave(); 
  I2C_Write(Registre);
  I2C_AckSlave(); 
  I2C_Write(Donnee);
  I2C_AckSlave();
  I2C_Stop();
}

void ADXL345_Write(INT8U Registre, INT16 Donnee) {
// Chronogramme d'ecriture d'une donnee 16 bits
  I2C_Start();
  I2C_Write(CHIP_Write); 
  I2C_AckSlave(); 
  I2C_Write(Registre);
  I2C_AckSlave(); 
  I2C_Write(Donnee >> 8);
  I2C_AckSlave();
  I2C_Write(Donnee);
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

INT8U ADXL345_ReadByte(INT8U Register) {
  INT8U ValeurLue;
  PE_CS = 0;      // Selection de la puce
  
  SPI_Write(0b01000001);
  ValeurLue = SPI_ByteTransfert(Register);
   
  PE_CS = 1;
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
