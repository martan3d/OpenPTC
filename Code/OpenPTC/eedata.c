
/*
 * EEdata.c
 * Fixed at 5 locomotives storage. 
 * Each button/switch contains a programmed function code
 * 
 */

#include <avr/eeprom.h>
#include <stdio.h>

uint8_t EEMEM G0button0;     /* Loco 0 Function Codes per input */
uint8_t EEMEM G0button1;
uint8_t EEMEM G0button2;
uint8_t EEMEM G0switch0;
uint8_t EEMEM G0switch1;
uint8_t EEMEM G0switch2;
uint8_t EEMEM G0switch3;

uint8_t EEMEM G1button0;     /* Loco 1 Function Codes per input */
uint8_t EEMEM G1button1;
uint8_t EEMEM G1button2;
uint8_t EEMEM G1switch0;
uint8_t EEMEM G1switch1;
uint8_t EEMEM G1switch2;
uint8_t EEMEM G1switch3;

uint8_t EEMEM G2button0;     /* Loco 2 Function Codes per input */
uint8_t EEMEM G2button1;
uint8_t EEMEM G2button2;
uint8_t EEMEM G2switch0;
uint8_t EEMEM G2switch1;
uint8_t EEMEM G2switch2;
uint8_t EEMEM G2switch3;

uint8_t EEMEM G3button0;     /* Loco 3 Function Codes per input */
uint8_t EEMEM G3button1;
uint8_t EEMEM G3button2;
uint8_t EEMEM G3switch0;
uint8_t EEMEM G3switch1;
uint8_t EEMEM G3switch2;
uint8_t EEMEM G3switch3;

uint8_t EEMEM G4button0;     /* Loco 4 Function Codes per input */
uint8_t EEMEM G4button1;
uint8_t EEMEM G4button2;
uint8_t EEMEM G4switch0;
uint8_t EEMEM G4switch1;
uint8_t EEMEM G4switch2;
uint8_t EEMEM G4switch3;

uint8_t group0[7];
uint8_t group1[7];
uint8_t group2[7];
uint8_t group3[7];
uint8_t group4[7];

uint8_t *gptr = &G0button0;
uint8_t *pptr = group0;

/* return all input Fcodes */
uint8_t * getGroupData(uint8_t index)
{
   uint8_t eedata, i;
   
   gptr = &G0button0 + (index*7);    // point to start of EEDATA we want
   pptr = &group0[0] + (index*7);    // fill this sram array with it
    
   for(i=0;i<7;i++)
   {
     eedata = eeprom_read_byte( (const uint8_t*) *(gptr+i));
     *(pptr+i) = eedata;
   }
  return &group0[0] + (index*7);     // return pointer to sram array we just filled
}

/* write all input Fcodes */
void putGroupData(uint8_t index, uint8_t * data)
{
   uint8_t eedata, i;
 
   pptr = &group0[0] + (index*7);    // fill this sram array with passed in data
   memcpy(pptr, data, 7);            // blast it in there

   for(i=0;i<7;i++)
   {
     gptr = &group0[0] + (index*7);
     pptr = &G0button0 + (index*7);    // point to start of EEDATA we want to write to
     eeprom_write_byte( (uint8_t*) pptr[i], gptr[i] );
   }

}
