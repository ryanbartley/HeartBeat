//
//  Servo.c
//  
//
//  Created by Ryan Bartley on 10/9/14.
//
//

#include "Servo.h"

uint8_t attachedA = 0;
uint8_t attachedB = 0;
#ifdef SERVO_PIN_C
uint8_t attachedC = 0;
#endif

void seizeTimer1()
{
	uint8_t oldSREG = SREG;
	
	cli();
	TCCR1A = _BV(WGM11); /* Fast PWM, ICR1 is top */
	TCCR1B = _BV(WGM13) | _BV(WGM12) /* Fast PWM, ICR1 is top */
	| _BV(CS11) /* div 8 clock prescaler */
	;
	OCR1A = 3000;
	OCR1B = 3000;
	ICR1 = clockCyclesPerMicrosecond()*(20000L/8);  // 20000 uS is a bit fast for the refresh, 20ms, but
	// it keeps us from overflowing ICR1 at 20MHz clocks
	// That "/8" at the end is the prescaler.
#if defined(__AVR_ATmega8__)
	TIMSK &= ~(_BV(TICIE1) | _BV(OCIE1A) | _BV(OCIE1B) | _BV(TOIE1) );
#else
	TIMSK1 &=  ~(_BV(OCIE1A) | _BV(OCIE1B) | _BV(TOIE1) );
#endif
	
	SREG = oldSREG;  // undo cli()
}

void releaseTimer1() {}

#define NO_ANGLE (0xff)

uint8_t PWMServo::attach(Servo *servo, int pinArg)
{
	return attach(servo, pinArg, 544, 2400);
}

uint8_t PWMServo::attach(Servo *servo, int pinArg, int min, int max)
{
#ifdef SERVO_PIN_C
	if (pinArg != SERVO_PIN_A && pinArg != SERVO_PIN_B && pinArg != SERVO_PIN_C) return 0;
#else
	if (pinArg != SERVO_PIN_A && pinArg != SERVO_PIN_B) return 0;
#endif
	
	servo->min16 = min / 16;
	servo->max16 = max / 16;
	
	servo->pin = pinArg;
	servo->angle = NO_ANGLE;
	digitalWrite(pin, LOW);
	pinMode(pin, OUTPUT);
	
#ifdef SERVO_PIN_C
	if (!attachedA && !attachedB && !attachedC) seizeTimer1();
#else
	if (!attachedA && !attachedB) seizeTimer1();
#endif
	
	if (pin == SERVO_PIN_A) {
		attachedA = 1;
		TCCR1A = (TCCR1A & ~_BV(COM1A0)) | _BV(COM1A1);
	}
	
	if (pin == SERVO_PIN_B) {
		attachedB = 1;
		TCCR1A = (TCCR1A & ~_BV(COM1B0)) | _BV(COM1B1);
	}
	
#ifdef SERVO_PIN_C
	if (pin == SERVO_PIN_C) {
		attachedC = 1;
		TCCR1A = (TCCR1A & ~_BV(COM1C0)) | _BV(COM1C1);
	}
#endif
	return 1;
}