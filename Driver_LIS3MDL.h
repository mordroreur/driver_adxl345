/****************************************************************************/
/*  MESNARD Emmanuel                                               ISIMA    */
/*  Octobre 2020                                                            */
/*                     Driver pour le magnetometre LIS3MDL                  */
/*                                                                          */
/* Driver_LIS3MDL.h                 MPLAB X                    PIC 18F542   */
/****************************************************************************/

#ifndef Driver_LIS3MDL_H
#define Driver_LIS3MDL_H
#include <xc.h>
#include "TypesMacros.h"


//================================================================   
//    Connection des broches et connectique sur carte EasyPIC 6.0
//================================================================

// Broches du LIS3MDL
// 1   SCL,SPC    I2C serial clock (SCL), 
//                SPI serial port clock (SPC)
// 2 Reserved Connect to GND
// 3 GND Connect to GND
// 4 C1 Capacitor connection (C1=100 nF)
// 5 Vdd Power supply 
// 6 Vdd_IO Power supply for I/O pins
// 7 INT Interrupt
// 8 DRDY Data Ready
// 9 SDO/SA1   SPI serial data output (SDO), 
//             I2C less significant bit of the device address (SA1)
// 10 CS SPI enable I2C/SPI mode selection
//    1: SPI idle mode / I2C communication enabled;
//    0: SPI communication mode / I2C disabled)
// 11 SDA,SDI,SDO   I2C serial data (SDA)
//                  SPI serial data input (SDI)
//                  3-wire interface serial data output (SDO)
// 12 Reserved Connect to GND


// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
//
// ATTENTION : Brochage du connecteur du composant POLOLU
// VDD : TENSION REGULEE en SORTIE !!
// Vin : tension d'alimentation principale, entre 2.5 et 5.5 V
//       qui est la tension d'ENTREE
//
// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!


// Utilisation en mode SPI
// -----------------------
    // Cablage liaison SPI est : SCK = PORTC(3) = SCL du connecteur
    //                           SDI = PORTC(4) = SDA du connecteur
    //                           SDO = PORTC(5) = SDO du connecteur
    // Pas le choix, car 1 unique port SPI sur le PIC18F452
    // Cf. Datasheet PIC18F452, page 125
 

// Broche CS = PORTC(2) : A mettre a zero lors d'un transfert SPI
#define TRIS_LIS3MDL_CS   TRISCbits.TRISC2
#define LIS3MDL_CS        LATCbits.LATC2


// Utilisation en mode I2C
// -----------------------
// LIS3DML chip identification (Datasheet page 17) pour I2C
// I2C SAD, Slave ADress = 0b0011 1x0Rw
// x = 0 ou 1, selon broche SDO/SA1
#define LIS3MDL_CHIP_ID              (0x3D)
#define LIS3MDL_CHIP_ADDR            (0x1C)
#define LIS3MDL_I2C_ADDRESS_LOW      (0x38)  // SAD[0] = 0
#define LIS3MDL_I2C_ADDRESS_HIGH     (0x3C)  // SAD[0] = 1
#define LIS3MDL_I2C_CHIP_WRITE_0     (0x38)
#define LIS3MDL_I2C_CHIP_READ_0      (0x39)
#define LIS3MDL_I2C_CHIP_WRITE_1     (0x3C)
#define LIS3MDL_I2C_CHIP_READ_1      (0x3D)


//================================================================  
// Definition des Registres Internes du magnetometre (page 23)
//================================================================  

// LIS3DML device identification register (p24))
#define LIS3DML_WHO_AM_I             (0x0F)

// LIS3DML control register
#define LIS3MDL_CTRL_REG1            (0x20)
#define LIS3MDL_CTRL_REG2            (0x21)
#define LIS3MDL_CTRL_REG3            (0x22)
#define LIS3MDL_CTRL_REG4            (0x23)
#define LIS3MDL_CTRL_REG5            (0x24)
 

// LIS3DML status register
#define LIS3MDL_STATUS_REG           (0x27)

