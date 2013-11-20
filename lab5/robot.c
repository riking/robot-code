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

#define IR_LO PD2
#define IR_HI PD3

void lab4_initialize_timer0(void) {
  TCCR0B |= (1 << CS02);   //set timer 0 to increment at 62.5KHz
}

char falling_edge(int timeout) {
	char current_signal = PIND;
	current_signal &= (1 << PD5);
	char last_signal = current_signal;
	TCNT0=0;
	while(TCNT0 < timeout) {
		current_signal = PIND;
		current_signal &= (1 << PD5);
		if(current_signal == 0 && last_signal == 32) {
			return 1;
		}
		else {
			last_signal = current_signal;
		}
	}
	return 0; // no falling edge
}

char check_starting_bit(){
	char falling_edgeA = falling_edge(150);
		if(falling_edgeA == 0){
			return 0;
		}else{
			char falling_edgeB = falling_edge(155);
			if(falling_edgeB == 1){
				return 0;
			}else{
				//PORTD &= ~(1 << PD7);
				return 1;
			}
	}
}//end of check_starting_bit

/*
||
||
||		
*/

int main() {
	lab4_initialize_timer0();
	ONPIN(DDRD, LED);
	ONPIN(DDRD, IR_LO);
	ONPIN(DDRD, IR_HI);
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
	int commands[5] = [0,0,0,0,0];
	char bits[7];
	char i, j;
	while(1) {
		char start = check_starting_bit();
		if(start == 1) {
			for(i = 0; i<7; i++) {
				// assumes there is a falling edge detected.
				// waits .9ms, if no falling edge and dependent on bit read after .9ms.
				// 62500
				// copied from part1.c
				chk = falling_edge(75);
				if(chk == 1) {
					chk = falling_edge(57);
					if(chk == 1) {
						i = 7; // falling edge, no bit to read.
					} else {
						char a = PIND & (1 << PD5);
						if(a == 32) {
							// no falling edge, current signal is on high which -> 0;
							bits[i] = 0;
							
						} else {
							// current signal on low which -> 10
							bits[i] = 1;
						}
					}
				}
				if(i == 6) {
					// bit by bit comparison
					char eighteen[7] = {0,1,0,0,1,0,0};
					for(j = 0; j<7; j++){
						if(bits[j] == eighteen[j]){
							command[0]++;
						}
					}
					char nineteen[7] = {1,1,0,0,1,0,0};
					for(j = 0; j<7; j++){
						if(bits[j] == nineteen[j]){
							command[1]++;
						}
					}
					char sixteen[7] = {0,0,0,0,1,0,0};
					for(j = 0; j<7; j++){
						if(bits[j] == sixteen[j]){
							command[2]++;
						}
					}
					char seventeen[7] = {1,0,0,0,1,0,0};
					for(j = 0; j<7; j++){
						if(bits[j] == seventeen[j]){
							command[3]++;		
						}
					}			
					if(command[0] == 7){
						// checks counter and sets the wheel speed accordingly
						set_motor_speed(0,100);
						set_motor_speed(1,100);
					}
					if(command[1] == 7){
						set_motor_speed(0,-100);
						set_motor_speed(1,-100);						
					}
					if(command[2] == 7){
						set_motor_speed(0,0);
						set_motor_speed(1,100);
					}
					if(command[3] == 7){
						set_motor_speed(0,100);
						set_motor_speed(1,0);
					}
					//reset comparison counters
					command[0] = 0, command[1] = 0, command[2] = 0, command[3] = 0;
				}
			}
		}
	}
}
