// CPE123 - Fall 2013
// Lab 2 - Part 3
// Kane York and Alexander Bohlen
//
// This program causes the motors to slowly accelerate to full speed,
// then decelerate to a stop, reverse, and do the same.
// The robot should end up in the starting postition.


#define __AVR_ATmega328P__ 1
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#include "masks.h"

#define MOTOR1 PD5
#define MOTOR2 PD6
#define MOTOR3 PD7

// ========================== //
// ==== MOTOR OPERATIONS ==== //
// ========================== //

void initialize_motor_timer(void) {
	// Timer/Counter Control Register
	// ClockSelect = Prescaler 8 (16MHz -> 2 MHz)
	TCCR1B = 2;                  //increment timer at 2MHz
	TIMSK1 |= (1 << OCIE1A); //enable output compare match
	sei();                   //enable global interrupts
	OCR1A = 1000;
}

// _motor_number & 3 is index into _motor_speeds of current
// if that gives 3, skip the cycle
// _motor_number & 4 is 0 if turning on, 1 if turning off
// 4th element in _motor_speeds is ignored
signed char _motor_speeds[] = {0, 0, 0, 0};
volatile signed char _motor_number = 0;
volatile int _motor_wait;
// stagger the motors!

ISR(TIMER1_COMPA_vect) {
	signed char i = _motor_number & 3;
	if (_motor_number & 4) {
		// Turning off
		OCR1A = /*6000*/ + _motor_wait;

		// Next action: Turn on, so increment i, signal off.
		_motor_number = (i + 1) & 3; //% 4;
		switch (i) {
		case 0:
			OFFPIN(PORTD, MOTOR1);
			break;
		case 1:
			OFFPIN(PORTD, MOTOR2);
			break;
		case 2:
			OFFPIN(PORTD, MOTOR3);
			break;
		case 3:
			OCR1A = 6000 + 6000 + 6000 + 6000;
			break;
		}
	} else {
		// Turning on
		int diff = 10 * (int) _motor_speeds[i];
		_motor_wait = 1000 - diff;
		OCR1A = 2000 + 1000 + diff;

		// Next action: Turn off, so flip the signal bit
		_motor_number |= 4;
		switch (i) {
		case 0:
			ONPIN(PORTD, MOTOR1);
			break;
		case 1:
			ONPIN(PORTD, MOTOR2);
			break;
		case 2:
			ONPIN(PORTD, MOTOR3);
			break;
		}
	}
	TCNT1 = 0;
}

void set_motor_speed(int motor_number, signed char speed) {
	motor_number--; // pretend we're 1-indexed to the caller
	if ((motor_number < 0) || (motor_number > 2)) return;
	if (speed < -100) speed = -100;
	else if (speed > 100) speed = 100;

	_motor_speeds[motor_number] = speed;
}

// ========================== //
// ======== MAIN CODE ======= //
// ========================== //


void move(signed char speed) {
	set_motor_speed(1, speed);
	set_motor_speed(2, -speed);
}

int main(void) {
	initialize_motor_timer();

	ONMASK(DDRD, MASK(MOTOR1) | MASK(MOTOR2) | MASK(MOTOR3) );

	signed char speed;
	signed char delta = 1;

	// accelerate
	for (speed = 0; speed < 50; speed += delta) {
		move(speed);
		_delay_ms(200);
		if (speed > 10) delta++;
		if (speed > 30) delta++;
	}
	// stop, accelerate in reverse
	for (speed = 50; speed > 0; speed -= delta) {
		move(speed);
		_delay_ms(200);
		if (speed > 10) delta--;
		if (speed > 30) delta--;
		if (delta < 1) delta = 1;
	}
	delta = 1;
	for (speed = 0; speed > -50; speed -= delta) {
		move(speed);
		_delay_ms(200);
		if (speed < -10) delta++;
		if (speed < -30) delta++;
	}
	// stop slowly
	for (speed = -50; speed < 0; speed += delta) {
		move(speed);
		_delay_ms(200);
		if (speed < -10) delta--;
		if (speed < -30) delta--;
		if (delta < 1) delta = 1;
	}
}

