/*
 * stopwatch00.c
 *
 *  Created on: Sep 19, 2022
 *      Author: Mahmoud Qotp
 */


#define MAX_SEC_LOW 9
#define MAX_SEC_HIGH 5
#define MAX_MIN_LOW 9
#define MAX_MIN_HIGH 5
#define MAX_HOURS_LOW 9
#define MAX_HOURS_HIGH 9

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

unsigned char g_secL = 0, g_secH = 0, g_minL = 0, g_minH = 0, g_hrL = 0, g_hrH = 0;
ISR(TIMER1_COMPA_vect)
{
	if (g_secL == MAX_SEC_LOW)
	{
		if(g_secH == MAX_SEC_HIGH)
		{
			if(g_minL == MAX_MIN_LOW)
			{
				if(g_minH == MAX_MIN_HIGH)
				{
					if(g_hrL == MAX_HOURS_LOW)
					{
						if(g_hrH == MAX_HOURS_HIGH)
						{
							//Overflow Occurs
							PORTC ^= (1<<7); //Toggle the small led in the display.
							g_secL = 0;
							g_secH = 0;
							g_minL = 0;
							g_minH = 0;
							g_hrL = 0;
							g_hrH = 0;

						}
						else
						{
							g_hrH++;
						}



						g_hrL = 0;
					}

					else
					{
						g_hrL++;
					}
					g_minH = 0;
				}
				else
				{
					g_minH++;
				}
				g_minL = 0;
			}
			else
			{
				g_minL++;
			}
			g_secH = 0;
		}
		else
		{
			g_secH++;
		}
		g_secL = 0;
	}
	else
	{
		g_secL++;
	}
}

//	If a falling edge detected the Stop Watch time should be reset.
ISR(INT0_vect)
{
	g_secL = 0;
	g_secH = 0;
	g_minL = 0;
	g_minH = 0;
	g_hrL = 0;
	g_hrH = 0;
}

//	If a raising edge detected the Stop Watch time should be paused.
ISR(INT1_vect)
{
	TIMSK &= ~(1<<OCIE1A);
}

//	If a falling edge detected the Stop Watch time should be resumed.
ISR(INT2_vect)
{
	TIMSK |= (1<<OCIE1A);
}

void INT0_Init(void)
{
	MCUCR|= (1<<ISC01);
	MCUCR  &= ~(1<<ISC00); //Falling edge
	GICR |= (1<<INT0); //Interrupt Enable
	DDRD &= ~(1<<PD2); //Configure PD2 as an INPUT pin
	PORTD |= (1<<PD2); //Internal Pull Up Resistor
}

void INT1_Init(void)
{
	MCUCR|= (1<<ISC10) | (1<<ISC11); //Rising edge
	GICR |= (1<<INT1); //Interrupt Enable
	DDRD &= ~(1<<PD3); //Configure PD3 as an INPUT pin
}

void INT2_Init(void)
{
	MCUCSR &= ~(1<<ISC2); //Falling Edge
	GICR |= (1<<INT2); //Interrupt Enable
	DDRB &= ~(1<<PB2);	//Configure PB2 as an INPUT pin
	PORTB |= (1<<PB2); //Internal Pull Up Resistor
}

void TIMER1_Init(void)
{
	TCCR1A = (1<<FOC1A); //Using non PWM mode
	TCCR1B = (1<<WGM12) | (1<<CS10) | (1<<CS11); //CTC Mode // Prescaler 64
	OCR1A = 15624; //Maximum Value of the timer //15625 Cycles 15624
	TIMSK |= (1<<OCIE1A); //Timer Interrupt Enable
}

int main(void)
{
	DDRC |= 0x8f; //  Configure the first 4-pins in PORTC as OUTPUT pins to be Connected to 7447 decoder.
	PORTC &= ~(0x8f); //Initially Zero.
	DDRA |= 0x3f; // Use first 6-pins in PORTA as the enable/disable pins for the six 7-segments.
	DDRA &= ~(0x3f); //Initially All the six 7 segments are disabled.
	TIMER1_Init();
	INT0_Init();
	INT1_Init();
	INT2_Init();
	SREG |= (1<<7); //GIE bit

	while(1)
	{

		PORTA = (PORTA & 0xc0) | (1<<0); //Enable SecL
		PORTC = (PORTC & 0xf0) | (0x0f & g_secL);
		_delay_us(10); //To make it possible to human eye to capture the change.
		PORTA = (PORTA & 0xc0) | (1<<1); //Enable SecH
		PORTC = (PORTC & 0xf0) | (0x0f & g_secH);
		_delay_us(10);
		PORTA = (PORTA & 0xc0) | (1<<2); //Enable minL
		PORTC = (PORTC & 0xf0) | (0x0f & g_minL);
		_delay_us(10);
		PORTA = (PORTA & 0xc0) | (1<<3); //Enable minH
		PORTC = (PORTC & 0xf0) | (0x0f & g_minH);
		_delay_us(10);
		PORTA = (PORTA & 0xc0) | (1<<4); //Enable hrL
		PORTC = (PORTC & 0xf0) | (0x0f & g_hrL);
		_delay_us(10);
		PORTA = (PORTA & 0xc0) | (1<<5); //Enable hrH
		PORTC = (PORTC & 0xf0) | (0x0f & g_hrH);
		_delay_us(10);
	}
}

