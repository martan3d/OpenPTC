/*
 * Timer.h
 * Implements simple microsecond timer
 *
 */
 
#include <avr/io.h>
#include <avr/interrupt.h>

static volatile uint32_t msClock;
uint16_t normalMs = 2000;

ISR(TIMER1_COMPB_vect)
{
   msClock++;
   OCR1B = normalMs + TCNT1;     // one ms from where we are 
}    

#define COMPAREB  0x04           // Compare B match interrupt

void initTimer()
{
    msClock = 0;                 // take defaults on all
    TCCR1A = 0;
    TCCR1B = 0;
    TCNT1  = 0;
    TCCR1B |= 0x02;              // clock select, divide sysclock by 8
    TIMSK1 |= COMPAREB;          // enable compareB interrupt
}

uint32_t getMsClock()
{
    return msClock;
}