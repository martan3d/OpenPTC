/*
 * buttons.c
 *
 * Created: 9/14/2013 9:31:58 AM
 * Author: martan
 */

#include <avr/io.h>
#include "buttons.h"

// debounce filters

int8_t BFilter[3];
int8_t DFilter[6];

// working data

uint8_t Binputs = 0;
uint8_t Dinputs = 0;

// debounced data

uint8_t Bdata = 0;
uint8_t Ddata = 0;

/* init I/O ports 
 *  
 *   0 = input pin, 
 *   1 = output pin
 * 
 * 
 */

uint8_t leds[5] = { LED0, LED1, LED2, LED3, LED4, LEDSTOP };

#define BOUNCE 4

void initButtons()
{
  DDRD   = 0x01;                                                                 // D0 = Tx output to Xbee, D1 = Rx
  DDRD  &= ~(SELECTBUTTON | SWITCH0 | SWITCH1 | SWITCH2 | SWITCH3 | BUTTON2);    // inputs = 0
  PORTD |=  (SELECTBUTTON | SWITCH0 | SWITCH1 | SWITCH2 | SWITCH3 | BUTTON2);    // turn on internal pullups (important)
                                
  DDRB   =  (LED3 | LED4 | LEDSTOP);                                             // all inputs except these
  PORTB |=  (BUTTON0 | BUTTON1 | DIRECTION | LED3 | LED4 | LEDSTOP);             // Pullups for buttons and direction switch, turn off LEDS

  DDRC    = ~ADC;                          // PC0 = ADC input, not really needed, config of ADC takes preference all others outputs
  PORTC  |= (~ADC | LED0 | LED1 | LED2);   // outputs to 1 turns OFF leds

  BFilter[0] = BFilter[1] = BFilter[2] = 0;
  DFilter[0] = DFilter[1] = DFilter[2] = DFilter[3] = DFilter[4] =  DFilter[5] = 0;
}

/*
 * Return our local data values 
 * 
 */

uint8_t getBData()
{
   return Bdata;
}

uint8_t getDData()
{
   return Ddata;
}

/*
 * Debounce Input Buttons
 *
 */

uint8_t filterInput(uint8_t inputs, uint8_t button, int8_t *filter)
{
    uint8_t rdata = 0;

    if(inputs & button)          // Button inputs
      (*filter)++;
    else
      (*filter)--;

    if( *filter > BOUNCE)        // Must see one this many scans
    {
      rdata |= button;
      *filter = BOUNCE;
    }
    
    if( *filter < 1)             // or zero this many scans
    {
      rdata &= ~(button);
      *filter = 0;
    }

  return rdata;
}
    
   
/*
 * Grab button states from the ports, zero means pressed, so invert them 
 *
 */

 
void scanButtons()
{
    /****** PORTB input pins ******/

    Binputs = PINB;              // get all input data
    Binputs = ~Binputs;          // invert bits, 0 means ON, change to 1 = ON

    Bdata = 0;
    Bdata |= filterInput(Binputs, BUTTON0, &BFilter[0]);
    Bdata |= filterInput(Binputs, BUTTON1, &BFilter[1]);
    Bdata |= filterInput(Binputs, DIRECTION, &BFilter[2]);
    
    /********* PORTD input pins **********/

    Dinputs = PIND;              // get all input data
    Dinputs = ~Dinputs;          // invert bits, 0 means ON, change to 1 = ON

    Ddata = 0;
    Ddata |= filterInput(Dinputs, SELECTBUTTON, &DFilter[0]);
    Ddata |= filterInput(Dinputs, SWITCH0, &DFilter[1]);
    Ddata |= filterInput(Dinputs, SWITCH1, &DFilter[2]);
    Ddata |= filterInput(Dinputs, SWITCH2, &DFilter[3]);
    Ddata |= filterInput(Dinputs, SWITCH3, &DFilter[4]);
    Ddata |= filterInput(Dinputs, BUTTON2, &DFilter[5]);

}
  

/* turn on 'index' LED from 0-4 Actually sets the port to 'low' */

void outputLEDS(uint8_t index)
{
       switch(index)
       {
          case 0:
          case 1:
          case 2:
                  PORTC = (LED0 | LED1 | LED2);
                  PORTB = (LED3 | LED4 | LEDSTOP);
                  PORTC &= ~leds[index];
                  break;
          case 3:
          case 4:
                  PORTC = (LED0 | LED1 | LED2);
                  PORTB = (LED3 | LED4 | LEDSTOP);
                  PORTB &= ~leds[index];
                  break;
       }
}
