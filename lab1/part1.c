// CPE123 - Fall 2013
// Lab 1 - Part 1
// Kane York and Nicolas Avila
//
// This program sets the pin5 of PORTB on and off each .4 seconds for three times.

#define __AVR_ATmega328P__ 1
#include <avr/io.h>

int MASK_PIN_5 = 0x10;

int main(void) {
    int mp5 = MASK_PIN_5;
    // TODO how to delay() without Arduino.h
    delay(1000);
    PORTB |= MASK_PIN_5;
    delay(400);
    PORTB &= ~MASK_PIN_5;
    delay(400);
    PORTB |= MASK_PIN_5;
    delay(400);
    PORTB &= ~MASK_PIN_5;
    delay(400);
    PORTB |= MASK_PIN_5;
    delay(400);
    PORTB &= ~MASK_PIN_5;
    delay(400);
    return 0;
}

