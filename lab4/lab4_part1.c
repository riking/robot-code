// CPE 123 - Fall 2013
// Lab 4 - Part 1
// Froylan Aguirre and Kane York
//
// This program makes the LED blink depending on what IR signal is recieved.

#define __AVR_ATmega328P__ 1
#include <avr/io.h>
#include <util/delay.h>

#include "masks.h"

#define IRSENSOR PD2
#define LED PD4
#define BOARDLED PB5

//initialize timer 0 for lab 4
void initialize_ir_timer(void) {
TCCR0B |= (1 << CS02);   //set timer 0 to increment at 62.5KHz
}

char read_IR();
// This function checks if the infared sensor is sensing anything.
// This function should be called at least every 2 milliseconds to avoid missing anything.
// The function will halt execution if no incoming reading is detected.
// If a signal is recieved, the numeric value will be returned.
// If no signal is recieved, -1 will be returned.

char high_bit(char port);
char findFallingEdge(char port, char timeout);

char read_IR() {
	OFFPIN(PORTB, BOARDLED);

	if (!findFallingEdge(IRSENSOR, 75+3)) {
		return -1; // no start bit
	}
	if (findFallingEdge(IRSENSOR, 156+3)) {
		return -1; // bad start bit
	}

	// we have a start bit - turn on led
	ONPIN(PORTB, BOARDLED);

	char ret = 0;
	char i, tmp;
	for (i = 0; i < 7; i++) {
		tmp = high_bit(IRSENSOR);
		if (tmp == -1) return -1;
		if (tmp) {
			ret |= (1 << i);
		}
	}
	OFFPIN(PORTB, BOARDLED);
	return ret;
}

// 2.5 ms @ 62.5 KHz = 156 +1
// 1.2 ms @ 62.5 KHz = 75 +1
// 0.9 ms @ 62.5 KHz = 56.3 +2

// (boolean)
// does a single cycle read on the ir sensor, should be looped 7 times
// -1 if abort
char high_bit(char port) {
	char tmp;
	if (!findFallingEdge(port, 75+3)) {
		return -1;
	}
	if (findFallingEdge(port, 57+3)) {
		return -1;
	}
	return !GETPIN(PIND, port);
}

// return 1 if we get a falling edge within timeout
// return 0 if no falling edge is found
char findFallingEdge(char port, char timeout) {
	TCNT0 = 0;
	// wait until we have any high, then continue to next loop
	while (TCNT0 < timeout) {
		if (GETPIN(PIND, port))
			break;
	}

	// wait until we have a low
	while (TCNT0 < timeout) {
		if (!GETPIN(PIND, port))
			return 1;
	}
	return 0;
}
int main(void) {
	ONPIN(DDRD, LED);
	ONPIN(DDRB, PB5);

	ONPIN(PORTD, LED);

	initialize_ir_timer();
	char ir;
	char cnt, blink_count;

	while (1) {
		ir = read_IR();
		if (ir >= 0) {
			switch (ir) {
			case 18:
				blink_count = 1;
				break;
			case 19:
				blink_count = 2;
				break;
			case 16:
				blink_count = 3;
				break;
			case 17:
				blink_count = 4;
				break;
			default:
				blink_count = 0;
				break;
			}
			/*
			ONPIN(PORTD, LED);
			_delay_ms(100);
			OFFPIN(PORTD, LED);
			_delay_ms(100);
			ONPIN(PORTD, LED);
			_delay_ms(100);
			OFFPIN(PORTD, LED);
			_delay_ms(100);
			while (ir) {
				if (ir & 1) ONPIN(PORTD, LED);
				else OFFPIN(PORTD, LED);
				ir <<= 1;
				_delay_ms(400);
			}
			ONPIN(PORTD, LED);
			_delay_ms(100);
			OFFPIN(PORTD, LED);
			_delay_ms(100);
			ONPIN(PORTD, LED);
			_delay_ms(100);
			OFFPIN(PORTD, LED);
			_delay_ms(100);
*/
		} else {
			blink_count = 0;
		}

		for (cnt = 0; cnt < blink_count; cnt++) {
			ONPIN(PORTD, LED);
			_delay_ms(500);
			OFFPIN(PORTD, LED);
			_delay_ms(500);
		}
		blink_count = 0;

		_delay_ms(1);
	}
}
