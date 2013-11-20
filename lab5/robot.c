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

int check_starting_bit(){
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

int main() {
	lab4_initialize_timer0();
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
