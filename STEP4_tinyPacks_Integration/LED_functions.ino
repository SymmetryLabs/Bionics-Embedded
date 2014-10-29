

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
	switch ( currentAnimation ) {

		case FIRE:
		{
			fire.draw(deltaMs);
			break;
		}

		case TRAIN:
		{
			train.hue_Parameter.setValue(hueNow);
			train.draw(deltaMs);
			break;
		}

		case SPARKLE:
		{
			float magnitude = aaReal.getMagnitude();
			rollLPF = rollLPF * rollLPFalpha + ypr[2]*(1-rollLPFalpha);
			Serial.print("roll LPF "); Serial.println(rollLPF);

			float rollPercent = rollLPF/(M_PI/4) + 0.5;
			sparkle.hue_Parameter.setPercent( rollPercent );
			if ( magnitude > 8000) {
				// Map from 0-56599.9 -> MIN-MAX SparkleNumber
				float magnitudePercent = 1.2 * magnitude / 56599.;
				sparkle.sparkleNumber_Parameter.setPercent(magnitudePercent);
				sparkle.trigger();
			}
			sparkle.draw(deltaMs);
			break;
		}

		case POWER:
		{
			float magnitudeP = aaReal.getMagnitude();
			rollLPF = rollLPF * rollLPFalpha + ypr[1]*(1-rollLPFalpha);
			Serial.print("roll LPF "); Serial.println(rollLPF);

			float rollPercentP = rollLPF/(M_PI/4) + 0.5;
//			power.hue_Parameter.setPercent( rollPercentP );

			// Map from 0-56599.9 -> MIN-MAX SparkleNumber
			float magnitudePercent = 2.5*magnitudeP / (56599.);
			if ( magnitudePercent < 0 ) magnitudePercent = 0;
			power.level_Parameter.setPercent(magnitudePercent);
			
			power.draw(deltaMs);
			break;
		}

		case RUNNINGRAINBOW:
		{
			float magnitudeP = aaReal.getMagnitude();
			rollLPF = rollLPF * rollLPFalpha + ypr[1]*(1-rollLPFalpha);
			Serial.print("roll LPF "); Serial.println(rollLPF);

			float rollPercentP = rollLPF/(M_PI/4) + 0.5;
			runningrainbow.trainSpeed_Parameter.setPercent( rollPercentP );

			// Map from 0-56599.9 -> MIN-MAX SparkleNumber
			// float magnitudePercent = 2.5*(magnitudeP-7000.) / (56599.-7000.);
			float magnitudePercent = 2.5*(magnitudeP) / (56599.);
			if ( magnitudePercent < 0 ) magnitudePercent = 0;
			runningrainbow.level_Parameter.setPercent(max( magnitudePercent, .15 ));
			
			runningrainbow.draw(deltaMs);
			break;
		}
	}

	LEDS.show();

	lastAnimate = millis();
}
