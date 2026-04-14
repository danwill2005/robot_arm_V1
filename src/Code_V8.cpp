/*
 * robot_arm.cpp
 *
 * Created: 09/04/2026
 * Author : Daniel Williams
 */ 


#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <util/atomic.h>
#include <stdlib.h>

//these will be the tick values that equate to the ms value of the pulse width that determines a specific angle, servos have range 0.5-2.5ms --> 1000 - 4500 ticks
volatile uint16_t servo1_angle = 3000;
volatile uint16_t servo2_angle = 1000;
volatile uint16_t servo3_angle = 4500;
volatile uint16_t servo4_angle = 3000;
volatile uint16_t servo5_angle = 3000;
volatile uint16_t servo6_angle = 3000;

void uart_init() {
    UBRR0H = 0;
    UBRR0L = 103; // 9600 baud for 16MHz clock
    UCSR0B = (1 << RXEN0); // Enable Receiver
    UCSR0C = (1 << UCSZ01) | (1 << UCSZ00); // 8-bit data, 1 stop bit
}

char uart_receive() {
    while (!(UCSR0A & (1 << RXC0))); // Wait for data
    return UDR0;
}

void process_serial() {
    if (UCSR0A & (1 << RXC0)) { // Is there a character waiting?
        char c = UDR0;
        
        if (c == '<') { // Start of packet
            char buffer[10];
            uint8_t i = 0;
            
            // Read until end of packet or buffer full
            while (i < 9) {
                char next = uart_receive();
                if (next == '>') break;
                buffer[i++] = next;
            }
            buffer[i] = '\0'; // Null terminator to make it a string

            // buffer now looks like "S1:3450"
            uint16_t new_angle = atoi(&buffer[3]); // Convert "3450" to integer
			//printf("%d", new_angle);
            // Update the correct servo
			// Inside process_serial() after atoi...
			ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
				switch(buffer[1]) {
					case '1': servo1_angle = new_angle; break;
					case '2': servo2_angle = new_angle; break;
					case '3': servo3_angle = new_angle; break;
					case '4': servo4_angle = new_angle; break;
					case '5': servo5_angle = new_angle; break;
					case '6': servo6_angle = new_angle; break;
				}
			}
        }
    }
}

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
	DDRD |= (1<<DDD5) | (1<<DDD6) | (1<<DDD4);
	
	//sets pins 8,9,9 to output
	DDRB |= (1<<DDB0) | (1<<DDB1) | (1<<DDB2);
	
}

//cycle stage for the switch case, allows for a series of pulses within the 20ms window
volatile uint8_t cycle_stage = 0;



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
			OCR1A += (4500-servo1_angle);// gives servo 1 a 2.5ms slot
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
			OCR1A += (4500-servo2_angle);// gives servo 2 a 2.5ms slot
			cycle_stage = 4;	
		break;
		//turn on servo 3
		case 4:
			PORTD |= (1<<PORTD4);//pin 7 HIGH
			OCR1A += servo3_angle; //sets the next interrupt to be the end of servo 3 pulse width
			cycle_stage = 5;
		break;		
		//turn off servo3 pulse and wait until end of 2.5ms slot
		case 5:
			PORTD &= ~(1<<PORTD4);//pin 7 LOW
			OCR1A += (4500-servo3_angle);// gives servo 3 a 2.5ms slot
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
			OCR1A += (4500-servo4_angle);// gives servo 4 a 2.5ms slot
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
			OCR1A += (4500-servo5_angle);// gives servo 5 a 2.5ms slot
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
			OCR1A += (40000-30000);//30000 is the sum of 6*4500 tick slots for the servos
			cycle_stage = 0;
			break;
	}
}

void sweep_to_180_degrees(volatile uint16_t *servo_x_angle){
	uint16_t current_value;
	
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE){
		current_value = *servo_x_angle;//prevents interrupt during reading of the while loops in the function
	}
	
	while(current_value < 5000){ //rotates to 180 degrees
			ATOMIC_BLOCK(ATOMIC_RESTORESTATE){//prevents interrupt during reading 16 bit integer
				*servo_x_angle+=75;// =~ 3degree steps
				current_value = *servo_x_angle;
			}
			_delay_ms(20); //allows for slower movement
	}	
	
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE){
		*servo_x_angle = 5000;
	}
}

void sweep_to_0_degrees(volatile uint16_t *servo_x_angle){
	
	uint16_t current_value;
	
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE){
		current_value = *servo_x_angle;//prevents interrupt during reading of the while loops in the function
	}
	
	while(current_value > 1000){ //rotates to 0 degrees
			ATOMIC_BLOCK(ATOMIC_RESTORESTATE){//prevents interrupt during reading 16 bit integer
				*servo_x_angle-=75;// =~ 3degree steps
				current_value = *servo_x_angle;
			}
			_delay_ms(20); //allows for slower movement
	}
	
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE){
		*servo_x_angle = 1000;
	}
}

