/*
 * adc.c
 *
 * Created: 9/14/2013 9:31:58 AM
 *  Author: martan
 */

#include <avr/io.h>
#include <avr/interrupt.h>

volatile static uint16_t adcval;
volatile static uint16_t high;

/* startup a continuous ADC IRQ cycle */

void initADC(void)
{
  ADMUX = 0b01000000;   // Internal 1.1v ref, channel 0

	ADCSRA =
	(1 << ADEN)  |        // Enable ADC
	(1 << ADATE) |        // auto trigger enable
	(1 << ADIE)  |        // enable ADC interrupt
	(1 << ADPS2) |
	(1 << ADPS1) |
	(0 << ADPS0);
    
	ADCSRB = 0;       // free running mode
	ADCSRA |= (1 << ADSC); // start conversions
}

/* get the current ADC converted value */

uint16_t getADC(void)
{
	return adcval;
}

/* IRQ vector for ADC */

ISR(ADC_vect)
{
	adcval = ADC;
}



 
