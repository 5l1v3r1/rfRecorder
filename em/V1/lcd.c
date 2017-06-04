/****************************************************************************
 Titre	:   HD44780U Librairie LCD
 Auteur:    LeBomb
 Date:      02/09/2004
 Software:  WinAVR
 Cible:     Microcontroleur Atmel AT90S4414/8515/Mega

 DESCRIPTION
       Routine de base pour un ecran LCD type HD44780U
       L'ecran doit etre utilise en mode 4 bit de donnees (LCD_IO_MODE=1).
	   Le mode 8 bits de donnes n'est pas supporte (LCD_IO_MODE=0).
	   Possibilite d'avoir les bits de controle sur un port et les donnees sur un autre
 UTILISATION
       Liste de fonctions utiles dans lcd.h
	   Configurer ports et bits dans lcd.h
	   Initialiser de la facon suivante:
		   lcd_init(LCD_DISP_ON);
		   lcd_clrscr();
	   Fonction principale utilisable:
		   example: lcd_printf("Bonjour %d",x); Utilisation identique a un printf() classique.
       
*****************************************************************************/

#include "global.h"
#include "lcd.h"
#include "attente.h"
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <stdio.h>
#include <stdarg.h>

#if LCD_IO_MODE
#define lcd_e_delay()   asm volatile("nop")  /* delay 500ns with 4Mhz */
#define lcd_e_high()    sbi(LCD_E_PORT, LCD_E_PIN)
#define lcd_e_low()     cbi(LCD_E_PORT, LCD_E_PIN)
#define lcd_e_toggle()  toggle_e()
#endif

#if LCD_IO_MODE
#if LCD_LINES==1
#define LCD_FUNCTION_DEFAULT    LCD_FUNCTION_4BIT_1LINE 
#else
#define LCD_FUNCTION_DEFAULT    LCD_FUNCTION_4BIT_2LINES 
#endif
#else
#if LCD_LINES==1
#define LCD_FUNCTION_DEFAULT    LCD_FUNCTION_8BIT_1LINE
#else
#define LCD_FUNCTION_DEFAULT    LCD_FUNCTION_8BIT_2LINES
#endif
#endif


/* 
** function prototypes 
*/

#if LCD_IO_MODE
static void toggle_e(void);
#endif

/*
** local functions
*/

#if LCD_IO_MODE

// DDR en sortie
static void inline port_set_out(void)
{
 *(&LCD_DATA_PORT -1) = MASQUE_DATA_PORT | *(&LCD_DATA_PORT -1);
 *(&LCD_CONF_PORT -1) = MASQUE_CONF_PORT | *(&LCD_CONF_PORT -1);
}
// DDR en entree
static void inline port_set_in(void)
{
  *(&LCD_DATA_PORT -1) = 0xF0 & (*(&LCD_DATA_PORT -1));
}

static void inline toggle_e(void)
/* toggle Enable Pin */
{
  lcd_e_high();
  lcd_e_delay();
  lcd_e_low();
}

static void inline lcd_write(u08 data,u08 rs) 
{
  /* configure data pins as output */
  port_set_out();
  
  if (rs) 
    {   /* write data        (RS=1, RW=0) */
    
      /* output high nibble first */
      LCD_DATA_PORT = ((data>>4) & 0x0F)|(LCD_DATA_PORT & ~MASQUE_DATA_PORT);
      LCD_CONF_PORT = (1<<LCD_RS_PIN)|(LCD_CONF_PORT & ~MASQUE_CONF_PORT);;
	  lcd_e_toggle();
      
      /* output low nibble */
      LCD_DATA_PORT = (data & 0x0F)|(LCD_DATA_PORT & ~MASQUE_DATA_PORT);
      LCD_CONF_PORT = (1<<LCD_RS_PIN)|(LCD_CONF_PORT & ~MASQUE_CONF_PORT);;
	  lcd_e_toggle();
    }
  else 
    {    /* write instruction (RS=0, RW=0) */
      
      /* output high nibble first */
      
      LCD_DATA_PORT = ((data>>4)&0x0F)| (LCD_DATA_PORT & ~MASQUE_DATA_PORT);
      LCD_CONF_PORT = (LCD_CONF_PORT & ~MASQUE_CONF_PORT);
	  lcd_e_toggle();
      
      /* output low nibble */
      LCD_DATA_PORT = (data &0x0F)| (LCD_DATA_PORT & ~MASQUE_DATA_PORT);
      LCD_CONF_PORT = (LCD_CONF_PORT & ~MASQUE_CONF_PORT);
      lcd_e_toggle();
    }  
  
  /* all data pins high (inactive) */
  LCD_DATA_PORT = 0x0F | (LCD_DATA_PORT & ~MASQUE_DATA_PORT);
}

