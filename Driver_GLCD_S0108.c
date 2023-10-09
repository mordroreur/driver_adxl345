/****************************************************************************/
/*  Groupe C                                                      ISIMA     */
/*  Octobre 2020                                                            */
/*                                                                          */
/*                                                                          */
/*        Driver pour afficheur Graphique LCD (GLCD)                        */
/*        compatible controleur Samsung S0108, SB0108, KS0108               */
/*                                                                          */
/* Driver_GLCD_S0108.c              MPLAB X                    PIC 18F542   */
/****************************************************************************/
// Compatible avec le Graphical LCD Xiamen Ocular GDM12864B de la carte
// EasyPIC 6.0

#define _Driver_GLCD_S0108_C

#include <xc.h>
#include "TypesMacros.h"
#include "Driver_GLCD_S0108.h"
#include "Polices.h"


// Variables globales
extern ROM_INT8U *Font;    // Police en cours d'utilisation
extern INT8U FontWidth, FontHeight, FontSpace, FontOffset; 
extern INT8U CP_Line, CP_Row; // CP : current position


//================================================================   
//      Fonctions d'envoi de commandes ou de donnees
//================================================================


void GLCD_SendCmd(INT8U Instruction) {
    TRIS_GLCD_Data = 0x00;
    GLCD_RS = 0; GLCD_RW = 0;// Cmd mode
    GLCD_Data_OUT = Instruction;
    GLCD_Pulse_E();
}

void GLCD_SendData(INT8U Donnee) {
  GLCD_RS = 1;  GLCD_RW = 0; // Data mode
  GLCD_Data_OUT = Donnee;
  TRIS_GLCD_Data = 0;
  GLCD_Pulse_E();
}

// Generation d'un front sur la broche GLCD_E (PORTB(4)) servant a la validation
void GLCD_Pulse_E(void) { 
  GLCD_E = 1; 
  Delai_us(1); // Attente duree d'1 micro seconde (au moins 450 ns)
  GLCD_E = 0;  // front descendant effectif
}

// Initialisation du GLCD
void GLCD_Init(void) {
  //attente du power on
  Delay_ms(15);
  
  //orientation et initialisation
  TRISB &= 0b11000000;
  GLCD_Data_IN = 0x00;   // Data port : donnee ou instruction
  GLCD_Data_OUT = 0x00;
  TRIS_GLCD_Data = 0x00;
  
  //Reset simultanée des controleur G et D
  GLCD_CS1 = 1; GLCD_CS2 = 1;
  Delay_us(2);
  GLCD_RST = 1;
  Delay_us(2);
  
  //attente du reset
  INT8U Status = GLCD_ReadStatus();
  while (((Status >> 4) & 0x01) != 0x00){
    Status = GLCD_ReadStatus();
    Delay_us(2);
  }

  // Pour terminer, envoi de quelques commandes
  GLCD_SendCmd(GLCD_DISP_OFF);            // Normalement, deja effectue
  GLCD_SendCmd(GLCD_SET_START_LINE|0x00); // Idem
  GLCD_SendCmd(GLCD_SET_PAGE|0x00);       // Positionne les adresses a 0
  GLCD_SendCmd(GLCD_SET_COLUMN|0x00);
  GLCD_SendCmd(GLCD_DISP_ON);             // Allume et efface l'ecran
  GLCD_ClrScreen();  
  
  GLCD_SetFont(Police5x7, 5, 7, 0, 0x20);
}

//================================================================   
//      Fonctions de lecture de l'etat ou de donnees
//================================================================   

// Lecture de l'etat : 0b Busy 0 Disp Rst 0 0 0 0
//         si Busy = 0 : "Ready", sinon "In operation"
//         si Disp = 0 : "Disp On", sinon "Disp Off"
//         si Rst = 0 : "Normal", sinon "In Reset"
INT8U GLCD_ReadStatus(void) {
INT8U CurrentStatus;

  GLCD_RS = 0;  GLCD_RW = 1; // Status Read
  GLCD_Data_OUT = 0xFF;      // Efface le bus
  TRIS_GLCD_Data = 0xFF;
  GLCD_E = 1;
  Delai_us(5);
  CurrentStatus = GLCD_Data_IN; // Lecture du bus
  GLCD_E = 0;
  Delai_ms(15);
  return (CurrentStatus);
} 
 
