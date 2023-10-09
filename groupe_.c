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
#include "TypesMacros.h"

int main(void) {
  //initialisation des variables
  INT16 temperature = 0;
  INT16 mag_x = 0;
  INT16 mag_y = 0;
  INT16 mag_z = 0;
  
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
  LIS3MDL_Init();
  GLCD_Init();
  
  
  //On utilise D pour le clignotement
  //LATD = 0xAA;

    
  // Animation avec un clignotement ainsi que affichage de la temperature
  while (1) {
    LIS3MDL_Read_Temperature(&temperature);
    LIS3MDL_Read_Magnetic_Data(&mag_x, &mag_y, &mag_z);
    //Affichage sur les leds TODO : changer pour mettre sur le GLCD
    //LATB = (temperature>>8);
    //LATB = (INT8U) temperature;
    //LATB = LIS3MDL_Read_ID();
    //LATD = ~PORTD;
    
    
    
    temperature = (temperature/8) + 25;
    GLCD_Jauge_Acc((INT8U)(mag_x/8), (INT8U)(mag_y/8), (INT8U)(mag_z/8), (INT8U)(temperature));
    // on attend pour ne pas surcharger le capteur
    Delay_ms(500);
  }
  return 0;
}
