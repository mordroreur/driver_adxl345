/****************************************************************************/
/*  Groupe C                                                      ISIMA     */
/*  Octobre 2023                                                            */
/*                                                                          */
/*                                                                          */
/*                 Programme principale de test de LIS3MDL                  */
/*                                                                          */
/* groupe_.c                      MPLAB X                      PIC 18F452   */
/****************************************************************************/

// Configuration physique de la puce
#include <xc.h>
#include "ConfigBits_P18F.h"

// Macros et prototypes
#include "Driver_GLCD_S0108.h"
#include "Driver_LIS3MDL.h"
// #include "Driver_ADXL345.h"
#include "TypesMacros.h"

//initialisation des variables globales du fichiers
static INT16 temperature = 0;
static INT16 mag_x = 0;
static INT16 mag_y = 0;
static INT16 mag_z = 0;

int main(void)
{ 
    //Valeur de basepar defaut sur les pins
    LATA = 0;
    LATB = 0;
    LATC = 0;
    LATD = 0;
    LATE = 0;

    //orientation des pins(toute en sortie, les drivers changeront si besoins)
    TRISA = 0;
    TRISB = 0;
    TRISC = 0;
    TRISD = 0;
    TRISE = 0;
  
    //initailisation des drivers nécessaires
    LIS3MDL_Init(LIS3MDL_I2C_MODE);
    // LIS3MDL_Init(LIS3MDL_SPI_MODE);
    GLCD_Init();
  
    //On utilise D pour le clignotement
    //LATD = 0xAA;
    
    // Animation avec un clignotement ainsi que l'affichage de la temperature et des donnees magnetiques
    while (1) {
	LIS3MDL_Read_Temperature(&temperature); // lecture de la temperature
	LIS3MDL_Read_Magnetic_Data(&mag_x, &mag_y, &mag_z); // lecture des donnees magnetiques

	temperature = (temperature/8) + 25; // recalcul de la temperature

	//Affichage sur les leds
	//LATB = (temperature>>8);
	//LATB = (INT8U) temperature;
	//LATB = LIS3MDL_Read_ID();
	//LATD = ~PORTD;
    
	GLCD_Jauge_Acc((INT8U)(mag_x/8), (INT8U)(mag_y/8), (INT8U)(mag_z/8), (INT8U)(temperature));

	// on attend pour ne pas surcharger le capteur
	Delay_ms(500);
    }
    return 0;
}
