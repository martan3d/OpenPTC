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

uint8_t * getGroupData(uint8_t index);
void putGroupData(uint8_t index, uint8_t * data);

#endif /* EEDATA_H_ */

#if defined(__cplusplus)
}
#endif
