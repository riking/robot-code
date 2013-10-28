// CPE123 - Fall 2013
// Lab 2 - Part 2
// Kane York and Froylan Aguirre
//
// Line-following program

#define __AVR_ATmega328P__ 1
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#include "masks.h"

#define MOTOR1 PD5
#define MOTOR2 PD6
#define MOTOR3 PD7
#define SWITCH1 PD3
#define LIGHT0 2
#define LIGHT1 0

#include "motors.h"

//call this first to enable the analog-to-digital converter
void initialize_adc(void) {
	ADCSRA |= (1<<ADPS2) |  (1<<ADPS1) |  (1<<ADPS0);  //set prescalar
	ADMUX |= (1<<REFS0);  //set reference voltage
	ADMUX |= (1<<ADLAR);  //left align data
	ADCSRA |= (1<<ADEN);  //enable ADC
}
//call this to read one of the analog input pins
//example:   readADC(0);   --read input 0 and return the 8-bit digital number
unsigned char readADC(unsigned char num) {
	unsigned char temp;
	ADMUX = (ADMUX & 0xE0) | num; //set ADC input
	ADCSRA = ADCSRA | 0x40;       //start analog to digital conversion
	while (ADCSRA & (1<<ADSC));   //wait for conversion to finish
	temp = ADCH;                  //read upper 8bits
	return temp;
}


/*
motor1 RIGHT
motor2 LEFT

light0 LEFT
light1 RIGHT


*/

int factorial(int n) {
	if (n == 0) return 1;
	if (n == 1) return 1;
	return n * factorial(n - 1);
}


// reminder: motor1 +forward; motor2 -forward
int main(void) {
	initialize_motor_timer();
	initialize_adc();
	OFFPIN(DDRD, SWITCH1);

	// Wait for switch to be pressed
	while(GETPIN(PIND, SWITCH1)) ;

	while(1) {
		unsigned char light_left = readADC(LIGHT0);
		_delay_us(100);
		unsigned char light_right = readADC(LIGHT1);
		signed int light_diff = light_left - light_right;
		//light_diff *= 20;
		//if (light_diff > 80) light_diff = 80;
		//if (light_diff < -80) light_diff = -80;

		// BAD VERSION: Ignore right light sensor, follow line edge with left sensor
		if (light_left > 200) {
			set_motor_speed(1, 00);
			set_motor_speed(2, 10);
		} else if (light_left > 190) {
			set_motor_speed(1, -10);
			set_motor_speed(2, 10);
		} else {
			set_motor_speed(1, -10);
			set_motor_speed(2, 00);
		}
	}
}
