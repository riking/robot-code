// CPE123 - Fall 2013
// Lab 2 - Part 2
// Kane York and Alexander Bohlen
//
// This program fades in and out three LEDs.


#define __AVR_ATmega328P__ 1
#include <avr/io.h>
#include <util/delay.h>

#include "masks.h"

#define LED1 PD3
#define LED2 PD5
#define LED3 PD7

// spend one millisecond with the LED on for the first [duty / 100] fraction
void doCycle(int pin, signed char duty) {
	char offTime = 100 - duty;
	ONPIN(PORTD, pin);
	// use a counter, because _delay_us doesn't like dynamic numbers for some reason :<
	while (duty) {
		_delay_us(10);
		duty--;
	}
	OFFPIN(PORTD, pin);
	while (offTime) {
		_delay_us(10);
		offTime--;
	}
}

#define TWELVE(x) do {\
	x; x; x; x; x; x; x; x; x; x; x; x;\
} while(0)

void fadeInOut(int pin) {
	signed char duty;
	// fade in
	for (duty = 0; duty < 100; duty += 2) {
		TWELVE(doCycle(pin, duty));
	}

	doCycle(pin, 100);
	doCycle(pin, 100);
	doCycle(pin, 100);

	// fade out
	for (duty = 100; duty > 0; duty -= 2) {
		TWELVE(doCycle(pin, duty));
	}
}

int main(void) {
	ONPIN(DDRD, LED1);
	ONPIN(DDRD, LED2);
	ONPIN(DDRD, LED3);
	while (1) {
		fadeInOut(LED1);
		_delay_ms(250);
		fadeInOut(LED2);
		_delay_ms(250);
		fadeInOut(LED3);
		_delay_ms(250);
	}
}

