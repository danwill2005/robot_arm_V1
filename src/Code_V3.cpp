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
volatile uint8_t servo_value = 23; //1.5ms 1500/64us (1/16MHz/1024)



ISR(TIMER2_COMPA_vect) {
	//on the first interrupt this code will run
if (cycle_stage == 0) {
	//start of pulse
	PORTD |= servo_mask; // Pin HIGH
	
	// Schedule the end of pulse interrupt
	// new compare value = current count (starting 0) + desired angle ticks
	OCR2A = servo_value;
	
	//set next stage of PWM cycle
	cycle_stage = 1;
}
//on the second interrupt with the new compare value this code will run
else if (cycle_stage == 1) {
	//end of pulse
	PORTD &= ~servo_mask; // Pin LOW
	
	// Schedule the start of next cycle (20ms later)
	// 20ms / 64us = 312 ticks.
	// already used 31 ticks, wait 281 more.
	// Timer 2 only goes to 255, handle the overflow here:
	OCR2A = 150; // Wait about halfway through the 20ms
	cycle_stage = 2;
}
//this runs on the third interrupt
else {
	// reset for next pulse cycle
	OCR2A = (162-servo_value);
	cycle_stage = 0;
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
	while(servo_value < 39){
		servo_value += 1;
		_delay_ms(50);
	}
	//0deg
	while(servo_value > 10){
			servo_value -= 1;
			_delay_ms(50);
	}
	//90deg
	while(servo_value < 23){
		servo_value += 1;
		_delay_ms(50);
	}
	servo_value = 23;
}


int main(void) {
	//setup_servo_pwm();
	setup_software_pwm();
	
_delay_ms(1000);
//servo1_startup();
//_delay_ms(1000);
//servo2_startup();
//_delay_ms(1000);
servo3_startup();
//servo4_startup();
//servo5_startup();

/*
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
*/
	while(1){
		
	}
}

