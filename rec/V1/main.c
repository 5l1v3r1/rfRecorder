#include "global.h"
#include "attente.h"
#include "eeprom.h"


extern volatile u08 eeprom[512]={0x00};
extern volatile u08 debordement=0;

void init(void)
{
  // Initialisations des Ports
  DDRA=VAL_DDRA;
  DDRB=VAL_DDRB;
  DDRC=VAL_DDRC;
  DDRD=VAL_DDRD;
  sbi(RXPORT,RXBIT);     // Resistance de tirage sur RXD
  // Initialisations de Timer1
  // sbi(TIMSK,2);    // Interruption Timer1 owerflow
  TCNT1=0x0000;         // Initialisation timer1
  sbi(TCCR1B,0);   // CLK/1 declenchement timer1
  // Initialisation des LEDs
  LED1OFF();
  NUMPORT=0x00;
  ADCPORT=0x00;
}

/*
SIGNAL(SIG_OVERFLOW1)
{
  debordement++;
}
*/

int main(void)
{
  u16 pos=0;
  u16 temp_TCNT1=0x0000;
  init();
  //sbi(SREG,7);
  LED1ON();
  while(bit_is_clear(RXPIN,RXBIT));
  TCNT1=0x0000;
  while(pos<500)
    {
      while(bit_is_set(RXPIN,RXBIT));
	  temp_TCNT1=TCNT1;
	  TCNT1=0x0000;
	  //eeprom[pos++]=debordement;
	  //debordement=0;
	  eeprom[pos]=temp_TCNT1>>8;
	  pos++;
	  eeprom[pos]=temp_TCNT1;
	  pos++;
      while(bit_is_clear(RXPIN,RXBIT));
	  temp_TCNT1=TCNT1;
	  TCNT1=0x0000;
	  //eeprom[pos++]=debordement;
	  //debordement=0;
	  eeprom[pos]=temp_TCNT1>>8;
	  pos++;
	  eeprom[pos]=temp_TCNT1;
	  pos++;
    }
  for(pos=0;pos<512;pos++)
    { write_eeprom(pos,eeprom[pos]); }
  LED1OFF();
  while(1);
  return 0;
}
