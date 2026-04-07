/*
 * robot_arm.cpp
 *
 * Created: 29/03/2026 23:27:12
 * Author : Lenovo
 */ 

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

void setup_servo_pwm() {
	//Set pin 9 and 10 as output
	DDRB |= (1 << DDB1) | (1 << DDB2);

	//Configure Timer 1 for Fast PWM mode (Mode 14)

	//COM1A1 clears OC1A on Compare Match (non-inverting)
	TCCR1A = (1 << COM1A1) | (1<<COM1B1) | (1 << WGM11);
	TCCR1B = (1 << WGM13) | (1 << WGM12) | (1 << CS11); // Prescaler = 8

	//ICR1 defines the TOP value (the 20ms period)
	//Formula: (F_CPU / (Prescaler * Frequency)) - 1
	//(16,000,000 / (8 * 50)) - 1 = 39,999
	ICR1 = 39999;
	
	//starting angles for servo 1 and 2
	OCR1A = 3000;
	OCR1B = 1100;
}


#define servo_mask 0xE0 /// 11100000 in hex, the mask for Port D data direction and data registers, pins 5,6,7 on uno


void setup_software_pwm(){
	//Set pin 11 as output for servo 3
	DDRD |= servo_mask;
	
	//Set timer 2 to CTC mode
	TCCR2A = (1 << WGM21);
	
	//Enable the Interrupt
	TIMSK2 |= (1 << OCIE2A);
	
	//Set Prescaler to 1024
	TCCR2B = (1 << CS22) | (1<< CS21) | (1<<CS20);
	
	OCR2A  = 31;
	
	sei(); // Enable Global Interrupts
}

volatile uint8_t cycle_stage  = 0; //keeps track of current postion in the 20ms cycle
volatile uint8_t servo_value3 = 23; //1.5ms 1500/64us (1/16MHz/1024)
volatile uint8_t servo_value4 = 23;
volatile uint8_t servo_value5 = 23;


ISR(TIMER2_COMPA_vect) {
	//on the first interrupt this code will run
	
	switch(cycle_stage){
		case 0:
			//start of pulse
			PORTD |= (1<<PORTD5); // Pin 5 HIGH
	
			// Schedule the end of pulse interrupt
			// new compare value = current count (starting 0) + desired angle ticks
			OCR2A = servo_value3;
			//set next stage of PWM cycle
			cycle_stage = 1;
		break;
//on the second interrupt with the new compare value this code will run
		case 1: 
			//end of pulse
			PORTD &= ~(1<<PORTD5); // Pin 5 LOW
			PORTD |= (1<<PORTD6); // Pin 6 HIGH
	
			// Schedule the start of next cycle (20ms later)
			// 20ms / 64us = 312 ticks.
			// already used 31 ticks, wait 281 more.
			// Timer 2 only goes to 255, handle the overflow here:
			OCR2A = servo_value4; // Wait about halfway through the 20ms
			cycle_stage = 2;
		break;
			//this runs on the third interrupt
		case 2:
			PORTD &= ~(1<<PORTD6); // Pin 6 LOW
			PORTD |= (1<<PORTD7); // Pin 7 HIGH
			OCR2A = servo_value5;
			cycle_stage = 3;
		break;
		case 3:
			PORTD &= ~(1<<PORTD7); // Pin 7 LOW
			OCR2A = servo_value5;
			cycle_stage = 4;
		break;
		case 4:			
			OCR2A = 150; // halfway through 20ms cycle
			cycle_stage = 5;
		break;
		case 5:
			OCR2A = 162-(servo_value3+servo_value4+servo_value5);//remainder of 20ms
			cycle_stage = 0;
		break;
	}
}

void servo1_startup(){
	//sweeps to 180deg
	while(OCR1A < 5000){
		OCR1A += 50;
		_delay_ms(20);
	}
	
	_delay_ms(500);
	
	//sweeps to 0 deg
	while(OCR1A > 1000){
		OCR1A -= 50;
		_delay_ms(20);
	}
	_delay_ms(500);
	//sweeps to 90deg
	while(OCR1A < 3000){
		OCR1A += 50;
		_delay_ms(20);
	}
	OCR1A =3000;
}