static u08 lcd_read(u08 rs) 
{
  register u08 dataH, dataL;
  
  
  if (rs) sbi(LCD_RS_PORT, LCD_RS_PIN);    /* RS=1: read data      */
  else    cbi(LCD_RS_PORT, LCD_RS_PIN);    /* RS=0: read busy flag */
  sbi(LCD_RW_PORT, LCD_RW_PIN);            /* RW=1  read mode      */
  
  /* configure data pins as input */
  port_set_in();
  
  lcd_e_high();
  lcd_e_delay();
  
  
  dataH = *(&LCD_DATA_PORT -2);/* read high nibble first */
  
  lcd_e_low();    
  lcd_e_delay();                           /* Enable 500ns low       */
  
  lcd_e_high();
  lcd_e_delay();
  
  dataL = *(&LCD_DATA_PORT -2);            /* read low nibble        */
  
  lcd_e_low();
    
  return ( (dataH<<4) | (dataL&0x0F) );
}

#else // memory mapped mode

#define lcd_write(d,rs) if (rs) *(volatile u08*)(LCD_IO_DATA) = d; else *(volatile u08*)(LCD_IO_FUNCTION) = d;
/* rs==0 -> write instruction to LCD_IO_FUNCTION */
/* rs==1 -> write data to LCD_IO_DATA */

#define lcd_read(rs) (rs) ? *(volatile u08*)(LCD_IO_DATA+LCD_IO_READ) : *(volatile u08*)(LCD_IO_FUNCTION+LCD_IO_READ)
/* rs==0 -> read instruction from LCD_IO_FUNCTION */
/* rs==1 -> read data from LCD_IO_DATA */

#endif// memory mapped mode


static unsigned char inline lcd_waitbusy(void)
/* loops while lcd is busy, reads address counter */
{
  register unsigned char c;
  
  while ( (c=lcd_read(0)) & (1<<LCD_BUSY)) {}
  
  return (c);  // return address counter
}


static void inline lcd_newline(u08 pos)
     /* goto start of next line */
{
  register u08 addressCounter;
  
  
#if LCD_LINES==1
  addressCounter = 0;
#endif
#if LCD_LINES==2
  if ( pos < (LCD_START_LINE2) )
    addressCounter = LCD_START_LINE2;
  else
    addressCounter = LCD_START_LINE1;
#endif
#if LCD_LINES==4
  if ( pos < LCD_START_LINE3 )
    addressCounter = LCD_START_LINE2;
  else if ( (pos >= LCD_START_LINE2) && (pos < LCD_START_LINE4) )
    addressCounter = LCD_START_LINE3;
  else if ( (pos >= LCD_START_LINE3) && (pos < LCD_START_LINE2) )
    addressCounter = LCD_START_LINE4;
  else 
    addressCounter = LCD_START_LINE1;
#endif
  lcd_command((1<<LCD_DDRAM)+addressCounter);
  
}/* lcd_newline */


/*
** PUBLIC FUNCTIONS 
*/

void lcd_command(u08 cmd)
     /* send commando <cmd> to LCD */
{
  lcd_waitbusy();
  lcd_write(cmd,0);
}


void lcd_gotoxy(u08 x, u08 y)
     /* goto position (x,y) */
{
#if LCD_LINES==1
  lcd_command((1<<LCD_DDRAM)+LCD_START_LINE1+x);
#endif
#if LCD_LINES==2
  if ( y==0 ) 
    lcd_command((1<<LCD_DDRAM)+LCD_START_LINE1+x);
  else
    lcd_command((1<<LCD_DDRAM)+LCD_START_LINE2+x);
#endif
#if LCD_LINES==4
  if ( y==0 )
    lcd_command((1<<LCD_DDRAM)+LCD_START_LINE1+x);
  else if ( y==1)
    lcd_command((1<<LCD_DDRAM)+LCD_START_LINE2+x);
  else if ( y==2)
    lcd_command((1<<LCD_DDRAM)+LCD_START_LINE3+x);
  else /* y==3 */
    lcd_command((1<<LCD_DDRAM)+LCD_START_LINE4+x);
#endif
  
}/* lcd_gotoxy */


void inline lcd_clrscr(void)
     /* clear lcd and set cursor to home position */
{
  lcd_command(1<<LCD_CLR);
}


void inline lcd_home(void)
     /* set cursor to home position */
{
  lcd_command(1<<LCD_HOME);
}


void inline lcd_putc(char c)
     /* print character at current cursor position */
{
  register unsigned char pos;
  
  pos = lcd_waitbusy();   // read busy-flag and address counter
  if (c=='\n')
    lcd_newline(pos);
  else
    lcd_write(c, 1);
}


void inline lcd_puts(const char *s)
     /* print string on lcd (no auto linefeed) */
{
  register char c;
  
  while ( (c = *s++) ) 
    {
      lcd_putc(c);
    }
  
}


