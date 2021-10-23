/*
 * eedata.h
 *
 * Created: 9/22/2021 12:29:43 PM
 *  Author: martan
 */ 

#if defined(__cplusplus)
extern "C" {
#endif

#ifndef EEDATA_H_
#define EEDATA_H_


void readAllLocoAddressesFromEEPROM();
void writeAllLocoAddressesToEEPROM();
void setLocoAddress(uint8_t index, uint16_t address);
uint16_t getLocoAddress(uint8_t index);
uint8_t * getGroupData(uint8_t index);
void setGroupData(uint8_t * index, uint8_t subindex, uint8_t value);
void readAllGroupDataFromEEPROM();
void writeGroupDataToEEPROM(uint8_t index, uint8_t * data);

#endif /* EEDATA_H_ */

#if defined(__cplusplus)
}
#endif
