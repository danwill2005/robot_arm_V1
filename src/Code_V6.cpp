/*
 * robot_arm.cpp
 *
 * Created: 09/04/2026
 * Author : Daniel Williams
 */ 

//NEXT STEPS ATOMISE THE FUNCTIONS I E FUNCTION FOR LEFT RIGHT CENTRE RATHER THAN ALL IN THE SAME FUNCTION, USE POUNTERS TO PASS THE SERVO ANGLE RATHER THAN HAVEING 6 OF THE SAME LOOP IN 1 FUNCTION

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <util/atomic.h>


//This function sets up the registers in Timer 1 (16 bit) for Normal port and counter operation
void setup_pwm(){
	
	//TCCR1A register set to Normal port and clock operation
	TCCR1A = 0x00;
	
	//sets prescalar to 8
	TCCR1B = (1<<CS11);
	
	//Enables interrupt for TIMER1_COMPA
	TIMSK1 = (1<<OCIE1A); 
	
	//initialise OCR1A
	OCR1A = 0;
	
	//Enable global interrupts
	sei();
}


void setup_ports(){
	//sets pins 5,6,7 to output
	DDRD |= (1<<DDD5) | (1<<DDD6) | (1<<DDD7);
	
	//sets pins 8,9,9 to output
	DDRB |= (1<<DDB0) | (1<<DDB1) | (1<<DDB2);
	
}

//cycle stage for the switch case, allows for a series of pulses within the 20ms window
volatile uint8_t cycle_stage = 0;

//these will be the tick values that equate to the ms value of the pulse width that determines a specific angle, servos have range 0.5-2.5ms --> 1000 - 5000 ticks
volatile uint16_t servo1_angle = 3000;
volatile uint16_t servo2_angle = 3000;
volatile uint16_t servo3_angle = 3000;
volatile uint16_t servo4_angle = 3000;
volatile uint16_t servo5_angle = 3000;
volatile uint16_t servo6_angle = 3000;

ISR(TIMER1_COMPA_vect){
	
	switch(cycle_stage){
		//start of 20ms cycle
		//want to set the pulse of servo 1
		case 0:
			PORTD |= (1<<PORTD5);//sets pin 5 to HIGH
			OCR1A += servo1_angle; // sets the next interrupt to be the end of servo 1 pulse width
			cycle_stage = 1;
		break;
		//turn off servo1 and wait until the end of the 2.5ms slot
		case 1:
			PORTD &= ~(1<<PORTD5);//pin 5 LOW
			OCR1A += (5000-servo1_angle);// gives servo 1 a 2.5ms slot
			cycle_stage = 2;
		break;
		//turn on servo 2
		case 2:
			PORTD |= (1<<PORTD6);//pin 6 HIGH
			OCR1A += servo2_angle; //sets the next interrupt to be the end of servo 2 pulse width
			cycle_stage = 3;			
		break;
		//turn off servo2 pulse and wait until end of 2.5ms slot
		case 3:
			PORTD &= ~(1<<PORTD6);//pin 6 LOW
			OCR1A += (5000-servo2_angle);// gives servo 2 a 2.5ms slot
			cycle_stage = 4;	
		break;
		//turn on servo 3
		case 4:
			PORTD |= (1<<PORTD7);//pin 7 HIGH
			OCR1A += servo3_angle; //sets the next interrupt to be the end of servo 3 pulse width
			cycle_stage = 5;
		break;		
		//turn off servo3 pulse and wait until end of 2.5ms slot
		case 5:
			PORTD &= ~(1<<PORTD7);//pin 7 LOW
			OCR1A += (5000-servo3_angle);// gives servo 3 a 2.5ms slot
			cycle_stage = 6;
		break;
		//turn on servo 4
		case 6:
			PORTB |= (1<<PORTB0);//pin 8 HIGH
			OCR1A += servo4_angle; //sets the next interrupt to be the end of servo 4 pulse width
			cycle_stage = 7;
		break;
		//turn off servo4 pulse and wait until end of 2.5ms slot
		case 7:
			PORTB &= ~(1<<PORTB0);//pin 8 LOW
			OCR1A += (5000-servo4_angle);// gives servo 4 a 2.5ms slot
			cycle_stage = 8;
		break;
		//turn on servo 5
		case 8:
			PORTB |= (1<<PORTB1);//pin 9 HIGH
			OCR1A += servo5_angle; //sets the next interrupt to be the end of servo 5 pulse width
			cycle_stage = 9;
		break;
		//turn off servo5 pulse and wait until end of 2.5ms slot
		case 9:
			PORTB &= ~(1<<PORTB1);//pin 9 LOW
			OCR1A += (5000-servo5_angle);// gives servo 5 a 2.5ms slot
			cycle_stage = 10;
		break;
		//turn on servo 6
		case 10:
			PORTB |= (1<<PORTB2);//pin 10 HIGH
			OCR1A += servo6_angle; //sets the next interrupt to be the end of servo 6 pulse width
			cycle_stage = 11;
		break;
		//turn off servo6 and wait until the end off 20ms cycle
		case 11:
			PORTB &= ~(1<<PORTB2);//pin 10 LOW
			
			//40000 ticks = 20ms, 16MHz/8 = 2MHz, 0.02/(1/2000000) = 40000;
			OCR1A += (40000-30000);//30000 is the sum of 6*5000 tick slots for the servos
			cycle_stage = 0;
			break;
	}
}

