// CPE123 - Fall 2013
// Lab 2 - Part 1
// Kane York and Alexander Bohlen
//
// This program blinks LED1 five times each time the switch is pressed.


#define __AVR_ATmega328P__ 1
#include <avr/io.h>
#include <util/delay.h>

#include "masks.h"

#define LED1 PD4
#define SW1 PD6

// 
void blinkLed() {
	ONPIN(PORTD, LED1);
	_delay_ms(120);
	OFFPIN(PORTD, LED1);
	_delay_ms(280);
}

int main(void) {
	// setup
	ONPIN(DDRD, LED1);
	OFFPIN(DDRD, SW1);

	while (1) {
		// delay 'while' the switch pin is high
		while (GETPIN(PIND, SW1))
			_delay_ms(100);
		blinkLed(); blinkLed();blinkLed();blinkLed();blinkLed();
	}
}