// Lecture d'une donnee
INT8U GLCD_ReadData(void) {
INT8U CurrentData;

  GLCD_RS = 1;  GLCD_RW = 1;   // Data Read
  GLCD_Data_OUT = 0xFF;        // Efface le bus
  TRIS_GLCD_Data = 0xFF;
  GLCD_E = 1;
  CurrentData = GLCD_Data_IN;  // Lecture du bus
  GLCD_E = 0;
  return (CurrentData);
} 

//================================================================   
//      Fonctions basiques : position et remplissage   
//================================================================   

// Positionne en X et Y
void GLCD_SetPositionXY(INT8U X_Page, INT8U Y_Column) {   
  GLCD_SendCmd(GLCD_SET_X_ADDRESS|X_Page);
  GLCD_SendCmd(GLCD_SET_Y_ADDRESS|Y_Column);
}   
   
// Remplissage total de l'ecran avec motif
void GLCD_Fill(INT8U Sprite) {
INT8U x;   // Page x sur 3 bits
INT8U y;   // Colonne y sur 6 bits   
  
  GLCD_CS1 = 1;   // Remplissage des deux cotes a la fois
  GLCD_CS2 = 1;   
  for(x=0;x<8;x++){ 
    GLCD_SetPositionXY(x,0x00);  
    for(y=0;y<64;y++) GLCD_SendData(Sprite);   
  }   
}   

// Effacement ecran
// definit par #define GLCD_ClrScreen() GLCD_Fill(0x00)


//================================================================   
//      Fonctions de gestion d'un pixel : Set et Clr    
//================================================================   

// Dessine un pixel dans l'espace 64 x 128  (XX x YY)
void GLCD_SetPixel(INT8U XX, INT8U YY) {
INT8U x;      // Page x sur 3 bits
INT8U y;      // Colonne y sur 6 bits  
INT8U BitNbr; // Numero de bit sur l'octet considere
INT8U octet;  // Octet de donnee lu a l'adresse indiquee

  //         x = .... en fonction de XX      // Calcul la page (position x)
  //         BitNbr = .... en fonction de XX // Determine le bit dans l'octet considere

  x = XX >> 3 ;
  BitNbr = XX & 0x0F ;
  
  // Calcul de la position y (Colonne, gauche ou droite)
  if (YY < 64) {
    GLCD_CS1 = 0;  GLCD_CS2 = 1;  // Partie gauche
    y = YY;
  } else {
    GLCD_CS1 = 1;  GLCD_CS2 = 0;  // Partie droite
    y = YY - 64; 
  }
  
  //     1) Lecture de l'ancienne valeur de l'octet a cette position x, y
  //	   2) Mise a 1 du bit demande (BitNbr) sur l'octet lu
  //	   3) ecriture effective de cet octet sur l'ecran  

  GLCD_SetPositionXY(x,y);
  octet = GLCD_ReadData();
  octet |= (0x01 << BitNbr);
  GLCD_SendData(octet);
}

void GLCD_ClrPixel(INT8U XX, INT8U YY) {
  INT8U x;      // Page x sur 3 bits
  INT8U y;      // Colonne y sur 6 bits  
  INT8U BitNbr; // Numero de bit sur l'octet considere
  INT8U octet;  // Octet de donnee lu a l'adresse indiquee


  x = XX >> 3 ;
  BitNbr = XX & 0x0F ;
 
  // Calcul de la position y (Colonne, gauche ou droite)
  if (YY < 64) {
    GLCD_CS1 = 0;  GLCD_CS2 = 1;  // Partie gauche
    y = YY;
  } else {
    GLCD_CS1 = 1;  GLCD_CS2 = 0;  // Partie droite
    y = YY - 64; 
  }

  GLCD_SetPositionXY(x,y);
  octet = GLCD_ReadData();
  octet &= ~(0x01 << BitNbr);
  GLCD_SendData(octet);
}

//================================================================   
//      Fonctions de gestion des lignes droites   
//================================================================  

