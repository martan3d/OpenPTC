
/*
 * EEdata.c
 * Fixed at 5 locomotives storage. 
 * Each button/switch contains a programmed function code
 * 
 */

#include <avr/eeprom.h>
#include <stdio.h>

#define LOCOS 5
#define INPUTS 7

uint8_t  EEMEM Gbuttons[LOCOS][INPUTS];     /* Loco Function Codes */
uint16_t EEMEM locoAdr[LOCOS];            /* Loco addresses */

uint16_t locoAddresses[LOCOS];
uint8_t  group[LOCOS][INPUTS];


/* pull all loco addresses from eeprom into sram */

void readAllLocoAddressesFromEEPROM()
{
   uint8_t j;
   
   for(j=0;j<5;j++)               // get all five loco addresses
   {
     locoAddresses[j] = eeprom_read_word( (const uint16_t *) &locoAdr[j]);
   }
}

/* write locoaddresses from sram into EEPROM */

void writeAllLocoAddressesToEEPROM()
{
   uint8_t j;
   
   for(j=0;j<5;j++)               // get all five loco addresses
   {
     eeprom_write_word( (uint16_t*) &locoAdr[j], locoAddresses[j]);
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
   
   for(j=0;j<5;j++)                // get all five loco groups
   {
      for(i=0;i<7;i++)             // read all 7 bytes into sram
      {
        group[j][i] = eeprom_read_byte( (const uint8_t *) &Gbuttons[j][i]);
      }
   }
}

/* write all programmed loco Fcodes that are in sram into EEPROM by loco index */

void writeGroupDataToEEPROM(uint8_t index, uint8_t * data)
{
   uint8_t i;

   for(i=0;i<7;i++)
   {
      group[index][i] = data[i];
      eeprom_write_byte( (uint8_t*) &Gbuttons[index][i], group[index][i] );
   }
}
