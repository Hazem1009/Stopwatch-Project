/*
 * main.c
 *
 *  Created on: Sep 18, 2021
 *      Author: Hazem
 */
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
unsigned char Seconds=0;
unsigned char Minutes=0;
unsigned char Hours=0;
//Initializations
void Timer1Init(){
	TCNT1=0; // Intial value for the counter
	TCCR1A|=(1<<FOC1A); // Non PWM mode
	// Operate on CTC mode with prescaler N= 64 1=10^6/64(1+Top)
	//We get top value equals 15624
	TCCR1B|=(1<<WGM12) |(1<<CS10)|(1<<CS11);
	//Top value
	OCR1A=15624;
	//Enable module Interrupt
	TIMSK|=(1<<OCIE1A);
}
void Int0Init(){
	DDRD&=~(1<<PD2); // output
	MCUCR|=(1<<ISC01); // falling edge
	GICR|=(1<<INT0); // enable interrupt 0
	PORTD|=(1<<PD2); // Turn on internal pull up
}
void Int1Init(){
	DDRD&=~(1<<PD3); //output
	MCUCR|=(1<<ISC11)|(1<<ISC10); //rising edge
	GICR|=(1<<INT1); //enable interrupt 1
}
void Int2Init(){
	DDRB&=~(1<<PB2); // output
	PORTB|=(1<<PB2); // enable internal pull up
	GICR|=(1<<INT2); // enable interrupt 2
	// falling edge with a 0 intial value in ISC2
	//MCUCSR&=~(1<<ISC2);
}

//ISRs
ISR(TIMER1_COMPA_vect){
	Seconds++;
	if(Seconds==60){
		Minutes++;
		Seconds=0;
	}
	if(Minutes==60){
		Hours++;
		Minutes=0;
	}
	if(Hours==24){
		Hours=0;
	}

}
//Interrupt 0 ISR
ISR (INT0_vect){
	//Reset all values
	Seconds=0;
	Minutes=0;
	Hours=0;
}
// interrupt 1 ISR
ISR (INT1_vect){
	//Turn off Timer clock
	TCCR1B &=~(1<<CS10)&~(1<<CS11)&~(1<<CS12);
}
ISR(INT2_vect){
	TCCR1B|=(1<<CS10)|(1<<CS11);
}
int main(){
	Timer1Init();
	Int0Init();
	Int1Init();
	Int2Init();
	SREG|=(1<<7); //enable global interrupts
	DDRC|=0x0F; //PC0-3 output for 7 segments
	DDRA|=0x3F; //PA0-5 output for 7 segments enable
	PORTC=0x00;


	while(1){
		//Using % and / enabled me to use 3 variables instead of 6, where 1 for each 7 segments
		//Display seconds first by enabling 5th then 4th 7 segments
		PORTA=(1<<5);
		PORTC=Seconds%10; // take units of the number
		_delay_ms(3);

		PORTA=(1<<4);
		PORTC=Seconds/10 ; //take tenth of the number
		_delay_ms(3);

		PORTA=(1<<3);
		PORTC=Minutes%10 ; //take units of the number
		_delay_ms(3);

		PORTA=(1<<2);
		PORTC=Minutes/10 ; //take tenth of the number
		_delay_ms(3);

		PORTA=(1<<1);
		PORTC=Hours%10 ; //take units of the number
		_delay_ms(3);

		PORTA=(1<<0);
		PORTC=Hours/10 ; //take tenth of the number
		_delay_ms(3);

	}


}
