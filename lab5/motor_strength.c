// CPE 123 - Fall 2013
// Lab X - Part X
// Froylan Aguirre and Kane York
//
// xxxx

#define __AVR_ATmega328P__ 1
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#define MOTOR1 PD5
#define MOTOR2 PD6
#define MOTOR3 PD7
#include "motors.h"

int main(void) {
	initialize_motor_timer();
	ONPIN(DDRD, MOTOR1);
	ONPIN(DDRD, MOTOR2);
	ONPIN(DDRD, MOTOR3);
	ONPIN(DDRD, PD4);
	ONPIN(PORTD, PD4);

	while (GETPIN(PIND, PD0))
		;

	set_motor_speed(1, 100);
	set_motor_speed(2, 100);
	set_motor_speed(3, 100);
	while (1)
		;
}
