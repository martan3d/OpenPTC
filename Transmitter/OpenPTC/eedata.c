
/*
   EEdata.c
   Fixed at 5 locomotives storage.
   Each button/switch contains a programmed function code

*/

#include <avr/eeprom.h>
#include <stdio.h>
#include "eedata.h"

/* offest in EEPROM from 0000 */

#define EE_LOCO_ADDRESS0 0x00
#define EE_LOCO_ADDRESS1 0x02
#define EE_LOCO_ADDRESS2 0x04
#define EE_LOCO_ADDRESS3 0x06
#define EE_LOCO_ADDRESS4 0x08

#define EE_GROUP0        0x10
#define EE_GROUP1        0x18
#define EE_GROUP2        0x20
#define EE_GROUP3        0x28
#define EE_GROUP4        0x30

#define LOCOS 5
#define INPUTS 7

uint16_t locoAddresses[LOCOS];
uint8_t  group[LOCOS][INPUTS];

/* pull all loco addresses from eeprom into sram */

void readAllLocoAddressesFromEEPROM()
{
  uint8_t j;
  
  for (j = 0; j < LOCOS; j++)        // get all five loco addresses
  {
    locoAddresses[j] = eeprom_read_word((const uint16_t*) EE_LOCO_ADDRESS0+j);
  }
}

/* write locoaddresses from sram into EEPROM */

void writeAllLocoAddressesToEEPROM()
{
  uint8_t j;

  for (j = 0; j < LOCOS; j++)        // get all five loco addresses
  {
    eeprom_write_word( (uint16_t*) EE_LOCO_ADDRESS0+j, locoAddresses[j]);
  }
}

/* set a loco address in sram */

void setLocoAddress(uint8_t index, uint16_t address)
{
  locoAddresses[index] = address;
}

/* fetch a locoaddress from sram */

uint16_t getLocoAddress(uint8_t index)
{
  return locoAddresses[index];
}

/* return pointer to loco group specified by index */

uint8_t * getGroupData(uint8_t index)
{
  return &group[index][0];        // return pointer to sram array
}

/* set groupdata byte in sram array to passed in value */

void setGroupData(uint8_t index, uint8_t subindex, uint8_t value)
{
  group[index][subindex] = value;
}

/* read all group data from eeprom into sram */

void readAllGroupDataFromEEPROM()
{
  uint8_t i, j;

  for (j = 0; j < LOCOS; j++)         // get all five loco groups
  {
    for (i=0;i<INPUTS;i++)
    {
       group[j][i] = eeprom_read_byte( (const uint8_t *) EE_GROUP0 + (j*8) + i);
    }
  }
}

/* write all programmed loco Fcodes that are in sram into EEPROM by loco index */

void writeAllGroupDataToEEPROM(uint8_t index, uint8_t * data)
{
  uint8_t i;

  for (i = 0; i < 7; i++)
  {
    eeprom_write_byte( (uint8_t*) EE_GROUP0+(index*8)+i, data[i] );
  }
}
