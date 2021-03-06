/*
 * functions.c
 * 
 * Translate button bits to bits in PT output packet
 * 
 */

#include <avr/io.h>
#include "buttons.h"
#include "eedata.h"

/*
 *  Byte 9   = Functions 28-24
 *  Byte 10  = Functions 23-16
 *  Byte 11  = Functions 15-8
 *  Byte 12  = Functions 7-0
 *  
 */

uint8_t functionCodes[4];

void defaultLocoAddresses()
{
    setLocoAddress(0, 2606);
    setLocoAddress(1, 2607);
    setLocoAddress(2, 2608);
    setLocoAddress(3, 2609);
    setLocoAddress(4, 2610);
    
    writeAllLocoAddressesToEEPROM();

}

/* set all function codes for all locos to defaults This is for ESU Custom setup*/

void defaultFunctionCodes()
{
    uint8_t defaultFn[7] = { 10,   // BUTTON0 - Brake 
                              3,   // BUTTON1 - Couplers
                              2,   // BUTTON2 - Horn
                              1,   // SWITCH0 - Bell
                              0,   // SWITCH1 - Lights
                              6,   // SWITCH2 - DitchLights
                              8    // SWITCH3 - Engine Start
    };

    writeAllGroupDataToEEPROM(0, defaultFn);
    writeAllGroupDataToEEPROM(1, defaultFn);
    writeAllGroupDataToEEPROM(2, defaultFn);
    writeAllGroupDataToEEPROM(3, defaultFn);
    writeAllGroupDataToEEPROM(4, defaultFn);
}

/* based on the function code, set the bit in the outgoing PT message packet */

void setFunc(int8_t f)
{
  uint8_t bits = 0x01;

  if (f < 8)
      functionCodes[0] |= (bits << f);
      
  if ( (f > 7) && ( f < 16))             
      functionCodes[1] |= (bits << (f-8));

  if ( (f > 15) && ( f < 24))             
      functionCodes[2] |= (bits << (f-16));

  if ( (f > 23) && ( f < 29))             
      functionCodes[3] |= (bits << (f-24));
}


void clearFunc(int8_t f)
{
  uint8_t bits = 0x01;

  if (f < 8)
      functionCodes[0] &= ~(bits << f);
      
  if ( (f > 7) && ( f < 16))             
      functionCodes[1] &= ~(bits << (f-8));

  if ( (f > 15) && ( f < 24))             
      functionCodes[2] &= ~(bits << (f-16));

  if ( (f > 23) && ( f < 29))             
      functionCodes[3] &= ~(bits << (f-24));
}


/* 
 * turn input bits from buttons/switches into function codes
 * then into the proper bits in the PT output data packet 
 * 
 */

uint8_t * processInputs( uint8_t loco, uint8_t B, uint8_t D)
{
    uint8_t *p;
    uint8_t fn;

    functionCodes[0] = 0;
    functionCodes[1] = 0;
    functionCodes[2] = 0;
    functionCodes[3] = 0;

    p = getGroupData(loco);

    fn = p[0];        // get function code for button 0
    if (B & BUTTON0)
       setFunc(fn);   
    else
      clearFunc(fn);

    fn = p[1];        // get function code for button 1
    if (B & BUTTON1)
       setFunc(fn);   
    else
      clearFunc(fn);

    fn = p[2];        // get function code for button 2
    if (D & BUTTON2)
       setFunc(fn);   
    else
      clearFunc(fn);

    fn = p[3];        // get function code for switch 0
    if (D & SWITCH0)
       setFunc(fn);   
    else
      clearFunc(fn);

    fn = p[4];        // get function code for switch 1
    if (D & SWITCH1)
       setFunc(fn);   
    else
      clearFunc(fn);

    fn = p[5];        // get function code for switch 2
    if (D & SWITCH2)
       setFunc(fn);   
    else
      clearFunc(fn);

    fn = p[6];        // get function code for switch 3
    if (D & SWITCH3)
       setFunc(fn);   
    else
      clearFunc(fn);

    return functionCodes;
}