void servo1_startup(){
		
		while(servo1_angle < 5000){ //rotates to 180 degrees
			if(servo1_angle < 5000){
				ATOMIC_BLOCK(ATOMIC_RESTORESTATE){//prevents interrupt during reading 16 bit integer
					servo1_angle+=75;// =~ 3degree steps
				}
				_delay_ms(20); //allows for slower movement
				}else{//prevents overshoot
					ATOMIC_BLOCK(ATOMIC_RESTORESTATE){
						servo1_angle = 5000;
					}
			}
		}
		
		_delay_ms(1000);
		
		while(servo1_angle > 1000){ //rotates to 0 degrees
			if(servo1_angle > 1000){
				ATOMIC_BLOCK(ATOMIC_RESTORESTATE){//prevents interrupt during reading 16 bit integer
					servo1_angle-=75;// =~ 3degree steps
				}
				_delay_ms(20); //allows for slower movement
				}else{//prevents overshoot
					ATOMIC_BLOCK(ATOMIC_RESTORESTATE){
						servo1_angle = 1000;
					}
			}
		}
		
		_delay_ms(1000);
		
		while(servo1_angle < 3000){ //rotates to 180 degrees
			if(servo1_angle < 3000){
				ATOMIC_BLOCK(ATOMIC_RESTORESTATE){//prevents interrupt during reading 16 bit integer
					servo1_angle+=75;// =~ 3degree steps
				}
				_delay_ms(20); //allows for slower movement
				}else{//prevents overshoot
					ATOMIC_BLOCK(ATOMIC_RESTORESTATE){
						servo1_angle = 3000;
					}
			}
		}
		
}
void servo2_startup(){
		
		while(servo2_angle < 5000){ //rotates to 180 degrees
			if(servo2_angle < 5000){
				ATOMIC_BLOCK(ATOMIC_RESTORESTATE){//prevents interrupt during reading 16 bit integer
					servo2_angle+=75;// =~ 3degree steps
				}
				_delay_ms(20); //allows for slower movement
				}else{//prevents overshoot
				ATOMIC_BLOCK(ATOMIC_RESTORESTATE){
					servo2_angle = 5000;
				}
			}
		}
		
		_delay_ms(1000);
		
		while(servo2_angle > 1000){ //rotates to 0 degrees
			if(servo2_angle > 1000){
				ATOMIC_BLOCK(ATOMIC_RESTORESTATE){//prevents interrupt during reading 16 bit integer
					servo2_angle-=75;// =~ 3degree steps
				}
				_delay_ms(20); //allows for slower movement
				}else{//prevents overshoot
				ATOMIC_BLOCK(ATOMIC_RESTORESTATE){
					servo2_angle = 1000;
				}
			}
		}
		
		_delay_ms(1000);
		
		while(servo2_angle < 3000){ //rotates to 180 degrees
			if(servo2_angle < 3000){
				ATOMIC_BLOCK(ATOMIC_RESTORESTATE){//prevents interrupt during reading 16 bit integer
					servo2_angle+=75;// =~ 3degree steps
				}
				_delay_ms(20); //allows for slower movement
				}else{//prevents overshoot
				ATOMIC_BLOCK(ATOMIC_RESTORESTATE){
					servo2_angle = 3000;
				}
			}
		}
		
}
void servo3_startup(){
		
		while(servo3_angle < 5000){ //rotates to 180 degrees
			if(servo3_angle < 5000){
				ATOMIC_BLOCK(ATOMIC_RESTORESTATE){//prevents interrupt during reading 16 bit integer
					servo3_angle+=75;// =~ 3degree steps
				}
				_delay_ms(20); //allows for slower movement
				}else{//prevents overshoot
				ATOMIC_BLOCK(ATOMIC_RESTORESTATE){
					servo3_angle = 5000;
				}
			}
		}
		
		_delay_ms(1000);
		
		while(servo3_angle > 1000){ //rotates to 0 degrees
			if(servo3_angle > 1000){
				ATOMIC_BLOCK(ATOMIC_RESTORESTATE){//prevents interrupt during reading 16 bit integer
					servo3_angle-=75;// =~ 3degree steps
				}
				_delay_ms(20); //allows for slower movement
				}else{//prevents overshoot
				ATOMIC_BLOCK(ATOMIC_RESTORESTATE){
					servo3_angle = 1000;
				}
			}
		}
		
		_delay_ms(1000);
		
		while(servo3_angle < 3000){ //rotates to 180 degrees
			if(servo3_angle < 3000){
				ATOMIC_BLOCK(ATOMIC_RESTORESTATE){//prevents interrupt during reading 16 bit integer
					servo3_angle+=75;// =~ 3degree steps
				}
				_delay_ms(20); //allows for slower movement
				}else{//prevents overshoot
				ATOMIC_BLOCK(ATOMIC_RESTORESTATE){
					servo3_angle = 3000;
				}
			}
		}
		
}
void servo4_startup(){
		
		while(servo4_angle < 5000){ //rotates to 180 degrees
			if(servo4_angle < 5000){
				ATOMIC_BLOCK(ATOMIC_RESTORESTATE){//prevents interrupt during reading 16 bit integer
					servo4_angle+=75;// =~ 3degree steps
				}
				_delay_ms(20); //allows for slower movement
				}else{//prevents overshoot
				ATOMIC_BLOCK(ATOMIC_RESTORESTATE){
					servo4_angle = 5000;
				}
			}
		}
		
		_delay_ms(1000);
		
		while(servo4_angle > 1000){ //rotates to 0 degrees
			if(servo4_angle > 1000){
				ATOMIC_BLOCK(ATOMIC_RESTORESTATE){//prevents interrupt during reading 16 bit integer
					servo4_angle-=75;// =~ 3degree steps
				}
				_delay_ms(20); //allows for slower movement
				}else{//prevents overshoot
				ATOMIC_BLOCK(ATOMIC_RESTORESTATE){
					servo4_angle = 1000;
				}
			}
		}
		
		_delay_ms(1000);
		
		while(servo4_angle < 3000){ //rotates to 180 degrees
			if(servo4_angle < 3000){
				ATOMIC_BLOCK(ATOMIC_RESTORESTATE){//prevents interrupt during reading 16 bit integer
					servo4_angle+=75;// =~ 3degree steps
				}
				_delay_ms(20); //allows for slower movement
				}else{//prevents overshoot
				ATOMIC_BLOCK(ATOMIC_RESTORESTATE){
					servo4_angle = 3000;
				}
			}
		}
		
}
void servo5_startup(){
		
		while(servo5_angle < 5000){ //rotates to 180 degrees
			if(servo5_angle < 5000){
				ATOMIC_BLOCK(ATOMIC_RESTORESTATE){//prevents interrupt during reading 16 bit integer
					servo5_angle+=75;// =~ 3degree steps
				}
				_delay_ms(20); //allows for slower movement
				}else{//prevents overshoot
				ATOMIC_BLOCK(ATOMIC_RESTORESTATE){
					servo5_angle = 5000;
				}
			}
		}
		
		_delay_ms(1000);
		
		while(servo5_angle > 1000){ //rotates to 0 degrees
			if(servo5_angle > 1000){
				ATOMIC_BLOCK(ATOMIC_RESTORESTATE){//prevents interrupt during reading 16 bit integer
					servo5_angle-=75;// =~ 3degree steps
				}
				_delay_ms(20); //allows for slower movement
				}else{//prevents overshoot
				ATOMIC_BLOCK(ATOMIC_RESTORESTATE){
					servo5_angle = 1000;
				}
			}
		}
		
		_delay_ms(1000);
		
		while(servo5_angle < 3000){ //rotates to 180 degrees
			if(servo5_angle < 3000){
				ATOMIC_BLOCK(ATOMIC_RESTORESTATE){//prevents interrupt during reading 16 bit integer
					servo5_angle+=75;// =~ 3degree steps
				}
				_delay_ms(20); //allows for slower movement
				}else{//prevents overshoot
				ATOMIC_BLOCK(ATOMIC_RESTORESTATE){
					servo5_angle = 3000;
				}
			}
		}
		
}
void servo6_startup(){
		
		while(servo6_angle < 5000){ //rotates to 180 degrees
			if(servo6_angle < 5000){
				ATOMIC_BLOCK(ATOMIC_RESTORESTATE){//prevents interrupt during reading 16 bit integer
					servo6_angle+=75;// =~ 3degree steps
				}
				_delay_ms(20); //allows for slower movement
				}else{//prevents overshoot
				ATOMIC_BLOCK(ATOMIC_RESTORESTATE){
					servo6_angle = 5000;
				}
			}
		}
		
		_delay_ms(1000);
		
		while(servo6_angle > 1000){ //rotates to 0 degrees
			if(servo6_angle > 1000){
				ATOMIC_BLOCK(ATOMIC_RESTORESTATE){//prevents interrupt during reading 16 bit integer
					servo6_angle-=75;// =~ 3degree steps
				}
				_delay_ms(20); //allows for slower movement
				}else{//prevents overshoot
				ATOMIC_BLOCK(ATOMIC_RESTORESTATE){
					servo6_angle = 1000;
				}
			}
		}
		
		_delay_ms(1000);
		
		while(servo6_angle < 3000){ //rotates to 180 degrees
			if(servo6_angle < 3000){
				ATOMIC_BLOCK(ATOMIC_RESTORESTATE){//prevents interrupt during reading 16 bit integer
					servo6_angle+=75;// =~ 3degree steps
				}
				_delay_ms(20); //allows for slower movement
				}else{//prevents overshoot
				ATOMIC_BLOCK(ATOMIC_RESTORESTATE){
					servo6_angle = 3000;
				}
			}
		}
		
}