void GLCD_VerticalLine(INT8U XX1, INT8U YY, INT8U XX2, INT8U Color) {
INT8U XXdeb, XXfin; // Remise en ordre des XX1 et XX2
INT8U x;      // Page x sur 3 bits
INT8U y;      // Colonne y sur 6 bits  
INT8U BitNbr; // Numero de bit sur l'octet considere
INT8U octet;  // Octet de donnee lu a l'adresse indique
INT8U iXX; 

  //   |~~~~  A   C O M P L E T E R   ! ~~~~|
if (XX1 <= XX2){ 
    XXdeb = XX1;
    XXfin = XX2;
}
else{
    XXdeb = XX2;
    XXfin = XX1;
}

// Calcul de la position y (Colonne, gauche ou droite)
if (YY < 64) {
    GLCD_CS1 = 0;  GLCD_CS2 = 1;  // Partie gauche
    y = YY;
} else {
    GLCD_CS1 = 1;  GLCD_CS2 = 0;  // Partie droite
    y = YY - 64; 
}

x = XXdeb >> 3 ;
BitNbr = XXdeb & 0x0F ;
  
GLCD_SetPositionXY(x,y);
octet = GLCD_ReadData();
octet |= (0xFF << (BitNbr-1));
GLCD_SendData(octet);
x++;

while( x <  (XXfin >> 3)){
    GLCD_SetPositionXY(x,y);
    GLCD_SendData(0xFF);
    x++;
}

x = XXfin >> 3;
BitNbr = XXfin & 0x0F;

GLCD_SetPositionXY(x,y);
octet = GLCD_ReadData();
octet &= ~(0xFF << BitNbr);
GLCD_SendData(octet);
}

void GLCD_HorizontalLine(INT8U XX, INT8U YY1, INT8U YY2, INT8U Color) {
INT8U YYdeb, YYfin; // Remise en ordre des YY1 et YY2
INT8U iYY; 

  
  //                    |//
  //                   (o o)
  //   +---------oOO----(_)-----------------+
  //   |~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~|
  //   |~~~~  A   C O M P L E T E R   ! ~~~~|
  //   |~~~  E V E N T U E L L E M E N T ~~~|       
  //   |~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~|
  //   +--------------------oOO-------------+
  //                    |__|__|
  //                     || ||
  //                    ooO Ooo  

}


void GLCD_Line(INT8U XX1, INT8U YY1, INT8U XX2, INT8U YY2, INT8U Color) {
INT8U XXdeb, XXfin, YYdeb, YYfin; // Remise en ordre des points
float a, b; // equation de la droite Y = aX+b
INT8U XX, iYY; // index pour le trace de la droite

  if (XX1 == XX2) GLCD_HorizontalLine(XX1,YY1,YY2,Color);
  else if (YY1 == YY2) GLCD_VerticalLine(XX1,YY1,XX2,Color);
  else { // Trace en diagonale
      
  //                    |//
  //                   (o o)
  //   +---------oOO----(_)-----------------+
  //   |~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~|
  //   |~~~~  A   C O M P L E T E R   ! ~~~~|
  //   |~~~  E V E N T U E L L E M E N T ~~~|       
  //   |~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~|
  //   +--------------------oOO-------------+
  //                    |__|__|
  //                     || ||
  //                    ooO Ooo  

  }
}

void GLCD_Rectangle(INT8U XX1, INT8U YY1, INT8U XX2, INT8U YY2, INT8U Color) {
  // Rectangle vide
  GLCD_VerticalLine(XX1,YY1,XX2,Color);
  GLCD_VerticalLine(XX1,YY2,XX2,Color);
  GLCD_HorizontalLine(XX1,YY1,YY2,Color);
  GLCD_HorizontalLine(XX2,YY1,YY2,Color);
}

void GLCD_Box(INT8U XX1, INT8U YY1, INT8U XX2, INT8U YY2, INT8U Color) {
// Rectangle plein
INT8U YYdeb, YYfin; // Remise en ordre des YY1 et YY2
INT8U iYY; 

  YYdeb = ((YY1<YY2) ? YY1 : YY2 );
  YYfin = ((YY1<YY2) ? YY2 : YY1 );
    
  for (iYY=YYdeb; (iYY<=YYfin); iYY++) {
    GLCD_VerticalLine(XX1,iYY,XX2,Color);
  }
}


