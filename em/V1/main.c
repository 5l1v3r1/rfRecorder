#include "global.h"
#include "attente.h"
#include "lcd.h"
#include "eeprom.h"

extern volatile u16 eeprom32[]={0x0000};
extern volatile u16 pos=0;

void init(void)
{
  u16 i=0;
  u16 j=0;
  // Initialisations des Ports
  DDRA=VAL_DDRA;
  DDRB=VAL_DDRB;
  DDRC=VAL_DDRC;
  DDRD=VAL_DDRD;
  // Initialisation de l'eeprom
  for(i=0;i<256;i++)
    { 
	  eeprom32[i]=read_eeprom(j);
	  eeprom32[i]=eeprom32[i]<<8;
	  j++;
	  eeprom32[i]=eeprom32[i] | read_eeprom(j);
	  j++;
	}
  // Initialisation des LEDs
  LED1OFF();
  LED2OFF();
  LED3OFF();
  // Initialisations de Timer1
  TCNT1=0x0000;     // Initialisation timer1
  sbi(TIMSK,2);    // Interruption Timer1 owerflow
  sbi(TCCR1B,0);   // CLK/1 declenchement timer1
  cbi(SIGNALPORT,SIGNALBIT);
}

SIGNAL(SIG_OVERFLOW1)
{
  TCNT1=0xFFFF-eeprom32[pos];
  SIGNALTOGGLE();
  pos++;
}


int main(void)
{
  init();
  LED1ON();
  lcd_init(LCD_DISP_ON);
  lcd_clrscr();
  lcd_puts("*** Bonjour ***");
  delay_ms(100);
  sbi(TX_enable_PORT,TX_enable_BIT);
  lcd_clrscr();
  lcd_printf("Emission :\nen cours... ");
  sbi(SREG,7);
  while(pos<510);
  cbi(TIMSK,2);    // Interdiction des interruptions Timer1 owerflow
  cbi(TX_enable_PORT,TX_enable_BIT);
  lcd_printf("OK");
  LED1OFF();
  while(1);
  return 0;
}
