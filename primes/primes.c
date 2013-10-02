// CPE123 - Fall 2013
// Not a Lab - October 1
// Kane York
//
// This program calculates and displays the prime numbers.
// The display is done by cycling through the bits and displaying the low 3 on some LEDs.


#include <avr/io.h>
#include <util/delay.h>

#define INT_MAX_VALUE 0xFFFD

#define MASK(pin) (1 << pin)
#define ONMASK(reg,mask) reg |= mask
#define OFFMASK(reg,mask) reg &= ~mask
#define ONPIN(reg,pin) reg |= MASK(pin)
#define OFFPIN(reg,pin) reg &= ~MASK(pin)

void display_prime(unsigned int number);
unsigned int find_prime();

int main(void) {
	// setup pins
	ONMASK(DDRD, MASK(PD3) | MASK(PD5) | MASK(PD7));

	// Hardcode these, because it's easier to hardcode them than special-case them in find_prime()
	display_prime(2);
	display_prime(3);
	while(1)
		display_prime(find_prime());
}

/*
Expected output

 2 010
 3 011
 5 101
 7 111
11 1011
13 1101
17 10001
19 10011
23 10111
29 11100
*/

unsigned int last_prime = 3;

unsigned int find_prime() {
	unsigned int cur = last_prime;
	unsigned int try;

	while (1) {
		cur += 2;
		// we skip dividing by 2, because we skip all evens
		for (try = 3; try < cur / 2; try += 2) {
			if ((cur % try) == 0) {
				cur += 2;
				try = 3;
			}
		}
		break;
	}
	last_prime = cur;
	return cur;
}

void display_prime(unsigned int prime) {
	// Display the numbers by moving the bits down and displaying the current bottom 3.
	while (prime) {
		if (prime & 4) ONPIN(PORTD, PD7); else OFFPIN(PORTD, PD7);
		if (prime & 2) ONPIN(PORTD, PD5); else OFFPIN(PORTD, PD5);
		if (prime & 1) ONPIN(PORTD, PD3); else OFFPIN(PORTD, PD3);
		_delay_ms(250);
		prime >>= 1;
	}
	OFFPIN(PORTD, PD7);
	OFFPIN(PORTD, PD5);
	OFFPIN(PORTD, PD3);
	_delay_ms(400);
	ONPIN(PORTD, PD7);
	ONPIN(PORTD, PD5);
	ONPIN(PORTD, PD3);
	_delay_ms(100);
	OFFPIN(PORTD, PD7);
	OFFPIN(PORTD, PD5);
	OFFPIN(PORTD, PD3);
	_delay_ms(400);
}