void GLCD_Circle(INT8U CentreX, INT8U CentreY, INT8U Rayon) {
// Algorithme de trace de cercle d'Andres/Bresenham conseille !
 
  //                    |//
  //                   (o o)
  //   +---------oOO----(_)-----------------+
  //   |~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~|
  //   |~~~~  A   C O M P L E T E R   ! ~~~~|
  //   |~~~  E V E N T U E L L E M E N T ~~~|       
  //   |~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~|
  //   +--------------------oOO-------------+
  //                    |__|__|
  //                     || ||
  //                    ooO Ooo  

}


//================================================================   
//      Fonction de gestion des caracteres et des textes
//================================================================   

// Choix de la police
void GLCD_SetFont(ROM_INT8U *Police, INT8U Largeur, INT8U Hauteur, INT8U Espace, INT8U Offset) {
  Font=Police;            // Police en cours d'utilisation
  FontHeight = Hauteur; 
  FontWidth = Largeur;
  FontSpace = Espace;
  FontOffset = Offset;
  CP_Line = 0;            // Position courante
  CP_Row = 0;
}

// Ecriture d'un texte
void GLCD_PrintAt(INT8U Ligne, INT8U Y, CHAR *Texte, INT8U Color) {
  CP_Line = Ligne;
  CP_Row = Y;
  while (*Texte) {
    GLCD_Chr(*Texte,Color); // Ecriture effective sur LCD
    Texte++;                // Caractere suivant
  }
}

// Ecriture d'un texte en position courante
void GLCD_Print(CHAR *Texte, INT8U Color) {
  while (*Texte) {
    GLCD_Chr(*Texte,Color); // Ecriture effective sur LCD
    Texte++;                // Caractere suivant
  }
}

//   #define GLCD_WriteAt(L,Y,T,C) GLCD_PrintAt(L,Y,T,C)
//   #define GLCD_Write(L,Y,T,C) GLCD_Print(L,Y,T,C)

// Ecriture d'un caractere 
void GLCD_ChrAt(INT8U Ligne, INT8U Y, CHAR Caractere, INT8U Color) {
  CP_Line = Ligne;
  CP_Row = Y;
  GLCD_Chr(Caractere,Color);
}

// Ecriture d'un caractere en position courante
void GLCD_Chr(CHAR Caractere, INT8U Color) {
  INT8U donne;
    if(CP_Row + FontWidth > 127){
        if(!Color){
            for(int i = CP_Row; i < 128; i++){
                if(CP_Row < 64){
                    GLCD_CS1 = 0;
                    GLCD_CS2 = 1;   
                }else{
                    GLCD_CS1 = 1;
                    GLCD_CS2 = 0;   
                }
                GLCD_SetPositionXY(CP_Line, CP_Row);
                GLCD_SendData(0xFF);
                CP_Row++;
            }
        }
        CP_Row = 0;
        CP_Line++;
    }
  
    for(int i = 0; i < FontWidth; i++){
        if(CP_Row < 64){
            GLCD_CS1 = 0;
            GLCD_CS2 = 1;   
        }else{
            GLCD_CS1 = 1;
            GLCD_CS2 = 0;   
        }
        GLCD_SetPositionXY(CP_Line, CP_Row);
        if(Color){
            GLCD_SendData(Font[((Caractere-FontOffset)*FontWidth)+i]);
        }else{
            GLCD_SendData(~Font[((Caractere-FontOffset)*FontWidth)+i]);
        }
        CP_Row++;
    }
    
    if(!Color){
        for(int i = 0; i < FontSpace; i++){
            if(CP_Row < 64){
                GLCD_CS1 = 0;
                GLCD_CS2 = 1;   
            }else{
                GLCD_CS1 = 1;
                GLCD_CS2 = 0;   
            }
            GLCD_SetPositionXY(CP_Line, CP_Row);
            GLCD_SendData(0xFF);
            CP_Row++;
        }
    }else{
        CP_Row += FontSpace;
    }
}
 


