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
#define MOTOR3 PD5
#include "motors.h"

#define SW PB2
#define LED PB3

#define IR_LO PD6
#define IR_HI PD7

#define IR_CODE_BASE 180

void lab4_initialize_timer0(void) {
  TCCR0B |= (1 << CS02);   //set timer 0 to increment at 62.5KHz
}

char falling_edge(int timeout, char hf) {
	char current_signal = PIND;
	if(!hf) {
		current_signal &= (1 << IR_LO);
	} else {
		current_signal &= (1 << IR_HI);
	}
	char last_signal = current_signal;
	TCNT0=0;
	while(TCNT0 < timeout) {
		current_signal = PIND;
		if(!hf) {
			current_signal &= (1 << IR_LO);
		} else {
			current_signal &= (1 << IR_HI);
		}
		if(current_signal == 0 && last_signal == 32) {
			return 1;
		}
		else {
			last_signal = current_signal;
		}
	}
	return 0; // no falling edge
}

char check_starting_bit(char hf){
	char falling_edgeA = falling_edge(150, hf);
		if(falling_edgeA == 0){
			return 0;
		}else{
			char falling_edgeB = falling_edge(155, hf);
			if(falling_edgeB == 1){
				return 0;
			}else{
				//PORTD &= ~(1 << PD7);
				return 1;
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
		// assumes there is a falling edge detected.
		// waits .9ms, if no falling edge and dependent on bit read after .9ms.
		// 62500
		// copied from part1.c
		chk = falling_edge(75, hf);
		if(chk == 1) {
			chk = falling_edge(57, hf);
			if(chk == 1) {
				i = 7; // falling edge, no bit to read.
			} else {
				if(!hf) {
					a = PIND & (1 << IR_LO);
				} else {
					a = PIND & (1 << IR_HI);
				}
				if(a != 0) {
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

/*
|| 181 182 183 184 185
|| 186 187 188 189 190
*/

int main() {
	lab4_initialize_timer0();
	ONPIN(DDRD, LED);
	ONPIN(PORTB, LED);
	ONPIN(PORTB, SW);
	_delay_ms(200);
	OFFPIN(PORTB, LED);
	_delay_ms(200);
	char hf = 0;
	if (GETPIN(PINB, SW) != 0) {
		hf = 1;
		_delay_ms(200);
		ONPIN(PORTB, LED);
		_delay_ms(200);
		OFFPIN(PORTB, LED);
	}
	initialize_motor_timer();
	while(1) {
		unsigned char ir = read_ir(hf);
		switch (ir) {
		case 0:
			break;
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
			set_motor_speed(3, -30);
			break;
		case 7 + IR_CODE_BASE:
			set_motor_speed(1, 0);
			set_motor_speed(2, 0);
			break;
		case 8 + IR_CODE_BASE:
			break;
		case 9 + IR_CODE_BASE:
			break;
		case 10 + IR_CODE_BASE:
			break;
		}
	}
}
