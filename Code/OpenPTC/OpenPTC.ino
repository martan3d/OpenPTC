#include <avr/io.h>
#include <avr/interrupt.h>

#include "adc.h"
#include "xbee.h"
#include "timer.h"

#define PMLED     0x20

/*
 * *****************************************************************
 */

uint8_t txdata[] = {0xD0, 0x30, 0x0F, 0x6F, 0x93, 0x53, 0x08, 0x34, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x88};
uint16_t address = 2606;
uint8_t rev = 0;
uint16_t adcValue = 0;


#define MAXTIME 1000
uint32_t now = 0;
uint32_t then = 0;

int main(void)
{
  DDRB |= PMLED;
  
  initSerial(38400);            // run Xbee at 34.8K baud
  initTimer();                  // Setup the millisecond timer
  initADC();                    // Startup the ADC

  txdata[6] = address >> 8;
  txdata[7] = address & 0x00ff;

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
           adcValue = getADC();
           txdata[8] = adcValue/10;
           xbeeTransmitPTFrame(0xff, 0xff, txdata);
        }

  }
  
}
