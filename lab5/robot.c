// CPE 123 - Fall 2013
// Lab Final - Robot code
// Froylan Aguirre, Kane York, Nicolas Avila, and Easton Hargear
//
// xxxx

#include <avr/interrupt.h>
#include <util/delay.h>
#include "masks.h"

#define MOTOR1 PD1 // left
#define MOTOR2 PD3 // right
#define MOTOR3 PD2
#include "motors.h"

#define SW PB2
#define LED PB3

#define IR_LO PD6
#define IR_HI PD7

#define IR_CODE_BASE 180

void lab4_initialize_timer0(void) {
  TCCR0B |= (1 << CS02);   // set timer 0 to increment at 62.5KHz
  TCCR2B |= (1 << CS10);
  TCCR2B |= (1 << CS12);   // set timer 2 to increment at 15.63KHz
  TIMSK2 |= (1 << OCIE2A); // enable output compare match
  OCR2A = 0;
}

/*
timer goes to 7327*/
#define SHUTOFF_TIME 187561
volatile long timer = 0;
ISR(TIMER2_COMPA_vect) {
	timer++;
}


char falling_edge(int timeout, char hf) {
	char current_signal, last_signal;
	char pin = (hf) ? (IR_HI) : (IR_LO);

	current_signal = GETPIN(PIND, pin);

	TCNT0 = 0;
	while (TCNT0 <= timeout) {
		last_signal = current_signal;

		current_signal = GETPIN(PIND, pin);

		if (!current_signal && last_signal) {
			return 1;
		}
	}
	return 0; // no falling edge
}

char check_starting_bit(char hf){
	char falling_edgeA = falling_edge(40, hf);
	if (falling_edgeA == 0){
		return 0;
	} else {
		//OFFPIN(PORTB, LED);
		char falling_edgeB = falling_edge(150, hf);
		if (falling_edgeB == 0) {
			return 1;
		} else {
			return 0;
		}
	}
}

// returns 0 on no start bit
// returns interpreted IR code otherwise
unsigned char read_ir(char hf) {
	char bits[8];
	char i, chk, a;
	char start = check_starting_bit(hf);
	if (!start) {
		return 0;
	}
	for (i = 0; i < 8; i++) {
		bits[i] = 0;
	}

	OFFPIN(PORTB, LED);
	char pin = (hf) ? (IR_HI) : (IR_LO);
	for (i = 0; i < 8; i++) {
		// assumes there is a falling edge detected.
		// waits .9ms, if no falling edge and dependent on bit read after .9ms.
		// 62500
		// copied from part1.c
		chk = falling_edge(75, hf);
		if (chk) {
			chk = falling_edge(/*57*/ 55, hf);
			if (chk) {
				_delay_ms(6345);
				break; // falling edge, no bit to read.
			} else {
				a = GETPIN(PIND, pin);
				if (a != 0) {
					// no falling edge, current signal is on high which -> 0;
					bits[i] = 0;
					
				} else {
					// current signal on low which -> 10
					bits[i] = 1;
				}
			}
		}
	}
	unsigned char ret = 0;
	for (i = 0; i < 8; i++) {
		ret |= (bits[i] << i);
	}
	return ret;
}

int main() {
	// initialize timers
	lab4_initialize_timer0();
	initialize_motor_timer();
	ONPIN(DDRB, LED);
	ONPIN(PORTB, SW);

	// Frequency switch
	ONPIN(PORTB, LED);
	_delay_ms(200);
	OFFPIN(PORTB, LED);
	_delay_ms(200);
	char hf = 0;
	if (!GETPIN(PINB, SW) != 0) {
		hf = 1;
		_delay_ms(200);
		ONPIN(PORTB, LED);
		_delay_ms(200);
		OFFPIN(PORTB, LED);
		_delay_ms(200);
	}

	_delay_ms(200);
	ONPIN(PORTB, LED);

	while (1) {
		// Read the infared code, then react to it
		unsigned char ir = read_ir(1);

		switch (ir) {
		case 1 + IR_CODE_BASE:
			set_motor_speed(1, 100);
			set_motor_speed(2, -100);
			break;
		case 2 + IR_CODE_BASE:
			set_motor_speed(1, -100);
			set_motor_speed(2, 100);
			break;
		case 3 + IR_CODE_BASE:
			set_motor_speed(1, 100);
			set_motor_speed(2, 100);
			break;
		case 4 + IR_CODE_BASE:
			set_motor_speed(1, -100);
			set_motor_speed(2, -100);
			break;
		case 5 + IR_CODE_BASE:
			set_motor_speed(3, 30);
			break;
		case 6 + IR_CODE_BASE:
			set_motor_speed(3, -20);
			break;
		case 7 + IR_CODE_BASE: // stop
			set_motor_speed(1, 0);
			set_motor_speed(2, 0);
			break;
		case 8 + IR_CODE_BASE:
			break;
		case 9 + IR_CODE_BASE:
			break;
		case 10 + IR_CODE_BASE:
			break;
		default:
			ir = 0;
			break;
		}
		// Make flash if valid signal
		if (ir > IR_CODE_BASE && ir < 10 + IR_CODE_BASE) {
			ONPIN(PORTB, LED);
			_delay_ms(100);
			OFFPIN(PORTB, LED);
		}
		// Check for time limit
		if (timer > SHUTOFF_TIME) {
			set_motor_speed(1, 0);
			set_motor_speed(2, 0);
			ONPIN(PORTB, LED);
			_delay_ms(200);
			OFFPIN(PORTB, LED);
			_delay_ms(200);
			ONPIN(PORTB, LED);
			_delay_ms(200);
			OFFPIN(PORTB, LED);
			_delay_ms(200);
			ONPIN(PORTB, LED);
			_delay_ms(200);
			OFFPIN(PORTB, LED);
			_delay_ms(200);
			ONPIN(PORTB, LED);
			_delay_ms(200);
			OFFPIN(PORTB, LED);
			_delay_ms(200);
			ONPIN(PORTB, LED);
			_delay_ms(200);
			OFFPIN(PORTB, LED);
			_delay_ms(200);
			return 0; // kill program
		}
	}
	return 0;
}
