/****************************************************************************/
/*  MESNARD Emmanuel                                              ISIMA     */
/*  Octobre 2020                                                            */
/*                                                                          */
/*       Test de l'afficheur Graphique LCD (GLCD) avec controleur S108      */
/*                                                                          */
/* TD_SE_7_GLCD_S0108.c                MPLAB X                 PIC 18F542   */
/****************************************************************************/
// Configuration physique du composant
#include <xc.h>
#include "ConfigBits_P18F.h"
// Macros et prototypes
#include "TypesMacros.h"
#include "Driver_LIS3MDL.h"

int main(void) {

    LIS3MDL_Init();
    
    LATB = 0;
    TRISB = 0;
    LATA = 0;
    TRISA = 0;
    LATA = 0xAA;
    // Animation avec un clignotement
    while (1) { // sur 2 caracteres
        LATB = LIS3MDL_GetTemp();
        Delay_ms(500);
        LATA = ~LATA;
    }
    return 0;
}