void inline lcd_puts_p(const char *progmem_s)
     /* print string from program memory on lcd (no auto linefeed) */
{
  register char c;
  
  while ( (c = PRG_RDB(progmem_s++)) ) 
    {
      lcd_putc(c);
    }
  
}


void lcd_init(u08 dispAttr)
     /* initialize display and select type of cursor */
     /* dispAttr: LCD_DISP_OFF, LCD_DISP_ON, LCD_DISP_ON_CURSOR, LCD_DISP_CURSOR_BLINK */
{
#if LCD_IO_MODE   
  /*------ Initialize lcd to 4 bit i/o mode -------*/
  
  port_set_out();
  
  delay_ms(16);        /* wait 16ms or more after power-on       */
  
  /* initial write to lcd is 8bit */
  outp(LCD_FUNCTION_8BIT_1LINE>>4,LCD_DATA_PORT);
  lcd_e_toggle();
  delay_ms(5);         /* delay, busy flag can't be checked here */
    
  outp(LCD_FUNCTION_8BIT_1LINE>>4,LCD_DATA_PORT);
  lcd_e_toggle();
  delay_ms(1);           /* delay, busy flag can't be checked here */
  
  outp(LCD_FUNCTION_8BIT_1LINE>>4,LCD_DATA_PORT);
  lcd_e_toggle();
  delay_ms(1);           /* delay, busy flag can't be checked here */
  
  outp(LCD_FUNCTION_4BIT_1LINE>>4,LCD_DATA_PORT); /* set IO mode to 4bit */
  lcd_e_toggle();
  
  /* from now the lcd only accepts 4 bit I/O, we can use lcd_command() */    
#else  
  /*----- Initialize lcd to 8 bit memory mapped mode ------*/
  
  /* enable external SRAM (memory mapped lcd) and one wait state */
  /* add  -Wl,--defsym,__init_mcucr__=0xC0 to the link command   */
  /* or uncomment the next line:                                 */
  outp((1<<SRE)|(1<<SRW), MCUCR);
  
  /* reset lcd */
  delay_ms(16);                           /* wait 16ms after power-on     */
  lcd_write(LCD_FUNCTION_8BIT_1LINE,0);   /* function set: 8bit interface */                   
  delay_ms(5);                            /* wait 5ms                     */
  lcd_write(LCD_FUNCTION_8BIT_1LINE,0);   /* function set: 8bit interface */                 
  delay_ms(1);                              /* wait 64us                    */
  lcd_write(LCD_FUNCTION_8BIT_1LINE,0);   /* function set: 8bit interface */                
  delay_ms(1);                              /* wait 64us                    */
#endif
  lcd_command(LCD_FUNCTION_DEFAULT);      /* function set: display lines  */
  lcd_command(LCD_DISP_OFF);              /* display off                  */
  lcd_clrscr();                           /* display clear                */ 
  lcd_command(LCD_MODE_DEFAULT);          /* set entry mode               */
  lcd_command(dispAttr);                  /* display/cursor control       */

}/* lcd_init */

/* LeBomb */
void lcd_printf(const unsigned char *format, ...)
{
  static const char hex[] = "0123456789ABCDEF";
  unsigned int div_val, temp_arg;
  u08 i=8;
  va_list ap;

  va_start (ap, format);
  while(*format)
  {
	if(*format != '%')
	  { lcd_putc (*format); }
	else
	  {
		temp_arg = va_arg(ap,int);
		format++;
        switch (*format)
	      {
			case 'b':
			  i=8;
			  do { i--; if(bit_is_set(temp_arg,i)) {lcd_putc('1');} else {lcd_putc('0');} } while(i);
			  break;
			case 'c':
			  lcd_putc(temp_arg);
			  break;
			case 'd':
			  div_val = 10000;
			  if (temp_arg < 0) {temp_arg = - temp_arg; lcd_putc('-');}
			  while (div_val > 1 && div_val > temp_arg) { div_val /= DEC; }
			  do { lcd_putc(hex[temp_arg / div_val]); temp_arg %= div_val; div_val /= DEC; }
			  while (div_val);
			  break;
			case 'x':
			  div_val = 0x1000;
			  while (div_val > 0x10 && div_val > temp_arg) { div_val /= HEX; }
			  do { lcd_putc(hex[temp_arg / div_val]); temp_arg %= div_val; div_val /= HEX; }
			  while (div_val);
			  break;
			case 'm':
			  div_val = 0x1000;
			  do { lcd_putc(hex[temp_arg / div_val]); temp_arg %= div_val; div_val /= HEX; }
			  while (div_val);
			  break;
			default:
			  lcd_putc(*format);
			  break;
		  }
	  }
	format++;
  }
  va_end (ap);
}
