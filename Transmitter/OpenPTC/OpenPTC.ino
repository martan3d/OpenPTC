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
 *  PROMINI PINS in order, in groups of three.  The software DEPENDS on these
 *  being in the correct order. Optional ISP hookup, two groups of three.
 *  (ISP is only needed if you 'brick' the bootloader on the ProMini)
 *
 *  TX   Xbee DIN                        RAW
 *  RX   XBee OUT                        GND                -----
 *  RST                                  RST                    |
 *                                                              |    ISP 0
 *  GND                                  VCC                -----
 *  PD2  SW4 Sel button                  PC3 LED 0
 *  PD3  SW3                             PC2 LED 1
 *  
 *  PD4  SW2                             PC1 LED 2
 *  PD5  SW1                             PC0 ADC Input
 *  PD6  SW0                             PB5 LED 3          -----
 *                                                              |
 *  PD7  BUTTON YELLOW                   PB4 LED 4              |    ISP 1
 *  PB0  BUTTON AUX (GREEN? blue)        PB3 LED STOP       -----
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
#include "message.h"

/*******************************************************************/

uint8_t  txdata[]  = {0xD0, 0x30, 0x0F, 0x6F, 0x93, 0x53, 0x08, 0x34, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x88};
uint16_t address   = 2606;
uint8_t  dir       = 0;

uint16_t adcValue  = 0;
uint32_t now       = 0;
uint32_t then      = 0;
uint8_t  updatenow = 0;

uint32_t nowbutton  = 0;
uint32_t thenbutton = 0;


uint8_t  BData = 0;
uint8_t  CData = 0;
uint8_t  DData = 0;
uint8_t  adc   = 0;

uint8_t BDataOld = 0;
uint8_t DDataOld = 0;

uint8_t *p;

#define MAXTIME 300
#define BUTTONTIME 200
#define PMLED   0x20

uint16_t maxtime = MAXTIME;
uint8_t  masterIndex = 0;
uint8_t  oldIndex = 0;
uint8_t  select = 0;
uint8_t  checkup = 1;

uint8_t getdebug();

int main(void)
{
  
  initSerial(38400);            // run Xbee at 34.8K baud
  initTimer();                  // Setup the millisecond timer
  initADC();                    // Startup the ADC
  initButtons();                // set IO ports
  
  DDRB |= PMLED;                // except status/debug LED, set that here

// temporary hard code inits
 // txdata[6] = address >> 8;     // temporary xbee loco address
 // txdata[7] = address & 0x00ff;
//defaultFunctionCodes();
//defaultLocoAddresses();
///////

  readAllGroupDataFromEEPROM();           // load sram loco groups from EEPROM
  readAllLocoAddressesFromEEPROM();       // load PT loco addresses from EEPROM
  
  address = getLocoAddress(masterIndex);
  txdata[6] = address >> 8;           // xbee loco address
  txdata[7] = address & 0x00ff;       // this is the address the receiver responds to  

  outputLEDS(masterIndex);      // turn on the proper loco indicator LEDs

  sei();                        // enable interrupts
        
  then = getMsClock();          // get initial clock reading
  thenbutton = getMsClock();
  
  while(1)
  {
        /* collect buttons and knob data, process and load data packet for PT broadcast */

        adc = (getADC()/8) & 0x7f;            // ADC runs on irq, get Knob value and adjust
    
        scanButtons();                        // Grab input data for buttons/switches

        BData = getBData();                   // return each port
        DData = getDData();

        updatenow = BData ^ BDataOld;
        updatenow |= DData ^ DDataOld;

        BDataOld = BData;
        DDataOld = DData;

        dir = ( (BData & DIRECTION) << 5);    // get the direction switch data and move it to bit 7

        p = processInputs( masterIndex, BData, DData);  // process the inputs into programmed function codes

        txdata[8]  = adc | dir;                // insert direction bit into adjusted throttle value
        txdata[9]  = p[3];                     // insert adjusted button readings into the PT broadcast packet
        txdata[10] = p[2];                     // for all of the function code possibilities
        txdata[11] = p[1];                     // see PT packet format above
        txdata[12] = p[0];

        txdata[14] = getLocoAddress(1);

        /* check to see if user has switched to a new locomotive */

        nowbutton = getMsClock();
        if ((nowbutton - thenbutton) > BUTTONTIME)
        {
            thenbutton = getMsClock();

            if( (DData & SELECTBUTTON) )           // Advance Loco Number (more later, need off/on/sleep)
              {
                masterIndex++;
                if (masterIndex > 4)
                    masterIndex = 0;
              }
        }

        /* if we have a new loco selected, reload the data and set the LEDS */
        
        if (oldIndex != masterIndex)           // only need to do this if masterIndex changes
        {                                      // no need to reload data unless we have to
           oldIndex = masterIndex;             // it changed, save for next pass
           
           address = getLocoAddress(masterIndex);
           txdata[6] = address >> 8;           // xbee loco address
           txdata[7] = address & 0x00ff;       // this is the address the receiver responds to

           outputLEDS(masterIndex);            // turn on the proper loco indicator LEDs
        }

        /* Send out data every second if nothing else going on */
        
        now = getMsClock();                    // time to send data?
        if ((now - then) > maxtime)
        {
            then = getMsClock();
              
            if (!transmitting()){
                xbeeTransmitPTFrame(0xff, 0xff, txdata);
            }                
        }

        /* if any data has changed since last pass, send it now
        if (updatenow)
        {
            if (!transmitting()){
                xbeeTransmitPTFrame(0xff, 0xff, txdata);
                updatenow = 0;
            }
        }
        */

        if (msgRX() != 0)                      // Xbee directed message come in?
        {
           processDirectedMessage();
        }
  }
}
