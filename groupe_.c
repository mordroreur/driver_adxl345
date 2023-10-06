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
  INT16U temperature = 0;
  
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
  
  
  //On utilise D pour le clignotement
  LATD = 0xAA;

    
  // Animation avec un clignotement ainsi que affichage de la temperature
  while (1) {
    LIS3MDL_Read_Temperature(&temperature);
    //Affichage sur les leds TODO : changer pour mettre sur le GLCD
    LATB = (temperature>>8);
    LATD = ~PORTD;

    // on attend pour ne pas surcharger le capteur
    Delay_ms(500);
  }
  return 0;
}
