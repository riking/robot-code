#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#define MOTOR1 PD5
#define MOTOR2 PD6
#define MOTOR3 PD7

#include "motors.h"

#define rightsensor() readADC(1)
#define leftsensor() readADC(0)

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

int main(void){
	unsigned char q;
	unsigned char w;
	int a;//velocity of motor
	int c = 1;//how the velocity changes
	int d;//controls turning (0 is left, 1 is right)
	int v;//velocity of motors ranges from -100 to 100
	//an. pin 0 is left sensor
	//an. pin 1 is right sensor
	//right motor might be faster than left motor
	//black produces higher signal than white at same altitudes
	set_motor_speed(3,0);
	ONPIN(DDRD, PD6);
	ONPIN(DDRD, PD5);
	while (GETPIN(PIND, PD3)){} //when switch is pressed, the robot begins

	initialize_motor_timer();
	initialize_adc();
	while (1){
		_delay_us(100);//must have this delay to make sure the read sensor has enough time to execute
		q = rightsensor();
		_delay_us(100);//must have this delay to make sure the read sensor has enough time to execute
		w = leftsensor();
		a = (170-(1.1*q)) + 70;//equations converting sensor output intovelocity
		v = (170-(1.1*w)) + 70;//equations converting sensor output intovelocity
		set_motor_speed(1,v);	
		set_motor_speed(2,a);
		_delay_ms(5);
	}
	return 0;
}					
