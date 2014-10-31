

void LEDsetup() {

   	// For safety (to prevent too high of a power draw), the test case defaults to
   	// setting brightness to 25% brightness
   	LEDS.setBrightness(255);

	// Put ws2801 strip on the hardware SPI pins with a BGR ordering of rgb and limited to a 1Mhz data rate
	// LEDS.addLeds<WS2801, 11, 13, BGR, DATA_RATE_MHZ(1)>(leds, NUM_LEDS);
	LEDS.addLeds<WS2801, 11, 13, RGB, DATA_RATE_MHZ(1)>(leds, NUM_LEDS);

	lastAnimate = millis();

}


uint8_t hueNow = 0;

float rollLPF = 0;
const float rollLPFalpha = 0.960;

void LEDrun() {

	// Calculate deltaTime since last animate (animations are time dependent)
	// This allows for non-fixed framerate running - the Teensy does its best
	unsigned long deltaMs = millis() - lastAnimate;
	Serial.print("dT Animation "); Serial.println(deltaMs);

	hueNow = (hueNow + 1) % 255;



	float magnitude = aaReal.getMagnitude();
	// Map from 0-56599.9 -> MIN-MAX SparkleNumber
	float magnitudePercent = 2.00* magnitude / (56599.);
	if ( magnitudePercent < 0 ) magnitudePercent = 0;
	Serial.print("magPercent "); Serial.println(magnitudePercent);

	rollLPF = rollLPF * rollLPFalpha + ypr[1]*(1-rollLPFalpha);
	Serial.print("roll LPF "); Serial.println(rollLPF);
	float rollPercentP = rollLPF/(M_PI/4) + 0.5;




/*
	switch ( currentAnimation ) {
                
		case FIRE:
		{
			fire.draw(deltaMs);
			break;
		}
                */
                /*
		case TRAIN:
		{
			train.hue_Parameter.setValue(hueNow);
			train.draw(deltaMs);
			break;
		}
                

		case SPARKLE:
		{
			// Trigger if percent high enough
			if ( magnitudePercent > 0.1) {
//				sparkle.level_Parameter.setPercent(magnitudePercent);
                                animations[currentAnimation]->level_Parameter.setPercent(magnitudePercent);
				sparkle.trigger();
			}
//			sparkle.draw(deltaMs);
			break;
		}

		case POWER:
		{
			break;
		}

		case RUNNINGRAINBOW:
		{
			break;
		}
	}
*/


	// animations[currentAnimation]->level_Parameter.setPercent( magnitudePercent );
	// animations[currentAnimation]->hue_Parameter.setPercent( rollPercentP );
    Serial.print("anim 1 decay = "); Serial.println(sparkle.decay_Parameter.getValue());
    Serial.print("anim 2 decay = "); Serial.println(power.decay_Parameter.getValue());
    Serial.print("anim 3 decay = "); Serial.println(runningrainbow.decay_Parameter.getValue());

	float currentDecay = animations[currentAnimation]->decay_Parameter.getValue();
	Serial.print("currentAnimation decay = "); Serial.println(currentDecay);

	animations[currentAnimation]->draw( deltaMs );

	LEDS.show();

	lastAnimate = millis();
}
