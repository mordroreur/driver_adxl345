#include "Driver_LIS3MDL.h"
#include "Driver_SPI.h"
#include <math.h>

INT8U device_name;

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
	LIS3MDL_CS = 0;
	TRIS_LIS3MDL_CS = 0;
    LIS3MDL_CS = 1;
	
	SPI_Init();
	
	Delay_us(5);
/*	LIS3MDL_CS = 0;
	SPI_Write(LIS3DML_WHO_AM_I);
	device_name = SPI_Read();
	LIS3MDL_CS = 1; */
	
    // 0x70 = 0b11110000
    // OM = 11 (ultra-high-performance mode for X and Y); DO = 100 (10 Hz ODR)
    //LIS3MDL_SendCmd(LIS3MDL_CTRL_REG1, 0xF0);
    LIS3MDL_SendCmd(LIS3MDL_CTRL_REG1, 0b10010000);

    // 0x00 = 0b00000000
    // FS = 00 (+/- 4 gauss full scale)
    //LIS3MDL_SendCmd(LIS3MDL_CTRL_REG2, 0x00);
    //LIS3MDL_SendCmd(LIS3MDL_CTRL_REG2, 0x00);

    // 0x00 = 0b00000000
    // MD = 00 (continuous-conversion mode)
    LIS3MDL_SendCmd(LIS3MDL_CTRL_REG3, 0x00);

    // 0x0C = 0b00001100
    // OMZ = 11 (ultra-high-performance mode for Z)
    //LIS3MDL_SendCmd(LIS3MDL_CTRL_REG4, 0x0C);

    // 0x40 = 0b01000000
    // BDU = 1 (block data update)
    //LIS3MDL_SendCmd(LIS3MDL_CTRL_REG5, 0x40);
}

void LIS3MDL_SendCmd(INT8U Register, INT8U Data) {
  LIS3MDL_CS = 0;      // Selection de la puce
  Delay_us(1);
  SPI_Write(0x00 | Register);
  SPI_Write(Data);
  Delay_us(1);
  LIS3MDL_CS = 1;
  Delay_us(10);
}

INT8U LIS3MDL_GetTemp()
{
    INT8U temp = 0xa0;
	LIS3MDL_CS = 0;      // Selection de la puce
    Delay_us(1);
	
    SPI_Write(0b10000000 | LIS3DML_WHO_AM_I);
	temp = SPI_Read();
    
    Delay_us(1);
	LIS3MDL_CS = 1;
    Delay_us(10);
    return temp;
}

/*
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