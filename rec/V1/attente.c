// fonctions d'attente pour delais peu precis

#include "global.h"
#include "attente.h"



void delay_3cyc(u08 dec) 
     // 3 cycles, 8 bits
{
  while ( dec ) dec--;
} 


void delay_4cyc(u16 dec) 
     // 4 cycles, 16 bits
{
    while ( dec ) dec--;
}

void delay_us(u16 us)
{
  us=((us*(QUARTZ/4000))-3);
  while(us)
    us--;
}  


void delay_ms(u16 ms) 
{
  u16 dec;
    while ( ms ) 
      {
	dec = ( (QUARTZ / 4) - 1);
	while ( dec-- ) // attention aux simplifications a la compilation
    rien();
	ms--;// gcc optimise souvent, ce qui n est pas pratique ici
      }
} 

void testInterruptions()// permet de voir combien de temps necessitent les interruptions, en mesurant la periode du signal sortant sur le port D, qui est de 20ms sans int.
{
  outp(0xFF, DDRD);// mesure du temps mis
  while(1)
    {
      outp(0xFF, PORTD);
      
      delay_ms(10);

      outp(0x00, PORTD);
      delay_ms(10);
    }
}
