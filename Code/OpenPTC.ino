#include <avr/io.h>
#include <avr/interrupt.h>

#include "adc.h"
#include "xbee.h"
#include "timer.h"

#define PMLED     0x20

/*
 * *****************************************************************
 */


#define MAXTIME 100
uint32_t now = 0;
uint32_t then = 0;

int main(void)
{

  DDRB |= PMLED;

  initSerial(38400);            // run Xbee at 34.8K baud
  initTimer();                  // Setup the millisecond timer
  initADC();                    // Startup the ADC

  sei();                        // enable interrupts
        
  then = getMsClock();          // get initial clock reading

  while(1)
  {
        /* toggle LED if its time */
        now = getMsClock();
        if ((now - then) > MAXTIME)
        {
           then = getMsClock();
           PORTB ^= PMLED;
        }

  }
  
}
