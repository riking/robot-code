// CPE123 - Fall 2013
// Lab 1 - Part 3
// Kane York and Nicolas Avila
//
// This program displays the numbers from 1 to 7 in binary on LEDs, looping forever
// A simple modified version of Part2.

#define __AVR_ATmega328P__ 1
#include <avr/io.h>
#include <util/delay.h>

// TODO change these to the correct ones upon recieveing the LEDs
#define xDDR DDRB
#define xPORT PORTB
#define x1pin PB4
#define x2pin PB5
#define x4pin PB6

// because these are macros, they get substituted into the code. yey!
#define MASK(pin) (1 << pin)
#define ONMASK(reg,mask) reg |= mask
#define OFFMASK(reg,mask) reg &= ~mask
#define ONPIN(reg,pin) reg |= MASK(pin)
#define OFFPIN(reg,pin) reg &= ~MASK(pin)

void showNumber(int number); // c, y u do dis 2 me :(

int main(void) {
	// setup data direction
	ONMASK(xDDR, MASK(x1pin) | MASK(x2pin) | MASK(x4pin));

	int i = 0;
	int dir = 1;
	while (1) {
		showNumber(i);
		// reverse direction @ endpoints (TODO this seems like something that could have an external validity test)
		if (i == 7) dir = -1;
		if (i == 0) dir = 1;
		i += dir;
		_delay_ms(300);
	}
}

void showNumber(int number) {
	if (number & 4) ONPIN(xPORT, x4pin); else OFFPIN(xPORT, x4pin);
	if (number & 2) ONPIN(xPORT, x2pin); else OFFPIN(xPORT, x2pin);
	if (number & 1) ONPIN(xPORT, x1pin); else OFFPIN(xPORT, x1pin);
}
