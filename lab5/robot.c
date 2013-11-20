// CPE 123 - Fall 2013
// Lab Final - Robot code
// Froylan Aguirre, Kane York, Nicolas Avila, and Easton Hargear
//
// xxxx

#include <avr/interrupt.h>
#include <util/delay.h>
#include "masks.h"

#define MOTOR1 PD5
#define MOTOR2 PD6
#define MOTOR3 PD7
#include "motors.h"

#define SW PB2
#define LED PB3

int main() {
	ONPIN(DDRD, LED);
	ONPIN(PORTB, LED);
	ONPIN(PORTB, SW);
	_delay_ms(200);
	char hf = 0;
	OFFPIN(PORTB, SW);
	if (GETPIN(PINB, SW) != 0) {
		hf = 1;
		_delay_ms(200);
		ONPIN(PORTB, SW);
		_delay_ms(200);
		OFFPIN(PORTB, SW);
	}

	initialize_motor_timer();

}
