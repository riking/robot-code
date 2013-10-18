// CPE123 - Fall 2013
// Lab 3 - Part 1
// Kane York and Froylan Aguirre
//
// This program turns on LED1 if the light sensor is dark.


#define __AVR_ATmega328P__ 1
#include <avr/io.h>
#include <util/delay.h>

#include "masks.h"

#define LIGHT1 0
#define LIGHT2 1
#define LED1 PD0


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

int main(void) {
	initialize_adc();
	ONPIN(DDRD, LED1);
	while (1) {
		unsigned char light = readADC(SENSOR1);
		if (light < 128) {
			ONPIN(PORTD, LED1);
		} else {
			OFFPIN(PORTD, LED1);
		}
	}
}

