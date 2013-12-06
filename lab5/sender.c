// CPE 123 - Fall 2013
// Lab Final - Sender arduino code 
// Froylan Aguirre, Kane York, Nicolas Avila, and Easton Hargear
//
// xxxx

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "masks.h"

#define ST_LED PB1
#define IR_LED PB3
#define ST_SW PB0
#define SW1 PD3
#define SW2 PD4
#define SW3 PD5
#define SW4 PD6
#define SW5 PD7

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
    PORTB |= (1 << IR_LED);  //turn on the IR LED
    if (frequency == 38) {
      _delay_us(HIGH_US_38KHZ);
    } else {
      _delay_us(HIGH_US_56KHZ);
    }

    PORTB &= ~(1 << IR_LED);  //turn off the IR LED
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
      PORTB |= (1 << IR_LED); //turn on the IR LED
      if (frequency == 38) {
        _delay_us(HIGH_US_38KHZ);
      } else {
        _delay_us(HIGH_US_56KHZ);
      }

      PORTB &= ~(1 << IR_LED);  //turn off the IR LED
      if (frequency == 38) {
        _delay_us(LOW_US_38KHZ);
      } else {
        _delay_us(LOW_US_56KHZ);
      }
    }

    _delay_us(600);
  }
}

/*
sw1 = forward
sw5 = backward
sw2 = turn left
sw3 = turn right
sw4 = arm state
*/
char pr_forward = 0;
char pr_turn = 0;
char pr_arm = 0;
unsigned char read_press() {
	if (GETPIN(PIND, SW4) != pr_arm) {
		pr_arm = GETPIN(PIND, SW4);
		if (pr_arm == 0) {
			return 6 + IR_CODE_BASE; // arm down
		} else {
			return 5 + IR_CODE_BASE; // arm up
		}
	}
	if (!GETPIN(PIND, SW1) && (pr_forward != 1)) {
		pr_forward = 1;
		return 1 + IR_CODE_BASE; // forward
	} else if (!GETPIN(PIND, SW5) && (pr_forward != -1)) {
		pr_forward = -1;
		return 2 + IR_CODE_BASE; // backward
	} else if (GETPIN(PIND, SW1) && GETPIN(PIND, SW5) && pr_forward != 0) {
		pr_forward = 0;
		pr_turn = 0;
		return 7 + IR_CODE_BASE; // stop
	}
	
	if (!GETPIN(PIND, SW2) && (pr_turn != 1)) {
		pr_turn = 1;
		return 3 + IR_CODE_BASE; // left
	} else if (!GETPIN(PIND, SW3) && (pr_turn != -1)) {
		pr_turn = -1;
		return 4 + IR_CODE_BASE; // right
	} else if (GETPIN(PIND, SW2) && GETPIN(PIND, SW3) && pr_turn != 0) {
		pr_forward = 0;
		pr_turn = 0;
		return 7 + IR_CODE_BASE; // stop
	}

	return 0;
}

int main(void) {
	ONPIN(DDRB, IR_LED);
	ONPIN(DDRB, ST_LED);

	ONPIN(PORTB, ST_SW);

	ONMASK(PORTD, MASK(SW1) | MASK(SW2) | MASK(SW3) | MASK(SW4) | MASK(SW5));
	OFFMASK(DDRD, MASK(SW1) | MASK(SW2) | MASK(SW3) | MASK(SW4) | MASK(SW5));
	_delay_ms(800);

	// Pick a frequency
	char hf = 0;
	hf = GETPIN(PINB, ST_SW) == 0;
	OFFPIN(PORTB, ST_SW);

	ONPIN(PORTB, ST_LED);
	_delay_ms(200);
	OFFPIN(PORTB, ST_LED);
	_delay_ms(200);
	if (hf) {
		ONPIN(PORTB, ST_LED);
		_delay_ms(200);
		OFFPIN(PORTB, ST_LED);
		_delay_ms(200);
		set_frequency_56(); // 56 khz
	} else {
		set_frequency_38(); //set the transmit frequency to 38KHz
	}

	while(1) {
		unsigned char code = read_press();
		if (code != 0) {
			ONPIN(PORTB, ST_LED);
			send_start_bit();
			send_command(code);
			_delay_us(200);
			OFFPIN(PORTB, ST_LED);
			_delay_us(3300);
		}
		OFFPIN(PORTB, ST_LED);
	}
}
