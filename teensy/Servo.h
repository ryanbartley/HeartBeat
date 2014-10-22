//
//  Servo.h
//  
//
//  Created by Ryan Bartley on 10/9/14.
//
//

#ifndef ____Servo__
#define ____Servo__

#include <stdio.h>
#include <inttypes.h>

#if defined(__AVR_AT90USB646__) || defined(__AVR_AT90USB1286__) // Teensy++
#define SERVO_PIN_A 25
#define SERVO_PIN_B 26
#define SERVO_PIN_C 27
#endif

static void seizeTimer1();
static void releaseTimer1();

static uint8_t attachedA;
static uint8_t attachedB;
#ifdef SERVO_PIN_C
static uint8_t attachedC;
#endif

struct Servo {
	uint8_t pin;
	uint8_t angle;       // in degrees
	uint8_t min16;       // minimum pulse, 16uS units  (default is 34)
	uint8_t max16;       // maximum pulse, 16uS units, 0-4ms range (default is 150)
};

uint8_t attach( Servo* servo, int );
// pulse length for 0 degrees in microseconds, 544uS default
// pulse length for 180 degrees in microseconds, 2400uS default
uint8_t attach( Servo* servo, int, int, int);
// attach to a pin, sets pinMode, returns 0 on failure, won't
// position the servo until a subsequent write() happens
// Only works for 9 and 10.
void detach( Servo *servo );
void write( Servo *servo, int);         // specify the angle in degrees, 0 to 180
uint8_t read( Servo *servo );
uint8_t attached( Servo *servo );

#endif /* defined(____Servo__) */
