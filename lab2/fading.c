// CPE123 - Fall 2013
// Lab 2 - Part 1
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

#define PERIOD_us 1000

void doCycle(int pin, float duty) {
	int onTime = (int) (PERIOD_us * duty);
	int offTime = PERIOD_us - onTime;
	ONPIN(PORTD, pin);
	_delay_us(onTime);
	OFFPIN(PORTD, pin);
	_delay_us(offTime);
}

void fadeInOut(int pin) {
	float duty;
	for (duty = 0; duty < 1; duty += 0.2) {
		doCycle(pin, duty);
		doCycle(pin, duty);
		doCycle(pin, duty);
	}
	for (duty = 1; duty > 0; duty += 0.2) {
		doCycle(pin, duty);
		doCycle(pin, duty);
		doCycle(pin, duty);
	}
}

int main(void) {
	fadeInOut(LED1);
	fadeInOut(LED2);
	fadeInOut(LED3);
}

