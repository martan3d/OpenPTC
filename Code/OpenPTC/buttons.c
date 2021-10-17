/*
 * buttons.c
 *
 * Created: 9/14/2013 9:31:58 AM
 * Author: martan
 */

#include <avr/io.h>
#include "buttons.h"

// button data read from i/o pins



// debounced data
uint8_t BdataOut = 0;
uint8_t CdataOut = 0;
uint8_t DdataOut = 0;

int8_t Bcnt[5];
int8_t Ccnt[4];
int8_t Dcnt[8];

uint8_t mask = 1;
uint8_t i = 0;

uint8_t Bdata = 0;
uint8_t Cdata = 0;
uint8_t Ddata = 0;

#define BOUNCE 10

/* init I/O ports 
 *  
 *   0 = input pin, 
 *   1 = output pin
 * 
 */

void initButtons()
{
  DDRD   = 0x01;                // D0 = Tx output to Xbee, D1 = Rx
  DDRD  &= ~(SELECTBUTTON | SWITCH0 | SWITCH1 | SWITCH2 | SWITCH3 | BUTTON2);  // inputs = 0
  PORTD |=  (SELECTBUTTON | SWITCH0 | SWITCH1 | SWITCH2 | SWITCH3 | BUTTON2);  // pullups
                                
  DDRB   = 0x00;                 // all inputs
  PORTB |=  (BUTTON0 | BUTTON1); // Pullups for buttons

  DDRC   = ~ADC;                 // PC0 = ADC input
}

uint8_t getBData()
{
   return Bdata;
}

uint8_t getCData()
{
   return Cdata;
}

uint8_t getDData()
{
   return Ddata;
}


void scanButtons()
{
    /****** PORTB input pins ******/

    Bdata = PINB;              // get all input data
    Bdata = ~Bdata;            // invert bits, 0 means ON, change to 1 = ON

    /****** PORTC input pins ******/

    Cdata = PINC;              // get all input data
    Cdata = ~Cdata;            // invert bits, 0 means ON, change to 1 = ON
     
    /********* PORTD input pins **********/

    Ddata = PIND;              // get all input data
    Ddata = ~Ddata;            // invert bits, 0 means ON, change to 1 = ON
     
   
}
