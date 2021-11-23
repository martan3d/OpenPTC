/*
 * xbee.h
 *
 * Created: 9/22/2021 12:35:55 PM
 *  Author: martan
 */ 
 
#if defined(__cplusplus)
extern "C" {
#endif

#ifndef XBEE_H_
#define XBEE_H_

void initSerial( long baud);
void disableTXIRQ(void);
void enableTXIRQ(uint8_t c);
void startTransmit(uint8_t * rxb, uint8_t txl);
uint8_t transmitting();
uint8_t computeCRC(uint8_t * t, uint8_t dlen);
uint8_t * xbeeTransmitDataFrame(uint8_t dh, uint8_t dl, uint8_t * data, uint8_t msglen);
uint8_t * xbeeTransmitPTFrame(uint8_t dh, uint8_t dl, uint8_t * data);
int8_t msgRX(void);
void clrmsgRX(void);
uint16_t getRXAddress();
void enableRXIRQ(void);


#endif /* XBEE_H_ */

#if defined(__cplusplus)
}
#endif
