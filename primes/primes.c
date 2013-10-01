// CPE123 - Fall 2013
// Not a Lab
// Kane York
//
// This program displays the prime numbers.


#include <avr/io.h>
#include <util/delay.h>

#define INT_MAX_VALUE 0xFFFD

#define MASK(pin) (1 << pin)
#define ONMASK(reg,mask) reg |= mask
#define OFFMASK(reg,mask) reg &= ~mask
#define ONPIN(reg,pin) reg |= MASK(pin)
#define OFFPIN(reg,pin) reg &= ~MASK(pin)

void display_prime(int number);
int find_prime();

int main(void) {
	// setup pins
	ONMASK(DDRD, MASK(PD3) | MASK(PD5) | MASK(PD7));
	// TODO
	PORTD = 0xFF;

	//display_prime(2);
	display_prime(3);
	while(1)
		display_prime(find_prime());
}

/*
Expected output:

 2 010
 3 011
 5 101
 7 111
11 011
13 101
17 001
19 011
23 111
29 101
*/

int last_prime = 3;

int find_prime() {
	int cur = last_prime;
	int try;

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

void display_prime(int prime) {
	if (prime & 8) ONPIN(PORTD, PD7); else OFFPIN(PORTD, PD7);
	if (prime & 4) ONPIN(PORTD, PD5); else OFFPIN(PORTD, PD5);
	if (prime & 2) ONPIN(PORTD, PD3); else OFFPIN(PORTD, PD3);
	_delay_ms(400);
}