void sweep_to_90_degrees(volatile uint16_t *servo_x_angle){
	
	uint16_t current_value;
	
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE){
		current_value = *servo_x_angle;//prevents interrupt during reading of the while loops in the function
	}
	
	while((current_value > 3075) || (current_value < 2925)){ //rotates to 180 degrees
		if(current_value < 2925){
			ATOMIC_BLOCK(ATOMIC_RESTORESTATE){//prevents interrupt during reading 16 bit integer
				*servo_x_angle+=75;// =~ 3degree steps
				current_value = *servo_x_angle;
			}
			_delay_ms(20); //allows for slower movement
		}else if(current_value > 3075){
			ATOMIC_BLOCK(ATOMIC_RESTORESTATE){//prevents interrupt during reading 16 bit integer
				*servo_x_angle-=75;// =~ 3degree steps
				current_value = *servo_x_angle;
			}		
			_delay_ms(20);	
		}
	}
	
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE){
		*servo_x_angle = 3000;
	}

}

void servo_x_startup(volatile uint16_t *servo_x_angle){
	//startup function for each servo
	//goes to 180 deg, then to 0, then to 90
	
	sweep_to_180_degrees(servo_x_angle);
			
	_delay_ms(1000);
			
	sweep_to_0_degrees(servo_x_angle);
			
	_delay_ms(1000);
			
	sweep_to_90_degrees(servo_x_angle);
	
}

uint8_t move_by_step_until_target(volatile uint16_t *servo_x_angle, uint16_t target){
	
	uint16_t current_value;
	
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE){
		current_value = *servo_x_angle;
	}
	
	if(current_value < (target - 75)){
		ATOMIC_BLOCK(ATOMIC_RESTORESTATE){
			*servo_x_angle += 75;
		}
		return 1;
	}
	else if (current_value > (target + 75)){
		ATOMIC_BLOCK(ATOMIC_RESTORESTATE){
			*servo_x_angle -= 75;
		}
		return 1;
	}
	else{
		ATOMIC_BLOCK(ATOMIC_RESTORESTATE){
			*servo_x_angle = target;
		}
		return 0;
	}
	
}

void simultaneous_movement(){// to test maximum current draw
	// move all servos to 180 degrees
	while (move_by_step_until_target(&servo1_angle, 5000) | move_by_step_until_target(&servo2_angle, 5000) | move_by_step_until_target(&servo3_angle, 5000) | move_by_step_until_target(&servo4_angle, 5000) | move_by_step_until_target(&servo5_angle, 5000) | move_by_step_until_target(&servo6_angle, 5000))
	{
		_delay_ms(20);
	}

	_delay_ms(1000);

	// move all servos to 0 degrees
	while (move_by_step_until_target(&servo1_angle, 1000) | move_by_step_until_target(&servo2_angle, 1000) | move_by_step_until_target(&servo3_angle, 1000) | move_by_step_until_target(&servo4_angle, 1000) | move_by_step_until_target(&servo5_angle, 1000) | move_by_step_until_target(&servo6_angle, 1000))
	{
		_delay_ms(20);
	}	
	
	//move all servos to 90 degrees
	while (move_by_step_until_target(&servo1_angle, 3000) | move_by_step_until_target(&servo2_angle, 3000) | move_by_step_until_target(&servo3_angle, 3000) | move_by_step_until_target(&servo4_angle, 3000) | move_by_step_until_target(&servo5_angle, 3000) | move_by_step_until_target(&servo6_angle, 3000))
	{
		_delay_ms(20);
	}	
}

int main(void) {
	//position the robot to be in the 90 degrees rest position to prevent startup jerk
	setup_ports();
	uart_init();
	setup_pwm();

	servo_x_startup(&servo1_angle);
		_delay_ms(1000);
	servo_x_startup(&servo2_angle);
		_delay_ms(1000);
	servo_x_startup(&servo3_angle);
		_delay_ms(1000);
	servo_x_startup(&servo4_angle);
		_delay_ms(1000);
	servo_x_startup(&servo5_angle);
		_delay_ms(1000);
	servo_x_startup(&servo6_angle);
		_delay_ms(1000);

	simultaneous_movement();

	while(1) {
		// If the buffer is overflowing with junk, clear it
		if (UCSR0A & (1 << DOR0)) {
			uint8_t dummy = UDR0; 
		}
		process_serial();
	}
}