void servo2_startup(){
	//sweeps to 180
		while(OCR1B < 4900){
			OCR1B += 75;
			_delay_ms(20);
		}
		
		_delay_ms(500);
		//sweeps to 90
		while(OCR1B < 3000){
			OCR1B += 75;
			_delay_ms(20);
		}
		
		_delay_ms(500);		
//sweeps to 0
		while(OCR1B > 1100){
			OCR1B -= 75;
			_delay_ms(20);
		}


		OCR1B =1100;
	
}

void servo3_startup(){
	//sweeps to 180
	while(servo_value3 < 39){
		servo_value3 += 1;
		_delay_ms(50);
	}
	//0deg
	while(servo_value3 > 10){
			servo_value3 -= 1;
			_delay_ms(50);
	}
	//90deg
	while(servo_value3 < 23){
		servo_value3 += 1;
		_delay_ms(50);
	}
	servo_value3 = 23;
}

void servo4_startup(){
	//sweeps to 180
	while(servo_value4 < 39){
		servo_value4 += 1;
		_delay_ms(50);
	}
	//0deg
	while(servo_value4 > 10){
		servo_value4 -= 1;
		_delay_ms(50);
	}
	//90deg
	while(servo_value4 < 23){
		servo_value4 += 1;
		_delay_ms(50);
	}
	servo_value4 = 23;
}

void servo5_startup(){
	//sweeps to 180

	//0deg
	while(servo_value5 > 8){
		servo_value5 -= 1;
		_delay_ms(50);
	}
	//90deg
	while(servo_value5 < 22){
		servo_value5 += 1;
		_delay_ms(50);
	}
	servo_value5 = 22;
}


int main(void) {
	setup_servo_pwm();
	setup_software_pwm();
	
_delay_ms(1000);
servo1_startup();
_delay_ms(1000);
servo2_startup();
_delay_ms(1000);
servo3_startup();
_delay_ms(1000);
servo4_startup();
_delay_ms(1000);
servo5_startup();


//both servos move at the same time
while ((OCR1A != 5000) || (OCR1B != 3000)){
	//move to target angle
	if (OCR1A < 5000){
		OCR1A += 75;		
	}//prevent overshoot due to ||
	if(OCR1A >= 5000){
		OCR1A = 5000;
	}
	if (OCR1B < 3000){
		OCR1B += 75;
	}
	if(OCR1B >= 3000){
		OCR1B = 3000;
	}
	_delay_ms(20);
}
_delay_ms(1000);

//return to inital position to rpevent snap on next upload
while ((OCR1A > 3000) || (OCR1B > 1100)){
	OCR1A -= 75;
	OCR1B -= 75;
	_delay_ms(20);
}

OCR1A = 3000;
OCR1B = 1100;

//3 servos move at the same time
while ((servo_value3 != 8) || (servo_value4 != 8) || (servo_value5 != 8)){
	//move to target angle
	if (servo_value3 > 8){
		servo_value3 -= 1;
	}//prevent overshoot due to ||
	if(servo_value3 <= 8){
		servo_value3 = 8;
	}
	if (servo_value4 > 8){
		servo_value4 -= 1;
	}
	if(servo_value4 <= 8){
		servo_value4 = 8;
	}
	if (servo_value5 > 8){
		servo_value5 -= 1;
	}
	if(servo_value5 <= 8){
		servo_value5 = 8;
	}
	_delay_ms(50);
}

//return to inital position to rpevent snap on next upload
while ((servo_value3 < 22) || (servo_value4 < 22) || (servo_value5 < 22)){
	servo_value3 += 1;
	servo_value4 += 1;
	servo_value5 += 1;
	_delay_ms(50);
}

servo_value3 = 23;
servo_value4 = 23;
servo_value5 = 22;

//claw opens and closes to pick up an object
/*
while(servo_value5 > 8){
	servo_value5 -= 1;
	_delay_ms(50);
}
_delay_ms(2000);
while(servo_value5 <= 22){
	servo_value5 += 1;
	_delay_ms(50);
}
*/
	while(1){
		
	}
}

