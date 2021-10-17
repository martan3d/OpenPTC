#include <avr/io.h>
#include <avr/interrupt.h>

/*  Protothrottle Clone 
 *   
 * This is the format of the PT packet sent out every 1000ms if nothing changes.  If there is a change, it's sent immediately
 * 
 *  Byte0    = Address of the base/receiver.  0xD0 to 0xEF.  The PT displays 0 to 31 for simplicity, but internally adds 0xD0 to the value.
 *  Byte1    = Address of the ProtoThrottle.  0x30 to 0x49.  The PT displays A to Z for simplicity, but internally translates to 0x30 to 0x49.
 *  Byte2    = 15 (packet size)
 *  Byte3,4  = CRC16 for data validation
 *  Byte5    = 'S' for Status Packet
 *  Byte 6,7 = 16-bit locomotive address.  Long address (0-9999) if MSB clear.  Short address (0-127) if MSB set.
 *  Byte 8   = Speed/direction.  Encoded the same as DCC "128 step" mode.  0 = idle, 1 = e-stop, 2-127 = speed, MSB = direction
 *  Byte 9   = Functions 28-24
 *  Byte 10  = Functions 23-16
 *  Byte 11  = Functions 15-8
 *  Byte 12  = Functions 7-0
 *  Byte 13  = Status bits.  Currently only supports emergency stop.
 *  Byte 14  = Battery voltage (20mV per LSB)
 *
 ******************************************************************
 *
 *  PROMINI PINS in order, in groups of three
 *
 *  TX                                   RAW
 *  RX                                   GND
 *  RST                                  RST
 *  
 *  GND                                  VCC
 *  PD2  SW4 Sel button                  PC3 LED 0
 *  PD3  SW3                             PC2 LED 1
 *  
 *  PD4  SW2                             PC1 LED 2
 *  PD5  SW1                             PC0 ADC Input
 *  PD6  SW0                             PB5 LED 3
 *
 *  PD7  BUTTON YELLOW                   PB4 LED 4
 *  PB0  BUTTON AUX (GREEN? blue)        PB3 LED STOP
 *  PB1  BUTTON RED                      PB2 Dir Switch
 *
 *
 */

#include "adc.h"
#include "xbee.h"
#include "timer.h"
#include "buttons.h"
#include "eedata.h"
#include "functions.h"

#define PMLED     0x20

/*
 * *****************************************************************
 */

uint8_t  txdata[] = {0xD0, 0x30, 0x0F, 0x6F, 0x93, 0x53, 0x08, 0x34, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x88};
uint16_t address = 2606;
uint8_t  rev = 0;

uint16_t adcValue = 0;
uint32_t now = 0;
uint32_t then = 0;

uint8_t  BData = 0;
uint8_t  CData = 0;
uint8_t  DData = 0;

uint8_t *p;

#define MAXTIME 300

int main(void)
{
  
  initSerial(38400);            // run Xbee at 34.8K baud
  initTimer();                  // Setup the millisecond timer
  initADC();                    // Startup the ADC
  initButtons();                // set IO ports
  DDRB |= PMLED;                // except LED, set that here

// temporary hard code inits
  txdata[6] = address >> 8;     // temporary xbee loco address
  txdata[7] = address & 0x00ff;
  defaultFunctionCodes();
///////

  sei();                        // enable interrupts
        
  then = getMsClock();          // get initial clock reading

  while(1)
  {
        /* collect I/O data, buttons and ADC */

        adcValue = getADC();

        scanButtons();

        BData = getBData();
        CData = getCData();
        DData = getDData();

        p = processInputs( 0, BData, DData);
        
        txdata[8]  = 0xff; //adcValue;
        txdata[9]  = p[0];
        txdata[10] = 0x011; //p[2];
        txdata[11] = 0x022; //p[1];
        txdata[12] = 0x033; //p[0];
        txdata[13] = 0xaa;

        now = getMsClock();
        if ((now - then) > MAXTIME)
        {
            then = getMsClock();

            if ( getDebug() )
                 PORTB ^= PMLED;

           if (!transmitting())
              xbeeTransmitPTFrame(0xff, 0xff, txdata);
        }
  }
}
