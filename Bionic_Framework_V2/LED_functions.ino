

void LEDsetup() {

   	// For safety (to prevent too high of a power draw), the test case defaults to
   	// setting brightness to 25% brightness
   	LEDS.setBrightness(255);

	// Put ws2801 strip on the hardware SPI pins with a BGR ordering of rgb and limited to a 1Mhz data rate
	// LEDS.addLeds<WS2801, 11, 13, BGR, DATA_RATE_MHZ(1)>(leds, NUM_LEDS);
	LEDS.addLeds<WS2801, 11, 13, RGB, DATA_RATE_MHZ(1)>(leds, NUM_LEDS);

	lastAnimate = millis();

}


void LEDrun() {
  
        static float magLPF;
        const float magLPFalpha = 0.8;

	static float rollLPF;
	const float rollLPFalpha = 0.960;

	static float pitchLPF;
	const float pitchLPFalpha = 0.960;

	// Calculate deltaTime since last animate (animations are time dependent)
	// This allows for non-fixed framerate running - the Teensy does its best
	unsigned long deltaMs = millis() - lastAnimate;
	SERIAL_PRINT("dT Animation "); SERIAL_PRINTLN(deltaMs);

	float magnitude = aaReal.getMagnitude();
//	float magnitudePercent = 2.50*( magnitude-1000.) / (56599.-1000.); // Map from 0-56599.9 -> MIN-MAX SparkleNumber
	float magnitudePercent = ( magnitude-1000.00 ) / (30000.00); // Map from 0-56599.9 -> MIN-MAX SparkleNumber
	if ( magnitudePercent < 0 ) magnitudePercent = 0;
        else if ( magnitudePercent > 1 ) magnitudePercent = 1;
	SERIAL_PRINT("magPercent "); SERIAL_PRINTLN(magnitudePercent);

	rollLPF = rollLPF * rollLPFalpha + ypr[2]*(1-rollLPFalpha);
	SERIAL_PRINT("roll LPF "); SERIAL_PRINTLN(rollLPF);
	float rollPercentP = rollLPF/(M_PI/4) + 0.5;

	pitchLPF = rollLPF * pitchLPFalpha + ypr[1]*(1-pitchLPFalpha);
	SERIAL_PRINT("pitch LPF "); SERIAL_PRINTLN(pitchLPF);
	float pitchPercentP = pitchLPF/(M_PI/4) + 0.5;


	animations[currentAnimation]->level_Parameter.setPercent(magnitudePercent);
//	animations[currentAnimation]->hue_Parameter.setPercent(rollPercentP);

	// Run animation-specific code
	switch ( currentAnimation ) {

		case SPARKLE:
		{
			// Trigger if percent high enough
			if ( magnitudePercent > 0.2) {
                              animations[currentAnimation]->level_Parameter.setPercent(magnitudePercent);
				sparkle.trigger(); // Sooooo ghetto
			}
			break;
		}

		case POWER:
		{
			break;
		}
/*
		case RUNNINGRAINBOW:
		{
			break;
		}

		case NOISE:
		{
			magLPF = magLPF * magLPFalpha + magnitude*(1-magLPFalpha);
			SERIAL_PRINT("mag LPF "); SERIAL_PRINTLN(magLPF);
			float magPercentP = (magnitude) / (56599.);
			animations[currentAnimation]->level_Parameter.setPercent(magPercentP);
			break;
		}
		*/
		default:
			SERIAL_PRINTLN("ERROR!!! Running an unsupported animation from LEDrun()");
	}


	animations[currentAnimation]->draw( deltaMs );

	LEDS.show();

	lastAnimate = millis();
}
