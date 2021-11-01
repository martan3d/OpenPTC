#include "message.h"
#include "xbee.h"

uint8_t *msgPtr;
uint8_t msgType;

#define GETALL    0x10
#define RETALL    0x11
#define PUTALL    0x20
#define WRITEEE   0x30

#define LOCOS     5
#define DATA      7

uint8_t  transmitBuffer[60];
uint8_t  *gptr;
uint16_t locoAddress;
uint16_t destinationAddress;

void processDirectedMessage()
{
    uint8_t i = 0;
    uint8_t j = 0;
    uint8_t k = 0;
    uint16_t ladr;
    uint8_t dl, dh;
    
    msgPtr = getRX();
    msgType = msgPtr[0];                                   // get our message code

    switch(msgType)
    {
        case GETALL:                                       // send all our data back to whoever asked for it
             transmitBuffer[i++] = RETALL;                 // message header

             for(k=0;k<LOCOS;k++)
             {
               locoAddress = getLocoAddress(k);            // locoAddress
               gptr = getGroupData(k);                     // group data
               transmitBuffer[i++] = locoAddress & 0x00ff;
               transmitBuffer[i++] = (locoAddress & 0xff00) >> 8;
               for(j=0;j<DATA;j++){
                   transmitBuffer[i++] = gptr[j];
               }
             }

             destinationAddress = getRXAddress();         // get last xbee address that sent us a directed message
             dl = destinationAddress & 0x00ff;
             dh = (destinationAddress & 0xff00) >> 8;
             
             xbeeTransmitDataFrame(dh, dl, transmitBuffer, i);
   
        break;

        case PUTALL:
             i = 0;
             for (k=0;k<LOCOS;k++)
             {
                  ladr = msgPtr[i++];
                  ladr |= (msgPtr[i++] << 8);
                  
                  gptr = getGroupData(k);
                  for (j=0;j<DATA;j++)
                  {
                    gptr[j] = *msgPtr++;
                  }
             }
        break;

        case WRITEEE:
             writeAllLocoAddressesToEEPROM();
             for (j=0;j<0;j++)
             {
                writeAllGroupDataToEEPROM(j, getGroupData(j)); 
             }
             break;
    }
}