// LIS3DML magnometer output register
#define LIS3MDL_OUT_X_L              (0x28)
#define LIS3MDL_OUT_X_H              (0x29)
#define LIS3MDL_OUT_Y_L              (0x2A)
#define LIS3MDL_OUT_Y_H              (0x2B)
#define LIS3MDL_OUT_Z_L              (0x2C)
#define LIS3MDL_OUT_Z_H              (0x2D)


// LIS3DML temperature output register
#define LIS3MDL_TEMP_OUT_L           (0x2E)
#define LIS3MDL_TEMP_OUT_H           (0x2F)

// LIS3DML interrupt register
#define LIS3MDL_INT_CFG              (0x30)
#define LIS3MDL_INT_SRC              (0x31)
#define LIS3MDL_INT_THS_L            (0x32)
#define LIS3MDL_INT_THS_H            (0x33)


//================================================================  
// Definition des controles du magnetometre (registres en ecriture)
//================================================================  


// Registre CTRL_REG1 (page 24)
// ---------------------------

// LIS3DML Temperature Sensor Control (CTRL_REG1[7]) page 24
#define LIS3MDL_TEMPSENSOR_ENABLE        (0x80)
#define LIS3MDL_TEMPSENSOR_DISABLE       (0x00)

// LIS3DML_XY-axis Operating Mode (CTRL_REG1[6:5]) page 25
#define LIS3MDL_OM_XY_LOWPOWER           (0x00)
#define LIS3MDL_OM_XY_MEDIUM             (0x20)
#define LIS3MDL_OM_XY_HIGH               (0x40)
#define LIS3MDL_OM_XY_ULTRAHIGH          (0x60)
   
// LIS3DML Output Data Rate  (CTRL_REG1[4:2]) page 25
#define LIS3MDL_ODR_0_625_HZ             (0x00)  // Output Data Rate = 0.625 Hz
#define LIS3MDL_ODR_1_25_HZ              (0x04)  // = 1.25 Hz  
#define LIS3MDL_ODR_2_5_HZ               (0x08)  // = 2.5 Hz
#define LIS3MDL_ODR_5_0_HZ               (0x0C)  // = 5 Hz
#define LIS3MDL_ODR_10_HZ                (0x10)  // Valeur par defaut : 10 Hz
#define LIS3MDL_ODR_20_HZ                (0x14) 
#define LIS3MDL_ODR_40_HZ                (0x18)  
#define LIS3MDL_ODR_80_HZ                (0x1C)

// LIS3DML Fast Output Data Rate  (CTRL_REG1[1]) page 24
#define LMS303C_FODR_DISABLE             (0x00)     
#define LMS303C_FODR_ENABLE              (0x02)

// LIS3DML Self Test  (CTRL_REG1[0]) page 24
#define LMS303C_SELFTEST_DISABLE         (0x00)     
#define LMS303C_SELFTEST_ENABLE          (0x01)


// Registre CTRL_REG2 (page 25)
// ---------------------------

// LIS3DML Full Scale (CTRL_REG2[6:5]) page 25
#define LIS3MDL_FS_DEFAULT               (0x00)
#define LIS3MDL_FS_4_GA                  (0x00)  // Full scale = �4 Gauss
#define LIS3MDL_FS_8_GA                  (0x20)
#define LIS3MDL_FS_12_GA                 (0x40)  
#define LIS3MDL_FS_16_GA                 (0x60)  // Full scale = �16 Gauss

// LIS3DML Reboot (CTRL_REG2[3]) page 25
#define LIS3MDL_REBOOT_DEFAULT           (0x00)  // Normal mode 
#define LIS3MDL_REBOOT_ENABLE            (0x08)  // Reboot memory content
   
// LIS3DML Soft reset (CTRL_REG2[2]) page 25
#define LIS3MDL_SOFT_RESET_DEFAULT       (0x00)
#define LIS3MDL_SOFT_RESET_ENABLE        (0x04)  // Reset Operation


// Registre CTRL_REG3 (page 26)
// ---------------------------
     
// LIS3DML Lowpower mode configuration (CTRL_REG3[5]) page 26
#define LIS3MDL_CONFIG_NORMAL_MODE       (0x00)
#define LIS3MDL_CONFIG_LOWPOWER_MODE     (0x20)
   
