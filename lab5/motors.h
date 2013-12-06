#ifndef _motors_H
#define _motors_H 1

// ========================== //
// ==== MOTOR OPERATIONS ==== //
// ========================== //

// Exported Symbols

// Call this at the beginning of the program to start the motors running.
void initialize_motor_timer(void);

// This method is the main API method.
void set_motor_speed(char motor_number, signed int speed);


// ========================== //
// ====    DEFINITIONS   ==== //
// ========================== //


#include "masks.h"

void initialize_motor_timer(void) {
	// Timer/Counter Control Register
	// ClockSelect = Prescaler 8 (16MHz -> 2 MHz)
	TCCR1B = 2;                  //increment timer at 2MHz
	TIMSK1 |= (1 << OCIE1A); //enable output compare match
	sei();                   //enable global interrupts
	OCR1A = 1000;

	ONPIN(DDRD, MOTOR1);
	ONPIN(DDRD, MOTOR2);
	ONPIN(DDRD, MOTOR3);
}

// _motor_number & 3 is index into _motor_speeds of current
// if that gives 3, skip the cycle
// _motor_number & 4 is 0 if turning on, 1 if turning off
// 4th element in _motor_speeds is ignored
static signed char _motor_speeds[] = {0, 0, 0, 0};
static volatile signed char _motor_number = 0;
static volatile int _motor_wait;

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
		if (_motor_speeds[i]) {
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
	}
	TCNT1 = 0;
}

void set_motor_speed(char motor_number, signed int speed) {
	motor_number--; // pretend we're 1-indexed to the caller
	if ((motor_number < 0) || (motor_number > 2)) return;
	signed char real_speed;
	if (speed < -100) real_speed = -100;
	else if (speed > 100) real_speed = 100;
	else real_speed = speed;

	_motor_speeds[motor_number] = speed;
}

#endif
