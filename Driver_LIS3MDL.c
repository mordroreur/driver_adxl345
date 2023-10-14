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
#include "Driver_I2C.h"

// variable globale de mode de communication
INT8U LIS3MDL_COM_MODE = 0; // 0 = I2C, 1 = SPI

/*
  Enables the LIS3MDL's magnetometer. Also:
  - Selects ultra-high-performance mode for all axes
  - Sets ODR (output data rate) to default power-on value of 10 Hz
  - Sets magnetometer full scale (gain) to default power-on value of +/- 4 gauss
  - Enables continuous conversion mode
  Note that this function will also reset other settings controlled by
  the registers it writes to.
  mode : the mode of communication (SPI or I2C) (LIS3MDL_SPI_MODE or LIS3MDL_I2C_MODE)
*/
void LIS3MDL_Init(INT8U mode)
{
    //Initialisation de la communication
    switch (mode) {
    case LIS3MDL_I2C_MODE:
	//Initialisation de la communication en I2C
	LIS3MDL_COM_MODE = LIS3MDL_I2C_MODE;
	I2C_Init();
	break;
    case LIS3MDL_SPI_MODE:
	//Initialisation de la communication en SPI
	LIS3MDL_COM_MODE = LIS3MDL_SPI_MODE;

	// Orientation des broches
	// 1: SPI idle mode / I2 C communication enabled;
	// 0: SPI communication mode / I2 C disabled)
	// valeur par defaut
	LIS3MDL_CS = 1;

	//Orientation des pins necessaires
	TRIS_LIS3MDL_CS = 0;

	//On se met en mode SPI
	LIS3MDL_CS = 1;

	SPI_Init();
	break;
    default:
	//Initialisation de la communication en I2C
	LIS3MDL_COM_MODE = LIS3MDL_I2C_MODE;
	I2C_Init();
	break;
    }

    //Temps pour etre sur qu'il soit initialisé (pas demandé dans la doc)
    Delay_ms(10);
  
    LIS3MDL_Write_Register(LIS3MDL_CTRL_REG1, LIS3MDL_SOFT_RESET_ENABLE);
  
    Delay_ms(10);
  
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
    LIS3MDL_Write_Register(LIS3MDL_CTRL_REG3, 0b00000000);

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

/*
 * Write a register of the LIS3MDL with the data given with modes SPI or I2C
 * Register : the register to read
 * Data : the data to write
 */
void LIS3MDL_Write_Register(INT8U Register, INT8U Data)
{
    switch (LIS3MDL_COM_MODE) {
    case LIS3MDL_SPI_MODE:
	LIS3MDL_CS = 0;  // Selection de la puce
	SPI_Write(0x00 | Register); // Ecriture de l'adresse du registre
	SPI_Write(Data); // Ecriture de la donnee
	LIS3MDL_CS = 1; // Deselection de la puce
	break;
    case LIS3MDL_I2C_MODE:
	I2C_Start();
	I2C_Write(LIS3MDL_I2C_CHIP_WRITE_0);
	I2C_AckSlave(); 
	I2C_Write(Register);
	I2C_AckSlave(); 
	I2C_Write(Data);
	I2C_AckSlave();
	I2C_Stop();
	break;
    default:
	// do nothing
	break;
    }
}

/*
 * Read a register of the LIS3MDL with modes SPI or I2C
 * Register : the register to read
 * return the data read
 */
INT8U LIS3MDL_Read_Register(INT8U Register)
{
    INT8U temp = 0x00;

    switch (LIS3MDL_COM_MODE) {
    case LIS3MDL_SPI_MODE:
	LIS3MDL_CS = 0; // Selection de la puce
	SPI_Write(0b10000000 | Register); // Ecriture de l'adresse du registre
	temp = SPI_Read(); // Lecture de la donnee
	LIS3MDL_CS = 1; // Deselection de la puce
	break;
    case LIS3MDL_I2C_MODE:
	I2C_Start();
	I2C_Write(LIS3MDL_I2C_CHIP_WRITE_0);
	I2C_AckSlave();
	I2C_Write(Register);
	I2C_AckSlave();
	I2C_ReStart();
	I2C_Write(LIS3MDL_I2C_CHIP_READ_0);
	I2C_AckSlave();
	temp = I2C_Read();
	I2C_NAck();
	I2C_Stop();
	break;
    default:
	// do nothing
	break;
    }
    
    return temp;
}

/*
 * Read the ID register of the LIS3MDL
 * return the ID read
 */
INT8U LIS3MDL_Read_ID()
{
    return LIS3MDL_Read_Register(LIS3DML_WHO_AM_I); // who am i
}

/*
 * Read the magnetics data register of the LIS3MDL
 * X_Mag : the data read on the X axis
 * Y_Mag : the data read on the Y axis
 * Z_Mag : the data read on the Z axis
 */
void LIS3MDL_Read_Magnetic_Data(INT16 *X_Mag, INT16 *Y_Mag, INT16 *Z_Mag)
{
    // Lecture des donnees sur 2 octets pour l'axe X
    *X_Mag = 0;
    *X_Mag = LIS3MDL_Read_Register(LIS3MDL_OUT_X_L); // LSB
    (*X_Mag) |= (LIS3MDL_Read_Register(LIS3MDL_OUT_X_H) << 8); // MSB

    // Lecture des donnees sur 2 octets pour l'axe Y
    *Y_Mag = 0;
    *Y_Mag = LIS3MDL_Read_Register(LIS3MDL_OUT_Y_L); // LSB
    (*Y_Mag) |= (LIS3MDL_Read_Register(LIS3MDL_OUT_Y_H) << 8); // MSB

    // Lecture des donnees sur 2 octets pour l'axe Z
    *Z_Mag = 0;
    *Z_Mag = LIS3MDL_Read_Register(LIS3MDL_OUT_Z_L); // LSB
    (*Z_Mag) |= (LIS3MDL_Read_Register(LIS3MDL_OUT_Z_H) << 8); // MSB
}

/*
 * Read the temperature data register of the LIS3MDL
 * Temperature : the data read for the temperature
 */
void LIS3MDL_Read_Temperature(INT16 *Temperature)
{
    // Lecture des donnees sur 2 octets pour la temperature
    *Temperature = 0;
    *Temperature = LIS3MDL_Read_Register(LIS3MDL_TEMP_OUT_L); // LSB
    (*Temperature) |= (LIS3MDL_Read_Register(LIS3MDL_TEMP_OUT_H) << 8); // MSB
}
