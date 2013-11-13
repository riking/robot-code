//CPE123 - Fall 2013
//Lab 4 - part 1
//Froylan Aguirre and partner Kane York
//
//...Makes an LED blink a certain number of times depending on what button is prressed...
//.......................................................
//.......................................................
#include <avr/io.h>
#include <util/delay.h>

#define LEDon (PORTB = PORTB | (1<<PB5))
#define LEDoff (PORTB = PORTB & ~ (1<<PB5))

//initialize timer 0 for lab 4
void lab4_initialize_timer0(void) {
  TCCR0B |= (1 << CS02);   //set timer 0 to increment at 62.5KHz
}

void blink_led( char t ){
	//makes LED blink t times
	char c;
	for (c=1;c<=t;c=c+1){
		LEDon;
		_delay_ms(300);
		LEDoff;
		_delay_ms(300);
	}
}

char check_falling_edge(float timeout_delay){
	//detects a falling edge, returns 1 if detected and 0 if not detected
	//must declare q and w to be 0 so that no accidental readings happen
	char q = 0;
	char v = 0;
	char w =0;
	TCNT0 = 0;
	while (TCNT0 <= timeout_delay){
		v = q;
		q = PIND & 4;		
		//conditions for fall
		if ((q == 0) && (v == 4)){
			timeout_delay = 0;
			w = 1;
		}else{ 
			w = 0;
		}
	}
	return w;	
}

char read_bits(){
	//reads the IR signals according to algorithm given by Dr.Seng
	char q = 0;
	char v;
	char c = 0; 
	char w = 0;
	char r = 0;
	for  (c=0;c<6;c=c+1){
		q = check_falling_edge(75.0);
		if (q == 1){
			v = check_falling_edge(56.25);
			if (v == 1){
				r = 0;
			}else if (v == 0){
				w = PIND & (1<<PD2); 
				if (w == 0){
					r = r + (1<<c); 
					//Interesting note: r = r + pow(2,c); doesnt work, only works for some buttons
				}
			}
		}else if (q == 0){ 
			r = 0;
		}
		
	}
	return r;
}

int main(void){
	char a = 0;
	char w = 0; 
	char c = 0;
	lab4_initialize_timer0();
	DDRB = DDRB | (1<<PB5);
	LEDoff;
	while(1){
		w = 0;//this resets w so that the LED doesnt keep blinking forever after button is pressed
		//Algorithm for Reading the IR Signals
		c = check_falling_edge(62.5);//checks for initial falling edge
		if (c == 1){
			a = check_falling_edge(156.25);
			if (a == 0){ 
				w = read_bits();
			} else if (a ==1){
				w = 0;
			}

		}
		
		switch (w){
			case 16://channel up
				blink_led(3);
				break;
			case 17://channel down
				blink_led(4);
				break;
			case 18://volume up
				blink_led(1);
				break;
			case 19://volume down
				blink_led(2);
				break;
			case 20://mute
				break;
			case 21://power
				break;
			}
		
	}
	return 0;
}