// LIS3DML SPI Communication Mode (CTRL_REG3[2]) page 22 et page 26
#define LIS3MDL_SIM_4_WIRE               (0x00)  // SPI classique par defaut
#define LIS3MDL_SIM_3_WIRE               (0x04)

// LIS3DML Operation Mode (CTRL_REG3[1:0]) page 26
#define LIS3MDL_SELECTION_MODE           (0x03)
#define LIS3MDL_CONTINUOUS_MODE          (0x00)
#define LIS3MDL_SINGLE_MODE              (0x01)
#define LIS3MDL_POWERDOWN1_MODE          (0x02)
#define LIS3MDL_POWERDOWN2_MODE          (0x03)


// Registre CTRL_REG4 (page 26)
// ---------------------------

// LIS3DML Z-axis Operation Mode (CTRL_REG4[3:2]) page 26 et page 27
#define LIS3MDL_OM_Z_LOWPOWER            (0x00)
#define LIS3MDL_OM_Z_MEDIUM              (0x04)
#define LIS3MDL_OM_Z_HIGH                (0x08)
#define LIS3MDL_OM_Z_ULTRAHIGH           (0x0C)   

// LIS3DML Big Little Endian selection (CTRL_REG4[1]) page 26 
#define LIS3MDL_BLE_LSB                  (0x00)
#define LIS3MDL_BLE_MSB                  (0x02)



// Registre CTRL_REG5 (page 27)
// ---------------------------

// LIS3DML Fast Read (CTRL_REG5[7]) page 27
#define LMS303C_FAST_READ_DISABLE        (0x00)     
#define LMS303C_FAST_READ_ENABLE         (0x80)

// LIS3DML Blokc Data Update for magnetic data (CTRL_REG5[6]) page 27
#define LIS3MDL_BDU_CONTINUOUS           (0x00)
#define LIS3MDL_BDU_MSBLSB               (0x40)
   

// Registre INT_CFG (page 29)
// -------------------------

// LIS3DML Enable Interrupt Generation (INT_CFG[7:5]) page 29
#define LIS3MDL_MASK_INT_CFG_XIEN         (0x80)
#define LIS3MDL_MASK_INT_CFG_YIEN         (0x40)
#define LIS3MDL_MASK_INT_CFG_ZIEN         (0x20)

// LIS3DML Interrupt configuration (INT_CFG[2:0]) page 29
#define LIS3MDL_MASK_INT_CFG_IEA          (0x04)
#define LIS3MDL_MASK_INT_CFG_LIR          (0x02)
#define LIS3MDL_MASK_INT_CFG_IEN          (0x01)



// Registre INT_SRC (page 29)
// --------------------------

#define LIS3MDL_MASK_INT_SRC_PTH_X        (0x80)
#define LIS3MDL_MASK_INT_SRC_PTH_Y        (0x40)
#define LIS3MDL_MASK_INT_SRC_PTH_Z        (0x20)
#define LIS3MDL_MASK_INT_SRC_NTH_X        (0x10)
#define LIS3MDL_MASK_INT_SRC_NTH_Y        (0x08)
#define LIS3MDL_MASK_INT_SRC_NTH_Z        (0x04)
#define LIS3MDL_MASK_INT_SRC_MROI         (0x02)
#define LIS3MDL_MASK_INT_SRC_INT          (0x01)


//================================================================   
//         Fonctions de gestion du magnetometre LIS3MDL
//================================================================

// Ouverture du LIS3MDL en mode SPI
void LIS3MDL_Init(void);

// Lecture et ecriture dans les registres
// INT8U LIS3MDL_Read_Register(INT8U Registre);
// void LIS3MDL_Write_Register(INT8U Registre, INT8U Donnee);

// Lecture des donnees
// INT8U LIS3MDL_Read_ID();
// void LIS3MDL_Read_Magnetic_Data(INT16 *X_Mag, INT16 *Y_Mag, INT16 *Z_Mag);
// void LIS3MDL_Read_Temperature(INT16 *Temperature);

void LIS3MDL_SendCmd(INT8U Register, INT8U Data);
INT8U LIS3MDL_GetTemp();

INT8 permut(INT8 R);
#endif 