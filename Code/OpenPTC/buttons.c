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

void initButtons()
{
  DDRD   = 0x01;                                                                 // D0 = Tx output to Xbee, D1 = Rx
  DDRD  &= ~(SELECTBUTTON | SWITCH0 | SWITCH1 | SWITCH2 | SWITCH3 | BUTTON2);    // inputs = 0
  PORTD |=  (SELECTBUTTON | SWITCH0 | SWITCH1 | SWITCH2 | SWITCH3 | BUTTON2);    // turn on internal pullups (important)
                                
  DDRB   =  (LED3 | LED4 | LEDSTOP);                                             // all inputs except these
  PORTB |=  (BUTTON0 | BUTTON1 | DIRECTION | LED3 | LED4 | LEDSTOP);             // Pullups for buttons and direction switch, turn off LEDS

  DDRC    = ~ADC;                          // PC0 = ADC input, not really needed, config of ADC takes preference all others outputs
  PORTC  |= (~ADC | LED0 | LED1 | LED2);   // outputs to 1 turns OFF leds
}

/* return our local data values */

uint8_t getBData()
{
   return Bdata;
}

uint8_t getDData()
{
   return Ddata;
}

/* grab button states from the ports, zero means pressed, so invert them */

void scanButtons()
{
    /****** PORTB input pins ******/

    Bdata = PINB;              // get all input data
    Bdata = ~Bdata;            // invert bits, 0 means ON, change to 1 = ON
     
    /********* PORTD input pins **********/

    Ddata = PIND;              // get all input data
    Ddata = ~Ddata;            // invert bits, 0 means ON, change to 1 = ON
     
   
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
