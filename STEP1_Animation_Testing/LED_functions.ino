unsigned long lastAnimate = 0;

Train train;
Fire fire;

enum AnimationState {
	FIRE,
	TRAIN
};

AnimationState currentAnimation = TRAIN;



void LEDsetup() {

   	// For safety (to prevent too high of a power draw), the test case defaults to
   	// setting brightness to 25% brightness
   	LEDS.setBrightness(200);

	// Put ws2801 strip on the hardware SPI pins with a BGR ordering of rgb and limited to a 1Mhz data rate
	// LEDS.addLeds<WS2801, 11, 13, BGR, DATA_RATE_MHZ(1)>(leds, NUM_LEDS);
	LEDS.addLeds<WS2801, 11, 13, RGB, DATA_RATE_MHZ(1)>(leds, NUM_LEDS);

	lastAnimate = millis();

}


uint8_t hueNow = 0;

void LEDrun() {

	// Calculate deltaTime since last animate (animations are time dependent)
	// This allows for non-fixed framerate running - the Teensy does its best
	unsigned long deltaMs = millis() - lastAnimate;


	hueNow = (hueNow + 1) % 255;
	switch ( currentAnimation ) {

		case FIRE:
			fire.draw(deltaMs);
			break;

		case TRAIN:
			train.hue_Parameter.setValue(hueNow);
			train.draw(deltaMs);
			break;
	}

	LEDS.show();

	lastAnimate = millis();
}