// CPE123 - Fall 2013
// Lab 2 - Part 1
// Kane York and Alexander Bohlen
//
// This program blinks LED1 five times each time the switch is pressed.


#define __AVR_ATmega328P__ 1
#include <avr/io.h>
#include <util/delay.h>

#include "masks.h"

#define LED1 PD3
#define SW1 PD6

int main(void) {
	ONPIN(DDRD, LED1);
	OFFPIN(DDRD, SW1);
	while (1) {
		...
	}
}

