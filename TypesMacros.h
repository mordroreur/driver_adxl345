/****************************************************************************/
/*  MESNARD Emmanuel                                              ISIMA     */
/*  Septembre 2020                                                          */
/*                                                                          */
/*                  Types et macros pour carte EasyPIC 6.0                  */
/*                                                                          */
/* TypesMacros.h                      MPLAB X                  PIC 18F542   */
/****************************************************************************/

#ifndef	_TypesMacros_H

#define _TypesMacros_H

// Redefinitions de type
// *********************
#define INT8 char
#define CHAR char
#define INT16 int                     
#define INT8U unsigned char           
#define INT16U unsigned int   
#define REAL float         
#define BOOLEAN unsigned char           
#define ROM_INT8U const unsigned char 
#define ROM_CHAR const char

// Constantes generales
// ********************
#define TRUE   1
#define FALSE  0

// Definition de macros temporelles 
// ********************************
// Frequence du quartz sur la carte
#define _XTAL_FREQ 8000000   // 8 Mhz
//#define __delay_us(x) _delay((unsigned long)((x)*(_XTAL_FREQ/4000000.0)))
//#define __delay_ms(x) _delay((unsigned long)((x)*(_XTAL_FREQ/4000.0)))
// Deja presentes dans pic18.h
#define Delai_us(x) _delay((unsigned long)((x)* 2.0))
#define Delai_ms(x) _delay((unsigned long)((x)* 2000.0))
#define Delay_us(x) _delay((unsigned long)((x)* 2.0))
#define Delay_ms(x) _delay((unsigned long)((x)* 2000.0))

// Definition de macros generales
// ******************************
#define MAXIMUM(A,B) ((A)>(B)) ? (A) : (B)

// Manipulations de bit sur des variables
// --------------------------------------
//     Deux versions : avec ou sans la lettre T (pour Test)
//                     avec modification de la variable ou sans modification
//     Usage :
//       unsigned char valeur;
//       unsigned char retour;
//
//       valeur = 0b00101110;
//       BitClr(valeur,2);  // Equivalent a valeur=0b00101010
//       retour = BitClrT(valeur,5); // laisse valeur inchangee
//                                   // mais retourne retour=0b00001010

// Negation sur la variable
#define NegT(Var) (~(Var))
#define Neg(Var) ((Var)= ~(Var))

// Mise a 1 d'un bit de la variable
#define BitSetT(Var,BitNbr) ((Var) | 1 << (BitNbr))
#define BitSet(Var,BitNbr) ((Var) |= 1 << (BitNbr))

// Mise a 0 d'un bit de la variable
#define BitClrT(Var,BitNbr) ((Var) & ~(1 << (BitNbr)))
#define BitClr(Var,BitNbr) ((Var) &= ~(1 << (BitNbr)))

// Mise a 0 ou a 1 d'un bit de la variable
#define BitValT(Var,BitNbr,Value) ((Var) & ~(1<<(BitNbr))) | ((Value)<<(BitNbr))
#define BitVal(Var,BitNbr,Value) ((Var) = ((Var) & ~(1<<(BitNbr))) | ((Value)<<(BitNbr)))

// Inversion d'un bit de la variable
#define BitToggleT(Var,BitNbr) ((Var) ^ 1 << (BitNbr))
#define BitToggle(Var,BitNbr) ((Var) ^= 1 << (BitNbr))


#endif