/*
 * functions.h
 *
 * Created: 10/18/2021 12:29:43 PM
 * Author: martan
 * 
 */ 

#if defined(__cplusplus)
extern "C" {
#endif

#ifndef FUNCTIONS_H_
#define FUNCTIONS_H_

void defaultFunctionCodes();
uint8_t * processInputs( uint8_t loco, uint8_t B, uint8_t D);

uint8_t getDebug();

#endif /* FUNCTIONS_H_ */

#if defined(__cplusplus)
}
#endif
