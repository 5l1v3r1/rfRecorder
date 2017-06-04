/****************************************************************************
 Titre	:   Parametres globaux
 Auteur:    LeBomb
 Date:      02/09/2004
 Software:  WinAVR
 Cible:     Microcontroleur Atmel AT90S4414/8515/Mega

 DESCRIPTION
       Definit et declare tous les parametres globaux relatifs a l'application
 UTILISATION
       Definir la valeur du QUARTZ
	   Choix du microcontroleur utilise:
		   librairy <avr/iomXX.h>
		   dans le makefile MCU = atmegaXX
*****************************************************************************/

#ifndef _GLOBAL_
#define _GLOBAL_

typedef unsigned char  u08;
typedef unsigned short u16;
typedef unsigned long u32;
typedef unsigned long long u64;
typedef signed char  s08;
typedef signed short s16;
typedef signed long s32;
typedef signed long long s64;

#define QUARTZ 4000 // frequence du quartz en KHz, sert pour les fonctions d'attente

#define GCC33

// includes generaux
#include <avr/io.h>
#include <avr/iom163.h>  // definition du processeur
#include <avr/interrupt.h>
#include <avr/signal.h>
#include <stdlib.h>

#define extr16_08_0(i) (*(char *)(&i))
#define extr16_08_1(i) (*((char *)(&i)+1))

#define extr32_16_0(i) (*(int *)(&i)) // poids faible
#define extr32_16_1(i) (* (((int *)(&i)) +1)) //poids fort
#define extr32_16_23(i) (*((int *)((char *)(&i)+1))) // les 2 octets du milieu

#define extr32_08_0(i) (*(char *)(&i))
#define extr32_08_1(i) (*((char *)(&i)+1))
#define extr32_08_2(i) (*((char *)(&i)+2))
#define extr32_08_3(i) (*((char *)(&i)+3))

#define nop() __asm__ __volatile__ ("NOP");
#define rien() __asm__ __volatile__ (" ");

/* Utilisation des ports
PORTA : 
0-7 -> Sorties Analogiques

PORTB : 
0   -> LED1
1   -> SIGNAL
2-7 -> Clavier

PORTC :
0-7 -> Sorties Numeriques

PORTD :
0   -> SIGNAL = RXD
1-6 -> LCD
7   -> POUT
*/

#define VAL_DDRA 0xFF // Analogiques
#define VAL_DDRB 0x01 // LED1 & Clavier
#define VAL_DDRC 0xFF // Numerique
#define VAL_DDRD 0xFE // LCD & POUT

// LED1
#define LED1PORT PORTB
#define LED1PIN PINB
#define LED1BIT 0

#define LED1ON() {sbi(LED1PORT,LED1BIT);}
#define LED1OFF() {cbi(LED1PORT,LED1BIT);}
#define LED1TOGGLE() {if(bit_is_set(LED1PIN,LED1BIT)) cbi(LED1PORT,LED1BIT); else sbi(LED1PORT,LED1BIT);}

// SIGNAL
#define SIGNALPORT PORTB
#define SIGNALPIN PINB
#define SIGNALBIT 1
#define RXPORT PORTD
#define RXPIN PIND
#define RXBIT 0

// POUT
#define POUTPORT PORTD
#define POUTBIT 7

// SORTIES
#define NUMPORT PORTC
#define NUMPIN PINC
#define ADCPORT PORTA
#define ADCPIN PINA

// Variables du circuit
#define adr_recepteur 144
#define commande_num 100
#define commande_adc 200
#define nb_ADC 8
#define nb_NUM 8

extern volatile u08 eeprom[512];

#endif
