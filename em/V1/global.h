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
#include <avr/iom32.h>  // definition du processeur
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

// Sens des ports pour l'initialisation
#define VAL_DDRA 0x00 // Analogiques
#define VAL_DDRB 0x0E // COM
#define VAL_DDRC 0xFF // LCD & TX_enable
#define VAL_DDRD 0xFA // ADR & PD & TX/RX

// LED1
#define LED1PORT PORTB
#define LED1PIN PINB
#define LED1BIT 1

#define LED1ON() {sbi(LED1PORT,LED1BIT);}
#define LED1OFF() {cbi(LED1PORT,LED1BIT);}
#define LED1TOGGLE() {if(bit_is_set(LED1PIN,LED1BIT)) cbi(LED1PORT,LED1BIT); else sbi(LED1PORT,LED1BIT);}

// LED2
#define LED2PORT PORTB
#define LED2PIN PINB
#define LED2BIT 2

#define LED2ON() {sbi(LED2PORT,LED2BIT);}
#define LED2OFF() {cbi(LED2PORT,LED2BIT);}
#define LED2TOGGLE() {if(bit_is_set(LED2PIN,LED2BIT)) cbi(LED2PORT,LED2BIT); else sbi(LED2PORT,LED2BIT);}

// LED3
#define LED3PORT PORTB
#define LED3PIN PINB
#define LED3BIT 3

#define LED3ON() {sbi(LED3PORT,LED3BIT);}
#define LED3OFF() {cbi(LED3PORT,LED3BIT);}
#define LED3TOGGLE() {if(bit_is_set(LED3PIN,LED3BIT)) cbi(LED3PORT,LED3BIT); else sbi(LED3PORT,LED3BIT);}

// SIGNAL
#define SIGNALPORT PORTD
#define SIGNALPIN PIND
#define SIGNALBIT 1

#define SIGNALON() {sbi(SIGNALPORT,SIGNALBIT);}
#define SIGNALOFF() {cbi(SIGNALPORT,SIGNALBIT);}
#define SIGNALTOGGLE() {if(bit_is_set(SIGNALPIN,SIGNALBIT)) cbi(SIGNALPORT,SIGNALBIT); else sbi(SIGNALPORT,SIGNALBIT);}

// Adresse de selection des voies Numeriques via le Xilinx
#define ADRPORT PORTD
#define ADRPIN PIND
#define ADRBIT0 3
#define ADRBIT1 4
#define ADRBIT2 5
#define ADRBIT3 6
#define ADRBIT4 7

// PD : bit selectionne par ADR
#define PDPORT PORTD
#define PDPIN PIND
#define PDBIT 2

// LCD
#define PORTLCD PORTC
#define LCDBIT0 0
#define LCDBIT1 1
#define LCDBIT2 2
#define LCDBIT3 3
#define LCDBIT_RS 4
#define LCDBIT_RW 5
#define LCDBIT_E 6

// TX Enable
#define TX_enable_PORT PORTC
#define TX_enable_BIT 7

// CLAVIER
#define nbre_de_touches 12
#define zero 0
#define un 1
#define deux 2
#define trois 3
#define quatre 4
#define cinq 5
#define six 6
#define sept 7
#define huit nbre_de_touches
#define neuf 9
#define level_max_menu 5
#define index_max_menu 5
extern volatile u08 level_menu;
extern volatile u08 index_menu[level_max_menu+1];

#define plus 8
#define moins 0
#define enter 11
#define back 10

// Variables du circuit
#define adr_recepteur 144
#define commande_num 100
#define commande_adc 200
#define nb_ADC 8
#define nb_NUM 3
#define nb_CMD 1
#define taille_eeprom 1024
extern volatile u16 eeprom32[taille_eeprom/2];

#endif
