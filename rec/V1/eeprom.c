/* Ecriture et lecture de l'eeprom */

# include "global.h"
# include "eeprom.h"


u08 read_eeprom(u16 address_eeprom)
{
  EEAR = address_eeprom;
  sbi(EECR,EERE);
  return EEDR;
}


void write_eeprom(u16 address_eeprom, u08 data_eeprom)
{
  while(bit_is_set(EECR,EEWE)) {}
  EEAR = address_eeprom;
  EEDR = data_eeprom;
  sbi(EECR,EEMWE);
  sbi(EECR,EEWE);
  while(bit_is_set(EECR,EEWE)) {}
}
