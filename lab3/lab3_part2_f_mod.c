#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#define leftmotor (PORTD = PORTD | (1 << PD6))
#define rightmotor (PORTD = PORTD | (1 << PD5))
#define rightsensor() readADC(1) 
#define leftsensor() readADC(0)

int lone; //length of time of signal to a motor
int ltwo; //length of time of signal to a motor
int lthree; //length of time of signal to some other device
volatile int state = 0;//the current part of a cycle (on/off and which motor)
volatile int t = 20000 ; //how much time the timer is updated
volatile int oone = 00000; //how much time a motor is off
volatile int otwo = 00000;//how much time a motor is off
volatile int othree = 00000;//how much time a motor is off

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

void initialize_timer(void) {
	TCCR1B = 2;                  //increment timer at 2MHz
	TIMSK1 |= (1 << OCIE1A); //enable output compare match
	sei();                   //enable global interrupts
}   

void set_motor_speed( int motor_number, int speed){
	//motornumber is not the variable that controls turning, but it 
	//signals WHICH motor's speed is being updated
	//0 is left
	if (motor_number == 0){
		ltwo = (3000) + (speed*(10));//converts speed to ticks
	}
	//1 is right
	if (motor_number == 1){
		lone = (3000) + (speed*(-10));//converts speed to ticks
	}
	//2 is the third device
	if (motor_number == 2){
		lthree = 0 ;//unkown next device
	}
	
	//the off times for each motor, makes motion smooth
	oone = 1000 - lone;
	otwo = 1000 - ltwo;
	othree = 1000 - lthree;
}


ISR(TIMER1_COMPA_vect){
	state = state + 1;
	//appropiate times for each state
	if (state == 2){
		t = oone;
		PORTD = PORTD & ~ (1<<PD5);
	}
	if (state == 4){
		t = otwo;
		PORTD = PORTD & ~ (1<<PD6);
	}
	if (state == 6){
		t = othree;
		state = 0;
	//	put next device here;
	}
	
	if (state == 1){
		t = lone;
		rightmotor;
	}
	if (state == 3){
		t = ltwo;
		leftmotor;
	}
	if (state == 5){
		t = lthree;
	//	put the next device here
	}
	//timer is updated by time t
	OCR1A = OCR1A + t;
	
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
	set_motor_speed(2,0);
	DDRD = DDRD | (1 << PD6);
	DDRD = DDRD | (1 << PD5);
	while (PIND & 8){} //when switch is pressed, the robot begins
	initialize_timer();
	initialize_adc();
	while (1){
		q = rightsensor();
		_delay_us(100);//must have this delay to make sure the read sensor has enough time to execute
		w = leftsensor();
		_delay_us(100);//must have this delay to make sure the read sensor has enough time to execute
		a = (170-(1.1*q)) + 70;//equations converting sensor output intovelocity
		v = (170-(1.1*w)) + 70;//equations converting sensor output intovelocity
		set_motor_speed(0,v);	
		set_motor_speed(1,a);
		_delay_ms(50);
	}
	return 0;
}					