void simultaneous_movement(){// to test maximum current draw
		
		while((servo1_angle < 5000) || (servo2_angle < 5000) || (servo3_angle < 5000) ||(servo4_angle < 5000) ||(servo5_angle < 5000) ||(servo6_angle < 5000)  ){ //rotates all servos to 180 degrees
			if(servo1_angle < 5000){
				ATOMIC_BLOCK(ATOMIC_RESTORESTATE){//prevents interrupt during reading 16 bit integer
					servo1_angle+=75;// =~ 3degree steps
				}
			}else{//prevents overshoot
				ATOMIC_BLOCK(ATOMIC_RESTORESTATE){
					servo1_angle = 5000;
				}
			}
			if(servo2_angle < 5000){
				ATOMIC_BLOCK(ATOMIC_RESTORESTATE){//prevents interrupt during reading 16 bit integer
					servo2_angle+=75;// =~ 3degree steps
				}
				}else{//prevents overshoot
				ATOMIC_BLOCK(ATOMIC_RESTORESTATE){
					servo2_angle = 5000;
				}
			}
			if(servo3_angle < 5000){
				ATOMIC_BLOCK(ATOMIC_RESTORESTATE){//prevents interrupt during reading 16 bit integer
					servo3_angle+=75;// =~ 3degree steps
				}
				}else{//prevents overshoot
				ATOMIC_BLOCK(ATOMIC_RESTORESTATE){
					servo3_angle = 5000;
				}
			}
			if(servo4_angle < 5000){
				ATOMIC_BLOCK(ATOMIC_RESTORESTATE){//prevents interrupt during reading 16 bit integer
					servo4_angle+=75;// =~ 3degree steps
				}
				}else{//prevents overshoot
				ATOMIC_BLOCK(ATOMIC_RESTORESTATE){
					servo4_angle = 5000;
				}
			}
			if(servo5_angle < 5000){
				ATOMIC_BLOCK(ATOMIC_RESTORESTATE){//prevents interrupt during reading 16 bit integer
					servo5_angle+=75;// =~ 3degree steps
				}
				}else{//prevents overshoot
				ATOMIC_BLOCK(ATOMIC_RESTORESTATE){
					servo5_angle = 5000;
				}
			}
			if(servo6_angle < 5000){
				ATOMIC_BLOCK(ATOMIC_RESTORESTATE){//prevents interrupt during reading 16 bit integer
					servo6_angle+=75;// =~ 3degree steps
				}
				}else{//prevents overshoot
				ATOMIC_BLOCK(ATOMIC_RESTORESTATE){
					servo6_angle = 5000;
				}
			}
			
			_delay_ms(20);
		}
		while((servo1_angle > 1000) || (servo2_angle > 1000) || (servo3_angle > 1000) || (servo4_angle > 1000) || (servo5_angle > 1000) || (servo6_angle > 1000)){ //rotates to 0 degrees
			if(servo1_angle > 1000){
				ATOMIC_BLOCK(ATOMIC_RESTORESTATE){//prevents interrupt during reading of 16 bit integer
					servo1_angle-=75;// =~ 3degree steps
				}
				}else{
					ATOMIC_BLOCK(ATOMIC_RESTORESTATE){
						servo1_angle = 1000;
					}
			}
			if(servo2_angle > 1000){
				ATOMIC_BLOCK(ATOMIC_RESTORESTATE){//prevents interrupt during reading of 16 bit integer
					servo2_angle-=75;// =~ 3degree steps
				}
				}else{
				ATOMIC_BLOCK(ATOMIC_RESTORESTATE){
					servo2_angle = 1000;
				}
			}
			if(servo3_angle > 1000){
				ATOMIC_BLOCK(ATOMIC_RESTORESTATE){//prevents interrupt during reading of 16 bit integer
					servo3_angle-=75;// =~ 3degree steps
				}
				}else{
				ATOMIC_BLOCK(ATOMIC_RESTORESTATE){
					servo3_angle = 1000;
				}
			}
			if(servo4_angle > 1000){
				ATOMIC_BLOCK(ATOMIC_RESTORESTATE){//prevents interrupt during reading of 16 bit integer
					servo4_angle-=75;// =~ 3degree steps
				}
				}else{
				ATOMIC_BLOCK(ATOMIC_RESTORESTATE){
					servo4_angle = 1000;
				}
			}
			if(servo5_angle > 1000){
				ATOMIC_BLOCK(ATOMIC_RESTORESTATE){//prevents interrupt during reading of 16 bit integer
					servo5_angle-=75;// =~ 3degree steps
				}
				}else{
				ATOMIC_BLOCK(ATOMIC_RESTORESTATE){
					servo5_angle = 1000;
				}
			}
			if(servo6_angle > 1000){
				ATOMIC_BLOCK(ATOMIC_RESTORESTATE){//prevents interrupt during reading of 16 bit integer
					servo6_angle-=75;// =~ 3degree steps
				}
				}else{
				ATOMIC_BLOCK(ATOMIC_RESTORESTATE){
					servo6_angle = 1000;
				}
			}
			_delay_ms(20);
		}
		while((servo1_angle < 3000) || (servo2_angle < 3000) || (servo3_angle < 3000) || (servo4_angle < 3000) || (servo5_angle < 3000) || (servo6_angle < 3000)){ //rotates to 180 degrees
			if(servo1_angle < 3000){
				ATOMIC_BLOCK(ATOMIC_RESTORESTATE){//prevents interrupt during reading 16 bit integer
					servo1_angle+=75;// =~ 3degree steps
				}
				}else{
					ATOMIC_BLOCK(ATOMIC_RESTORESTATE){
						servo1_angle = 3000;
					}
			}
			if(servo2_angle < 3000){
				ATOMIC_BLOCK(ATOMIC_RESTORESTATE){//prevents interrupt during reading 16 bit integer
					servo2_angle+=75;// =~ 3degree steps
				}
				}else{
				ATOMIC_BLOCK(ATOMIC_RESTORESTATE){
					servo2_angle = 3000;
				}
			}
			if(servo3_angle < 3000){
				ATOMIC_BLOCK(ATOMIC_RESTORESTATE){//prevents interrupt during reading 16 bit integer
					servo3_angle+=75;// =~ 3degree steps
				}
				}else{
				ATOMIC_BLOCK(ATOMIC_RESTORESTATE){
					servo3_angle = 3000;
				}
			}
			if(servo4_angle < 3000){
				ATOMIC_BLOCK(ATOMIC_RESTORESTATE){//prevents interrupt during reading 16 bit integer
					servo4_angle+=75;// =~ 3degree steps
				}
				}else{
				ATOMIC_BLOCK(ATOMIC_RESTORESTATE){
					servo4_angle = 3000;
				}
			}
			if(servo5_angle < 3000){
				ATOMIC_BLOCK(ATOMIC_RESTORESTATE){//prevents interrupt during reading 16 bit integer
					servo5_angle+=75;// =~ 3degree steps
				}
				}else{
				ATOMIC_BLOCK(ATOMIC_RESTORESTATE){
					servo5_angle = 3000;
				}
			}
			if(servo6_angle < 3000){
				ATOMIC_BLOCK(ATOMIC_RESTORESTATE){//prevents interrupt during reading 16 bit integer
					servo6_angle+=75;// =~ 3degree steps
				}
				}else{
				ATOMIC_BLOCK(ATOMIC_RESTORESTATE){
					servo6_angle = 3000;
				}
			}
			_delay_ms(20);
		}
		
}

int main(void) {
	//position the robot to be in the 90 degrees rest position to prevent startup jerk
	setup_ports();
	setup_pwm();

	servo1_startup();
		_delay_ms(1000);
	servo2_startup();
		_delay_ms(1000);
	servo3_startup();
		_delay_ms(1000);
	servo4_startup();
		_delay_ms(1000);
	servo5_startup();
		_delay_ms(1000);
	servo6_startup();
		_delay_ms(1000);

	simultaneous_movement();

	while(1){
		
	}
}

