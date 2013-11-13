//CPE123 - Fall 2013
//Lab 4 - part 2
//Froylan Aguirre with partner Kane York
//
//Controls the Arduino using a remote
//.......................................................
//......................................................
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#define RmotorON (PORTD = PORTD | 32 )
#define RmotorOFF (PORTD = PORTD & ~ 32)
#define LmotorON (PORTD = PORTD | 64)
#define LmotorOFF (PORTD = PORTD & ~ 64)

int lone; //length of time of signal to a motor
int ltwo; //length of time of signal to a motor
int lthree; //length of time of signal to some other device
volatile int state = 0;//the current part of a cycle (on/off and which motor)
volatile int t = 20000 ; //how much time the timer is updated
volatile int oone = 00000; //how much time a motor is off
volatile int otwo = 00000;//how much time a motor is off
volatile int othree = 00000;//how much time a motor is off
char bits[7];//array of the bits read from the IR sensor

//initialize timer 0 for lab 4
void lab4_initialize_timer0(void) {
  TCCR0B |= (1 << CS02);   //set timer 0 to increment at 62.5KHz
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
		//conditions for a fall
		if ((q == 0) && (v == 4)){
			timeout_delay = 0;
			w = 1;
		}else{ 
			w = 0;
		}
	}
	return w;	
}

void read_bits(){
	/*reads bits once start bit and certain conditions are met according to 
	algorithm presented by Dr.Seng*/
	char q = 0;
	char v;
	char c = 0; 
	char w = 0;
	for  (c=0;c<6;c=c+1){
		q = check_falling_edge(75.0);
		if (q == 1){
			v = check_falling_edge(56.25);
			if (v == 1){
				//nothing happens
			}else if (v == 0){
				w = PIND & (1<<PD2); 
				if (w == 0){
					bits[c] = 1; 
					//Interesting note: r = r + pow(2,c); doesnt work, only works for some buttons
				} else if (w == 1){
					bits[c] = 0;
				}
			}
		}else if (q == 0){ 
			//nothing happens
		}
		
	}
}

void initialize_timer(void) {
	TCCR1B = 2;                  //increment timer at 2MHz
	TIMSK1 |= (1 << OCIE1A); //enable output compare match
	sei();                   //enable global interrupts
}   

void set_motor_speed( int motor_number, int speed){
	//0 is left
	if (motor_number == 0){
		ltwo = (3000) + (speed*(-10)); //converts velocity to ticks
		otwo = 3500 - ltwo; //makes motion smooth
	}
	//1 is right
	if (motor_number == 1){
		lone = (3000) + (speed*10); //converts velocity to ticks
		oone = 2500 - lone;//makes motion smooth
	}
	//2 is the next device
	if (motor_number == 2){
		lthree = 0; 
		othree = 1000 - lthree;
	}

}


ISR(TIMER1_COMPA_vect){
	state = state + 1; //the next state
	//appropiate times for each state
	if (state == 2){
		t = oone;
		RmotorOFF;
	}
	if (state == 4){
		t = otwo;
		LmotorOFF;
	}
	if (state == 6){
		t = othree;
		state = 0;
	//	put next device here;
	}
	
	if (state == 1){
		t = lone;
		RmotorON;
	}
	if (state == 3){
		t = ltwo;
		LmotorON;
	}
	if (state == 5){
		t = lthree;
	//	put the next device here
	}
	//timer is updated by time t
	OCR1A = OCR1A + t;
	
} 

int main(void){
	char c = 0;
	char w = 0;
	char a = 0;
	DDRD = DDRD | (1<<PD5);
	DDRD = DDRD | (1<<PD6);
	DDRB = DDRB | (1<<PB5);//take this out before turning in
	initialize_timer();
	lab4_initialize_timer0();
	while (1){
		w = 0;//reset w every cycle
		//reset bits[] every cycle
		for (c=0;c<6;c=c+1){
			bits[c] = 0;
		}
		//Algorithm for Reading the IR Signals
		c = check_falling_edge(62.5);
		if (c == 1){
			a = check_falling_edge(156.25);
			if (a == 0){ 
				read_bits();
				for (c = 0; c<6;c=c+1){
					w = w + ((bits[c]) * (1<<c));
				}
			} else if (a ==1){
				w = 0;
			}

		} 
				
		switch(w){//test to see if two signals can be read at same time
			case 16://channel up,forward
				set_motor_speed(0,45);
				set_motor_speed(1,50);
				set_motor_speed(2,0);	
				PORTB = PORTB | (1<<PB5); //remove this line after turning in
				_delay_ms(150);
				break;
			case 17://channel down,backward
				set_motor_speed(0,-45);
				set_motor_speed(1,-50);
				set_motor_speed(2,0);	
				PORTB = PORTB | (1<<PB5);//remove this line after turning in 
				_delay_ms(200);
				break;
			case 18://volume up,turn right
				set_motor_speed(1,0);
				set_motor_speed(0,45);
				set_motor_speed(2,0);
				_delay_ms(150);
				break;
			case 19://volume down,turn left
				set_motor_speed(0,0);
				set_motor_speed(1,55);
				set_motor_speed(2,0);
				_delay_ms(150);
				break;
			case 20://mute
				break;
			case 21://power
				break;
			default://turns off motors when buttons are not pressed
				set_motor_speed(0,0);
				set_motor_speed(1,0);
				set_motor_speed(2,0);	
				PORTB = PORTB &~ (1<<PB5);//remove this line after turning in
				break;
		}
	}
	return 0;
}
