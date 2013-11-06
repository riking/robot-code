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
void waitFor1(char port, char timeout);
void waitFor0(char port, char timeout);

char read_IR() {
	OFFPIN(PORTB, BOARDLED);
	if (GETPIN(PIND, IRSENSOR)) return -1;
	// we have a 0 - start reading
	ONPIN(PORTB, BOARDLED);

	char ret = 0;	
	char i;
	for (i = 0; i < 7; i++) {
		if (high_bit(IRSENSOR)) {
			ret |= (1 << i);
		}
	}
	OFFPIN(PORTB, BOARDLED);
	return ret;
}

// (boolean)
// does a single cycle read on the ir sensor, should be looped 7 times
char high_bit(char port) {
	waitFor1(port, 80);
	waitFor0(port, 80);
	// Timer-wait for 0.6 ms: math is 37.5
	// Round up to 38 for safety
	TCNT0 = 0;
	while (TCNT0 < 39)
		;

	return !GETPIN(PIND, port);
}

// stall until the port is high
void waitFor1(char port, char timeout) {
	TCNT0 = 0;
	while (TCNT0 < timeout) {
		if (GETPIN(PIND, port))
			return;
		_delay_us(10);
	}
}
// stall until the port is low
void waitFor0(char port, char timeout) {
	TCNT0 = 0;
	while (TCNT0 < timeout) {
		if (!GETPIN(PIND, port))
			return;
		_delay_us(10);
	}
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
			
			while (ir) {
				if (ir & 1) ONPIN(PORTD, LED);
				else OFFPIN(PORTD, LED);
				ir <<= 1;
				_delay_ms(400);
			}
		} else {
			blink_count = -1;
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
