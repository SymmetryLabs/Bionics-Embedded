// Uncomment this line if you have any interrupts that are changing pins - this causes the library to be a little bit more cautious
// #define FAST_SPI_INTERRUPTS_WRITE_PINS 1

// Uncomment this line to force always using software, instead of hardware, SPI (why?)
// #define FORCE_SOFTWARE_SPI 1

// Uncomment this line if you want to talk to DMX controllers
// #define FASTSPI_USE_DMX_SIMPLE 1

#include "FastLED.h"

#define NUM_LEDS 24

CRGB leds[NUM_LEDS];



void LEDsetup() {

   	// For safety (to prevent too high of a power draw), the test case defaults to
   	// setting brightness to 25% brightness
   	LEDS.setBrightness(64);

	// Put ws2801 strip on the hardware SPI pins with a BGR ordering of rgb and limited to a 1Mhz data rate
	LEDS.addLeds<WS2801, 11, 13, BGR, DATA_RATE_MHZ(1)>(leds, NUM_LEDS);

	// Create animation object
	// Animation train = new Train();
	Train train = new Train();
}

uint8_t x = 0;

void LEDrun() { 

/*
	// RUN BEHAVIORS
	for(int b = 0; b < NUM_BEHAVIORS; b++) {
		
	}

	// MAP TO LEDS
	for(int i = 0; i < NUM_LEDS; i++) { 
		// Set individual LED HSV here
		leds[i] = 
	}

	// POST FRAME
	LEDS.show();
*/

	
	
}



// run behaviors

/*
void mapToLEDS() {
	for(int i = 0; i < NUM_LEDS; i++) { 
		// Set individual LED HSV here
		leds[i] = 
	}
}
*/


void postFrame() {
	LEDS.show();
}