//=================================================
//      Création d'une Jauge selon un entier     //
//=================================================
void GLCD_Jauge_Acc(INT8U acc_X, INT8U acc_Y, INT8U acc_Z, INT8U temperature)
{
    CHAR txt [16]="Acceleration X:";
    INT8U page = 0;
    INT8U column = 0;
    INT8U color = GLCD_WHITE ;
    GLCD_PrintAt(page, column, txt, color);
    CHAR tmp [9];
    tmp [3] = '\0';
    tmp [2] = 'G';
    tmp[0] = acc_X/160 + '0';
    tmp[1] = (acc_X/16)%10 + '0';
    GLCD_Print(tmp, color);
    
    txt[13]='Y';
    page = 2;
    column = 0;
    GLCD_SetPositionXY(page, column);
    GLCD_PrintAt(page, column, txt, color);
    tmp[0] = acc_Y/160 + '0';
    tmp[1] = (acc_Y/16)%10 + '0';
    GLCD_Print(tmp, color);
    
    txt[13]='Z';
    page = 4;
    column = 0;
    GLCD_SetPositionXY(page, column);
    GLCD_PrintAt(page, column, txt, color);
    tmp[0] = acc_Z/160 + '0';
    tmp[1] = (acc_Z/16)%10 + '0';
    GLCD_Print(tmp, color);
    
    txt[0]='T';txt[1]='e';txt[2]='m';txt[3]='p';txt[4]='e';txt[5]='r';txt[6]='a';txt[7]='t';txt[8]='u';txt[9]='r';txt[10]='e';txt[11]=':';txt[12]='\0';
    page = 6;
    column = 0;
    GLCD_SetPositionXY(page, column);
    GLCD_PrintAt(page, column, txt, color);
    tmp[0] = temperature/10 + '0';
    tmp[1] = (temperature%10) + '0';
    tmp [2] = '.';
    tmp [3] = '0';
    tmp [4] = 'd';
    tmp [5] = 'e';
    tmp [6] = 'g';
    tmp [7] = 'C';
    tmp [8] = '\0';
    
      
    GLCD_Print(tmp, color);
        
    page = 1; GLCD_CS1 = 0; GLCD_CS2 = 1;
    for(column=0;column<acc_X/2; column++)
    {
        GLCD_SetPositionXY(page,column);  
        GLCD_SendData(0xFF);
    }
    if(acc_X/2>64)
    {
        GLCD_CS1 = 1; GLCD_CS2 = 0;
        for(column=64;column<acc_X/2; column++)
        {
            GLCD_SetPositionXY(page,column);  
            GLCD_SendData(0xFF);
        }
    }
    
    page = 3; GLCD_CS1 = 0; GLCD_CS2 = 1;
    for(column=0;column<acc_Y/2; column++)
    {
        GLCD_SetPositionXY(page,column);  
        GLCD_SendData(0xFF);
    }
    if(acc_Y/2>64)
    {
        GLCD_CS1 = 1; GLCD_CS2 = 0;
        for(column=64;column<acc_Y/2; column++)
        {
            GLCD_SetPositionXY(page,column);  
            GLCD_SendData(0xFF);
        }
    }
    
    page = 5; GLCD_CS1 = 0; GLCD_CS2 = 1;
    for(column=0;column<acc_Z/2; column++)
    {
        GLCD_SetPositionXY(page,column);  
        GLCD_SendData(0xFF);
    }
    if(acc_Z/2>64)
    {
        GLCD_CS1 = 1; GLCD_CS2 = 0;
        for(column=64;column<acc_Z/2; column++)
        {
            GLCD_SetPositionXY(page,column);  
            GLCD_SendData(0xFF);
        }
    }
    
    page = 7; GLCD_CS1 = 0; GLCD_CS2 = 1;
    for(column=0;column<temperature*1.6; column++)
    {
        GLCD_SetPositionXY(page,column);  
        GLCD_SendData(0xFF);
    }
    if(temperature*1.6>64)
    {
        GLCD_CS1 = 1; GLCD_CS2 = 0;
        for(column=64;column<temperature*1.6; column++)
        {
            GLCD_SetPositionXY(page,column);  
            GLCD_SendData(0xFF);
        }
    }
    
    GLCD_CS1 = 0; GLCD_CS2 = 0;
}