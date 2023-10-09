/****************************************************************************/
/*  Groupe C                                                      ISIMA     */
/*  Octobre 2023                                                            */
/*                                                                          */
/*                                                                          */
/*                     Driver pour le magnetometre LIS3MDL                  */
/*                                                                          */
/* Driver_LIS3MDL.c                 MPLAB X                    PIC 18F542   */
/****************************************************************************/
#include <math.h>
#include "Driver_LIS3MDL.h"
#include "Driver_SPI.h"


/*
Enables the LIS3MDL's magnetometer. Also:
- Selects ultra-high-performance mode for all axes
- Sets ODR (output data rate) to default power-on value of 10 Hz
- Sets magnetometer full scale (gain) to default power-on value of +/- 4 gauss
- Enables continuous conversion mode
Note that this function will also reset other settings controlled by
the registers it writes to.
*/
void LIS3MDL_Init(void)
{
  // Orientation des broches
  // 1: SPI idle mode / I2 C communication enabled;
  // 0: SPI communication mode / I2 C disabled)
  // valeur par defaut
  LIS3MDL_CS = 0;

  //Orientation des pins necessaires
  TRIS_LIS3MDL_CS = 0;

  //On se met en mode SPI
  LIS3MDL_CS = 1;

  //Initialisation de la communication en SPI
  SPI_Init();

  //Temps pour etre sur qu'il soit initialisé(pas demandé dans la doc)
  Delay_us(5);

  //recuperation du nom de la bete
  /*	LIS3MDL_CS = 0;
	SPI_Write(LIS3DML_WHO_AM_I);
	device_name = SPI_Read();
	LIS3MDL_CS = 1; */
  
  //registre 1 :
  //TEMP_EN : 1 = Temperature sensor enable
  //OM1 : 0 = X/Y axis on low power mode
  //OM0 : 0 = X/Y axis on low power mode
  //DO2 : 1 = Output data rate 10Hz
  //DO1 : 0 = Output data rate 10Hz
  //DO0 : 0 = Output data rate 10Hz
  //FAST_ODR : 0 = disable data rate higher than 80Hz
  //ST : 0 = self Test disable
  LIS3MDL_Write_Register(LIS3MDL_CTRL_REG1, 0b10010000);

  //registre 2 :
  //'0' : 0 = need to be 0
  //FS1 : 0 = +- 4 gauss
  //FS0 : 0 = +- 4 gauss
  //'0' : 0 = need to be 0
  //REBOOT : 0 = normal reboot 
  //SOFT_RST : 0 = Default value
  //'0' : 0 = need to be 0
  //'0' : 0 = need to be 0
  LIS3MDL_Write_Register(LIS3MDL_CTRL_REG2, 0b00000000);

  //registre 3 :
  //'0' : 0 = need to be 0
  //'0' : 0 = need to be 0
  //LP : 0 = disable low power mode
  //'0' : 0 = need to be 0
  //'0' : 0 = need to be 0
  //SIM : 0 = 4 wire SPI-mode
  //MD1 : 1 = Default value // operating mode : Power-down mode
  //MD0 : 1 = Default value // operating mode : Power-down mode
  LIS3MDL_Write_Register(LIS3MDL_CTRL_REG3, 0b00000011);

  //registre 4 :
  //'0' : 0 = need to be 0
  //'0' : 0 = need to be 0
  //'0' : 0 = need to be 0
  //'0' : 0 = need to be 0
  //OMZ1 : 0 = Z operative mode
  //OMZ0 : 0 = Z operative mode
  //BLE : 0 = LSB at lower address
  //'0' : 0 = need to be 0
  LIS3MDL_Write_Register(LIS3MDL_CTRL_REG4, 0b00000000);

  //registre 5 :
  //FAST_READ : 0 Disable(don t allow reading the hight part only)
  //BDU : 0 = continuous magnetic data update
  //'0' : 0 = need to be 0
  //'0' : 0 = need to be 0
  //'0' : 0 = need to be 0
  //'0' : 0 = need to be 0
  //'0' : 0 = need to be 0
  //'0' : 0 = need to be 0
  LIS3MDL_Write_Register(LIS3MDL_CTRL_REG5, 0b00000000);
}

// TODO : lock clock at 1 when not in use
void LIS3MDL_Write_Register(INT8U Register, INT8U Data) {
  LIS3MDL_CS = 0;      // Selection de la puce
  SPI_Write(0x00 | Register);
  SPI_Write(Data);
  LIS3MDL_CS = 1;
}

// TODO : lock clock at 1 when not in use
INT8U LIS3MDL_Read_Register(INT8U Register){
    INT8U temp = 0xa0;
    LIS3MDL_CS = 0;      // Selection de la puce
	
    SPI_Write(0b10000000 | Register);
    temp = SPI_Read();
    
    LIS3MDL_CS = 1;
    return temp;
}

INT8U LIS3MDL_Read_ID(){
  return LIS3MDL_Read_Register(LIS3DML_WHO_AM_I);
}

void LIS3MDL_Read_Magnetic_Data(INT16 *X_Mag, INT16 *Y_Mag, INT16 *Z_Mag){
  //TODO
}

void LIS3MDL_Read_Temperature(INT16U *Temperature){
  *Temperature = 0;
  *Temperature = LIS3MDL_Read_Register(LIS3MDL_TEMP_OUT_L);
  *Temperature |= (INT8U) (LIS3MDL_Read_Register(LIS3MDL_TEMP_OUT_H) << 8);
}




/*  En I2C
// Writes a mag register
void LIS3MDL_writeReg(INT8U reg, INT8U value)
{
	I2C_Start(address);
	I2C_Write(reg);
	I2C_Write(value);
	I2C_Stop();
}

// Reads a mag register
INT8U LIS3MDL_readReg(INT8U reg)
{
	INT8U value;

	I2C_Start(address);
	I2C_Write(reg);
	I2C_Stop();

	Wire.requestFrom(address, (INT8U)1);
	value = I2C_Read();

	return value;
}

// Reads the 3 mag channels and stores them in vector m
void LIS3MDL_read(INT8U * x, INT8U * y, INT8U * z)
{
  I2C_Start(address);
  // assert MSB to enable subaddress updating
  I2C_Write(OUT_X_L | 0x80);
  I2C_Stop();

  Wire.requestFrom(address, (INT8U)6);
  INT8U xlm = I2C_Read();
  INT8U xhm = I2C_Read();
  INT8U ylm = I2C_Read();
  INT8U yhm = I2C_Read();
  INT8U zlm = I2C_Read();
  INT8U zhm = I2C_Read();

  // combine high and low bytes
  *x = (INT16U)(xhm << 8 | xlm);
  *y = (INT16U)(yhm << 8 | ylm);
  *z = (INT16U)(zhm << 8 | zlm);
}



void LIS3MDL_vector_normalize(vector<float> *a)
{
  float mag = sqrt(vector_dot(a, a));
  a->x /= mag;
  a->y /= mag;
  a->z /= mag;
}

// Private Methods //////////////////////////////////////////////////////////////

INT16U LIS3MDL_testReg(INT8U address, regAddr reg)
{
  I2C_Start(address);
  I2C_Write((INT8U)reg);
  if (I2C_Stop() != 0)
  {
    return TEST_REG_ERROR;
  }

  Wire.requestFrom(address, (INT8U)1);
  if (Wire.available())
  {
    return I2C_Read();
  }
  else
  {
    return TEST_REG_ERROR;
  }
}

*/
