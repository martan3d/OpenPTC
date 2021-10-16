/*
 * buttons.h
 *
 * Created: 9/22/2021 12:29:43 PM
 *  Author: martan
 */ 

#if defined(__cplusplus)
extern "C" {
#endif

#ifndef BUTTONS_H_
#define BUTTONS_H_

void initButtons();
uint8_t getBData();
uint8_t getCData();
uint8_t getDData();
void scanButtons();

#define DIRECTION    0x04     //PORTB2
#define SELECTBUTTON 0x04     //PORTD2
#define SWITCH0      0x40     //PORTD6
#define SWITCH1      0x20     //PORTD5
#define SWITCH2      0x10     //PORTD4
#define SWITCH3      0x08     //PORTD3
#define BUTTON0      0x02     //PORTB1   // BRAKE
#define BUTTON1      0x01     //PORTB0   // AUX
#define BUTTON2      0x80     //PORTD7   // HORN
#define LED0         0x08     //PORTC3
#define LED1         0x04     //PORTC2
#define LED2         0x02     //PORTC1
#define ADC          0x01     //PORTC0
#define LED3         0x20     //PORTB5
#define LED4         0x10     //PORTB4
#define LEDSTOP      0x08     //PORTB3


#endif /* BUTTONS_H_ */

#if defined(__cplusplus)
}
#endif
