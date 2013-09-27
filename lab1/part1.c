// CPE123 - Fall 2013
// Lab 1 - Part 1
// Kane York and Nicolas Avila
//
// This program sets the pin5 of PORTB on and off each .4 seconds three times.

#include <avr/io.h>
#include <util/delay.h>

#define mask5 (1 << PB5)

int main(void) {
	// set up pin for output
	DDRB |= mask5;

	// wait for a bit (note: this causes a brief flash of the led, because setting it for output turned it off)
	_delay_ms(1000);
	// copypaste because i'm lazy
	PORTB |= mask5;
	_delay_ms(400);
	PORTB &= ~mask5;
	_delay_ms(400);
	PORTB |= mask5;
	_delay_ms(400);
	PORTB &= ~mask5;
	_delay_ms(400);
	PORTB |= mask5;
	_delay_ms(400);
	PORTB &= ~mask5;
	_delay_ms(400);
	return 0;
}

