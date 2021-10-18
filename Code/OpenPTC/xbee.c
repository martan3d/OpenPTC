/*
 * xbee.c
 *
 * Created: 9/22/2021 12:34:11 PM
 *  Author: martan
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>
#include <string.h>
#include <stdio.h>

#define BUFSIZE   48

#define HEADER    0
#define BODY      1
#define LEN1      2
#define LEN2      3
#define XCMD      4
#define ATRESP    5
#define CKSUM     6
#define FRAMETYPE 7

#define ATCMD		0x88
#define TXMSG		16
#define QUERYMSG	11

#define RXBROADCAST 0x81
#define RXMSGRX 16

uint8_t txprocess[BUFSIZE];
uint8_t rxbuffer[BUFSIZE];
uint8_t rxQueue[BUFSIZE];

uint8_t txi = 0;
uint8_t rxi = 0;

uint8_t rxbyte;
uint8_t rxstate;
uint8_t msgRx;
uint8_t rxlen;

uint8_t tximsglen = 0;
uint8_t txinprog = 0;

#define FCPU  16000000

void initSerial(long baud)
{
    long   baud_prescale = (FCPU / (baud * 16)) - 1;	// from the data sheet 8mhz clock
    UCSR0B = (1 << RXEN0) | (1 << TXEN0);				// enable tx and rx
    UCSR0C = (1 << USBS0) | (3 << UCSZ00);				// 8 bits, 1 stop, no parity
    UBRR0L = baud_prescale;								// baud rate
    UBRR0H = (baud_prescale >> 8);
    
}

void setRXbuff(void)
{
    uint8_t j,i;
    i=0;
    for(j=0;j<BUFSIZE;j++)    // set to all zeroes
    rxbuffer[i++]=0;
}

// RX Interrupt enable methods

void enableRXIRQ(void){
    UCSR0B |= (1 << RXCIE0);
    rxi = 0;
}

void resetRXptr(void){
    rxi = 0;
}

void disableTXIRQ(void){
    UCSR0B &= ~(1< TXCIE0);
}

void enableTXIRQ(uint8_t c)
{
    UDR0 = c;
    UCSR0B |= (1 << TXCIE0);
    UCSR0A |= (1 << TXC0);
    txi = 1;
}

void startTransmit(uint8_t * rxb, uint8_t txl)
{
    txinprog = 1;
    for(uint8_t i=0;i<txl;i++)
    txprocess[i] = rxb[i];
    txi = 1;
    tximsglen = txl;
    enableTXIRQ(txprocess[0]);
}

uint8_t computeCRC(uint8_t * t, uint8_t dlen)
{
    uint8_t i = 0;
    uint8_t cks = 0;
    for(i=3;i<dlen; i++)				// compute checksum
    cks += t[i];
    return(0xff-cks);
}

#define TXMSGDATA 16
uint8_t txReq[TXMSGDATA+10];

// transmit Protothrottle data frame - use this for broadcast messages that look like the PT

uint8_t * xbeeTransmitPTFrame(uint8_t dh, uint8_t dl, uint8_t * data)
{
    uint8_t i = 0;
    uint8_t j = 0;
    
    txReq[0] = 0x7e;                    // Start
    txReq[1] = 0;                       // Length MSB
    txReq[2] = TXMSGDATA + 4;           // Length LSB
    txReq[3] = 1;                       // 16 bit transmit Frame Type
    txReq[4] = 0;                       // Frame ID for ACK
    txReq[5] = dh;                      // 16 bit destination back to rpi
    txReq[6] = dl;
    txReq[7] = 0;                       // options none
    
    j = 0;
    for(i=8;i<TXMSGDATA+7;i++)
    txReq[i] = data[j++];
    
    // 28                               27
    txReq[TXMSGDATA+7] = computeCRC(txReq, TXMSGDATA+7);

    startTransmit(txReq, TXMSGDATA+8);  // 29

    return txReq;
}

#define TXMSGLENGTH 60                      // max message size
uint8_t txRequest[TXMSGLENGTH+10];

// transmit data directed frame to 16bit destination addres (dh,dl)

uint8_t * xbeeTransmitDataFrame(uint8_t dh, uint8_t dl, uint8_t * data, uint8_t msglen)
{
    uint8_t i = 0;
    uint8_t j = 0;

    txRequest[0] = 0x7e;                    // Start
    txRequest[1] = 0;                       // Length MSB
    txRequest[2] = msglen + 4;              // Length LSB
    txRequest[3] = 1;                       // 16 bit transmit Frame Type
    txRequest[4] = 0;                       // Frame ID for ACK
    txRequest[5] = dh;                      // 16 bit destination back to rpi
    txRequest[6] = dl;
    txRequest[7] = 0;                       // options none
    
    j = 0;
    for(i=8;i<msglen+7;i++)
    txRequest[i] = data[j++];
    
    txRequest[msglen+7] = computeCRC(txRequest, msglen+7);

    startTransmit(txRequest, msglen+8);

    return txRequest;
}


uint8_t transmitting()
{
    return txinprog;
}


/** Transmit interrupt */

ISR(USART_TX_vect)
{
    if(txi >= tximsglen)
    {
        disableTXIRQ();
        txinprog = 0;
        return;
    }
    UDR0 = txprocess[txi++];
}


/*** Receive Interrupt For Xbee API packets ***/


ISR(USART_RX_vect)
{
    // PORTD ^= 0x20;
    
    rxbyte = UDR0;                          // Get the byte out of the UART
    //UDR0 = rxbyte;    // DEBUG: echo
    switch(rxstate)                         // Decide what to do with it
    {
        case HEADER:
        if(rxbyte != 0x7e)				          // make sure this is start of message
        break;
        //rxi = 0;						              // reset counter/index
        rxbuffer[rxi++] = rxbyte;	          // store the byte
        rxstate = LEN1;				              // wait for Length
        break;
        
        case LEN1:
        rxbuffer[rxi++] = rxbyte;	          // length high
        rxstate = LEN2;
        rxlen = rxbyte;
        rxlen = rxlen << 8;                 // MSB into high byte
        break;
        
        case LEN2:
        rxbuffer[rxi++] = rxbyte;	          // length low
        rxlen = rxlen | rxbyte;             // LSB into low byte
        rxlen = rxlen + 2;                  // add in header, length
        rxstate = FRAMETYPE;			          // next should be frame type
        break;
        
        case FRAMETYPE:                     // Frame Type
        rxbuffer[rxi++] = rxbyte;           // save it
        switch(rxbyte)
        {
            case ATCMD:			                // Type of frame coming in?
            rxstate = ATRESP;               // AT response?
            break;
            
            case RXBROADCAST:               // Transmit
            rxstate = BODY;		              // data from another xbee node
            break;
        }
        break;
        
        case ATRESP:                        // response from AT message
        rxbuffer[rxi++] = rxbyte;	          // store the byte
        if(rxi > rxlen)
        {
            memcpy(rxQueue, rxbuffer, rxlen);
            rxi = 0;
            msgRx = RXMSGRX;
            rxstate = HEADER;               // start over and wait for next header and message
        }
        break;
        
        case BODY:                          // Response from datagram message
        rxbuffer[rxi++] = rxbyte;	          // store the byte
        if(rxi > rxlen)
        {
            memcpy(rxQueue, rxbuffer, rxlen);
            rxi = 0;
            msgRx = RXMSGRX;
            rxstate = HEADER;			          // wait for next header and message
        }
        break;
    }
    
}
