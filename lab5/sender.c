// CPE 123 - Fall 2013
// Lab Final - Sender arduino code 
// Froylan Aguirre, Kane York, Nicolas Avila, and Easton Hargear
//
// xxxx

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "masks.h"

#define ST_LED PD5
#define IR_LED PD3
#define SW1 PB0
#define SW2 PB1
#define SW3 PB2
#define SW4 PB3
#define SW5 PB4

#define IR_CODE_BASE 180

#define HIGH_US_38KHZ 7
#define LOW_US_38KHZ 20
#define HIGH_US_56KHZ 8
#define LOW_US_56KHZ 10

//Global variables for switching carrier frequencies
//Variables are global for timing correctness
static unsigned char loop_count;
static char logic_one_delay, logic_zero_delay;
static char frequency;

void set_frequency_38(void) {
  frequency = 38;
  loop_count = 89;  //number of periods in 2.4ms
  logic_one_delay = 45;
  logic_zero_delay = 22;
}

void set_frequency_56(void) {
  frequency = 56;
  loop_count = 141;  //number of periods in 2.4ms
  logic_one_delay = 70;
  logic_zero_delay = 35;
}

void send_start_bit() {
  unsigned char i;

  for (i=0;i<loop_count;i++) {
    PORTD |= (1 << IR_LED);  //turn on the IR LED
    if (frequency == 38) {
      _delay_us(HIGH_US_38KHZ);
    } else {
      _delay_us(HIGH_US_56KHZ);
    }

    PORTD &= ~(1 << IR_LED);  //turn off the IR LED
    if (frequency == 38) {
      _delay_us(LOW_US_38KHZ);
    } else {
      _delay_us(LOW_US_56KHZ);
    }
  }

  _delay_us(600);
}

void send_command(unsigned char command) {
  char i,j,delay;

  for (i=0;i<8;i++) {  //run 8 times
    if ((command & (1 << i)) != 0) {
      delay = logic_one_delay;
    } else {
      delay = logic_zero_delay;
    }

    for (j=0;j<delay;j++) {
      PORTD |= (1 << IR_LED); //turn on the IR LED
      if (frequency == 38) {
        _delay_us(HIGH_US_38KHZ);
      } else {
        _delay_us(HIGH_US_56KHZ);
      }

      PORTD &= ~(1 << IR_LED);  //turn off the IR LED
      if (frequency == 38) {
        _delay_us(LOW_US_38KHZ);
      } else {
        _delay_us(LOW_US_56KHZ);
      }
    }

    _delay_us(600);
  }
}

int main(void) {
  DDRD |= (1 << IR_LED);  //set the IR port pin to output

 
  while(1) {
    send_start_bit();
    send_command(18); //send code for Volume up 
    _delay_ms(100);   //send no more than 1 command every 100ms
  }

  return 0;
}

void wait_for_press(void) {
	while (1) {
		if (GETPIN(PINB, SW1) == 0) break;
		if (GETPIN(PINB, SW2) == 0) break;
		if (GETPIN(PINB, SW3) == 0) break;
		if (GETPIN(PINB, SW4) == 0) break;
		if (GETPIN(PINB, SW5) == 0) break;
	}
	ONPIN(DDRD, ST_LED);
	_delay_ms(1000);
	OFFPIN(DDRD, ST_LED);
}

int main(void) {
	ONPIN(DDRD, IR_LED);
	ONPIN(PORTD, IR_LED);
	ONPIN(DDRD, ST_LED);
	ONPIN(PORTD, ST_LED);

	ONMASK(PORTB, MASK(SW1) | MASK(SW2) | MASK(SW3) | MASK(SW4) | MASK(SW5));
	_delay_ms(200);

	// Pick a frequency
	char hf = 0;
	hf = GETPIN(PINB, SW1) != 0;

	OFFPIN(PORTD, ST_LED);
	_delay_ms(400);
	ONPIN(PORTD, ST_LED);
	_delay_ms(200);
	OFFPIN(PORTD, ST_LED);
	_delay_ms(200);
	if (hf) {
		ONPIN(PORTD, ST_LED);
		_delay_ms(200);
		OFFPIN(PORTD, ST_LED);
		_delay_ms(200);
		set_frequency_56(); // 56 khz
	} else {
		set_frequency_38(); //set the transmit frequency to 38KHz
	}

	while(1) {
		wait_for_press();
		send_start_bit();
		unsigned char code = read_press();
		code += IR_CODE_BASE;
		send_command(code);
	}
}
