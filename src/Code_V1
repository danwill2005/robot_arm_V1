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




void setup_software_pwm(){
	//Set pin 11 as output for servo 3
	DDRB |= (1 << DDB3);
	
	//Set timer 2 to CTC mode
	TCCR2A = (1 << WGM21);
	
	//Set the TOP value for 50 microseconds
	// (16MHz / 8 prescaler / 20,000Hz) = 100
	OCR2A = 99;
	
	//Enable the Interrupt
	TIMSK2 |= (1 << OCIE2A);
	
	//Set Prescaler to 8 and start
	TCCR2B |= (1 << CS21);
	
	sei(); // Enable Global Interrupts
}

volatile uint16_t soft_counter  = 0; //goes from 0 to 2000 (20ms)
volatile uint16_t servo_value3 = 30; //1.5ms 1500/10us

ISR(TIMER2_COMPA_vect) {
	soft_counter++;

	// Start of the 20ms cycle (2000 * 10us = 20,000us = 20ms)
	if (soft_counter >= 400) {
		soft_counter = 0;
		PORTB |= (1 << PORTB3); // Pin 11 HIGH
	}

	// End of the specific servo pulse
	if (soft_counter >= servo_value3) {
		PORTB &= ~(1 << PORTB3); // Pin 11 LOW
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
	while(servo_value3 < 250){
		servo_value3 += 5;
		_delay_ms(20);
	}
	//0deg
	while(servo_value3 > 50){
			servo_value3 -= 5;
			_delay_ms(20);
	}
	//90deg
	while(servo_value3 < 150){
		servo_value3 += 5;
		_delay_ms(20);
	}
	servo_value3 = 150;
}

int main(void) {
	setup_servo_pwm();
	//setup_software_pwm();
	
_delay_ms(1000);
servo1_startup();
_delay_ms(1000);
servo2_startup();
_delay_ms(1000);
//servo3_startup();


	while(1){
		
	}
}

