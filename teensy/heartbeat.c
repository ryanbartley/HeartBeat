#include <avr/io.h>
#include <avr/pgmspace.h>
#define __DELAY_BACKWARD_COMPATIBLE__ 
#include <util/delay.h>

#define DEBUG 0

#include "usb_rawhid.h"

#include "print.h"

// Teensy 2.0: LED is active high
#if defined(__AVR_ATmega32U4__) || defined(__AVR_AT90USB1286__)
#define LED_ON		(PORTD |= (1<<6))
#define LED_OFF		(PORTD &= ~(1<<6))
#define FLOWER_ON	(PORTC |= (1<<7))
#define FLOWER_OFF	(PORTC &= ~(1<<7))
#endif

#define FLOWER_CONFIG	(DDRD |= (1<<6))
#define LED_CONFIG		(DDRC |= (1<<7))
#define CPU_PRESCALE(n)	(CLKPR = 0x80, CLKPR = (n))
#define DIT 80		/* unit time for morse code */

const double PROGMEM fullCycle 		= 20.000;
const double PROGMEM fullyOpen 		= 0.855;
// 1.800 a
// 1.550 c
// 1.600 b
const double PROGMEM fullyClosed 	= 1.600;
const double PROGMEM amountToMove 	= 1.600 - 0.855;


void on();
void off();
void process();
void notify( int numTimes );

uint8_t buffer[64];

double amountOn;
#if DEBUG
int currentDirection = 1;
#else
int currentDirection = 0;
#endif

int main(void)
{
	// set for 16 MHz clock, and make sure the LED is off
	CPU_PRESCALE(0);
	LED_CONFIG;
	FLOWER_CONFIG;
	LED_OFF;
	FLOWER_OFF;

	amountOn = fullyOpen;

	// initialize the USB, but don't want for the host to
	// configure.  The first several messages sent will be
	// lost because the PC hasn't configured the USB yet,
	// but we care more about blinking than debug messages!
	usb_init();

	while (! usb_configured() ) {
		LED_ON;
		_delay_ms(10);
		LED_OFF;
		_delay_ms(10);
	}

	LED_ON;

	while(1) {
		int8_t recv = usb_rawhid_recv( buffer, 0 );
		if( recv > 0 ) {
			if( buffer[0] == 0 ) {
				#if DEBUG
				notify(6);
				#endif
				currentDirection = -1;
			}
			else if( buffer[0] == 255 ) {
				#if DEBUG
				notify(2);
				#endif
				currentDirection = 1;
			}
		}
		if( currentDirection != 0 ) {
			process();
		}
		on();
		off();
	}
}

void on()
{
	FLOWER_ON;
	_delay_ms(amountOn);
}

void off()
{
	FLOWER_OFF;
	_delay_ms(fullCycle - amountOn);
}

void notify( int numTimes ) 
{
	while(numTimes--) 
	{
		LED_ON;
		_delay_ms(20);
		LED_OFF;
		_delay_ms(20);
	}
}

void process()
{
	static const double totalSteps = 200.0;
	static double totalAnimation = 1.0 / totalSteps;
	static double currentFrame = 0.0;

	currentFrame += currentDirection;

	amountOn = ((currentFrame * totalAnimation) * amountToMove) + fullyOpen;
	if( amountOn >= fullyClosed && currentDirection == 1 ) {
		#if DEBUG
		currentDirection = -1;
		#else
		currentDirection = 0;
		#endif
		amountOn = fullyClosed;
		currentFrame = totalSteps;
	}
	else if( amountOn <= fullyOpen && currentDirection == -1 ) {
		#if DEBUG
		currentDirection = 1;
		#else
		currentDirection = 0;
		#endif
		amountOn = fullyOpen;
		currentFrame = 0;
	}
}
