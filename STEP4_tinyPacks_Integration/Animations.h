// Had to move this to main file for proper dependencies...
// #include "BasicParameter.h"

// ================================================================
// ===                     ABSTRACT CLASS                       ===
// ================================================================

class Animation {

	public:
		// Construction
		// Animation();

		// Draw routine, to be called every run
		// Setting =0 makes it a pure virtual function
		// Responsible function for changing the leds[] array
		virtual void draw( long _deltaTime ) = 0;
};



// ================================================================
// ===                     FIRE ANIMATION                       ===
// ================================================================

class Fire : public Animation {

	public:
		// Model parameters
		// BasicParameter diffusionConstant = new BasicParameter();
		// What should ranges be for these parameters?  How do they affect time-based animations?
		BasicParameter diffusionParameter = BasicParameter("diffu", 1, 0, 1);
		BasicParameter coolingParameter = BasicParameter("cooli", 100, 20, 100);
		BasicParameter sparkingParameter = BasicParameter("spark", 50, 50, 200);

		void draw( long _deltaTime );

	private:
		// Initialize my internal model storage variables
		uint8_t heat[NUM_LEDS];

		void runModel( long _deltaTime );
		CRGB heatColor( uint8_t _temperature );

};


void Fire::draw (long _deltaTime) {
	runModel(_deltaTime);

	// Push model to LEDs
    for( int j = 0; j < NUM_LEDS; j++) {
        leds[j] = heatColor( heat[j] );
        Serial.println(heat[j]);
        Serial.println(leds[j]);
        Serial.println(leds[j]);
        Serial.println(leds[j]);
        Serial.println();
    }
}

void Fire::runModel (long _deltaTime) {

	// Run cooling
	for( int i = 0; i < NUM_LEDS; i++ ) {
      heat[i] = qsub8( heat[i],  random8(0, ((coolingParameter.getValue() * 10) / NUM_LEDS) + 2));
    }

	// Run diffusion
	for( int k = NUM_LEDS - 3; k > 0; k-- ) {
      heat[k] = diffusionParameter.getValue() * (heat[k - 1] + heat[k - 2] + heat[k - 2] ) / 3;
    }

	// Run sparking
	if( random8() < sparkingParameter.getValue() ) {
      int y = random8(7);
      heat[y] = qadd8( heat[y], random8(160,255) );
    }
}

CRGB Fire::heatColor ( uint8_t _temperature ) {
	CRGB heatcolor;
 
  // Scale 'heat' down from 0-255 to 0-191,
  // which can then be easily divided into three
  // equal 'thirds' of 64 units each.
  uint8_t t192 = scale8_video( _temperature, 192);
 
  // calculate a value that ramps up from
  // zero to 255 in each 'third' of the scale.
  uint8_t heatramp = t192 & 0x3F; // 0..63
  heatramp <<= 2; // scale up to 0..252
 
  // now figure out which third of the spectrum we're in:
  if( t192 & 0x80) {
    // we're in the hottest third
    heatcolor.r = 255; // full red
    heatcolor.g = 255; // full green
    heatcolor.b = heatramp; // ramp up blue
   
  } else if( t192 & 0x40 ) {
    // we're in the middle third
    heatcolor.r = 255; // full red
    heatcolor.g = heatramp; // ramp up green
    heatcolor.b = 0; // no blue
   
  } else {
    // we're in the coolest third
    heatcolor.r = heatramp; // ramp up red
    heatcolor.g = 0; // no green
    heatcolor.b = 0; // no blue
  }
 
  return heatcolor;
}




// ================================================================
// ===                     TRAIN ANIMATION                      ===
// ================================================================

class Train : public Animation {

	public:
		// Model parameters
		BasicParameter trainSpeed_Parameter = BasicParameter("trSpe", 300, 1, 500);
		BasicParameter trainLength_Parameter = BasicParameter("trLen", 3, 1, 5);
		BasicParameter trainPeriod_Parameter = BasicParameter("trPer", 2000, 500, 4000);
		BasicParameter hue_Parameter = BasicParameter("hue", 120, 0, 255);

		void draw( long _deltaMs );


	private:
		unsigned long lastMove = 0;
		unsigned long lastTrainArrival = 0;

		void shiftPixels();
		void newTrain();

};


void Train::draw ( long _deltaTime ) {
	// Move existing trains along
	if ( (millis()-lastMove) > trainSpeed_Parameter.getValue() ) {
		shiftPixels();
		lastMove = millis();
	}

	// Determine arrival of new trains
	if ( (millis()-lastTrainArrival) > trainPeriod_Parameter.getValue() ) {
		newTrain();
		lastTrainArrival = millis();
	}
}


void Train::shiftPixels () {
	for ( int node = NUM_LEDS-1; node > 0; node-- ) leds[node] = leds[node-1];
	leds[0] = CRGB::Black;
}

void Train::newTrain () {
	for ( int node = 0; node < int(trainLength_Parameter.getValue()); node++ ) leds[node].setHSV( hue_Parameter.getValue(), 255, 255 );
}


// ================================================================
// ===                     SPARKLE ANIMATION                    ===
// ================================================================

class Sparkle : public Animation {

	public:
		// Model parameters
		BasicParameter decay_Parameter = BasicParameter("decay", 4, 1, 255); // What's a value that's physically intuitive?
		BasicParameter sparkleNumber_Parameter = BasicParameter("spNum", 3, 1, 9);
		BasicParameter hue_Parameter = BasicParameter("hue", 120, 0, 150);

		void draw( long _deltaMs );
		void trigger();


	private:
		void newSparkles();

		long lastSpark = 0;
		long lastDimming = 0;
		long sparkPeriod = 2000;

		bool isRandomSparkingOn = false;

};


void Sparkle::draw ( long _deltaTime ) {
	// Decrement the brightness
	for( int i = 0; i < NUM_LEDS; i++ ) {
		ledsHSV[i].val = qsub8( ledsHSV[i].val, int(decay_Parameter.getValue()) );
		ledsHSV[i].hue = hue_Parameter.getValue();
	}
	// for( int i = 0; i < NUM_LEDS; i++ ) { ledsHSV[i].val-=int(decay_Parameter.getValue()); }
	// if( (millis()-lastDimming) > decay_Parameter.getValue() ) {
	// 	for( int i = 0; i < NUM_LEDS; i++ ) {
	// 		leds[i].val = leds[i].val - 1;
	// 	}
	// 	lastDimming = millis();
	// }

	// Trigger random sparkles
	if ( isRandomSparkingOn ) {
		if ( (millis()-lastSpark) > sparkPeriod ) {
			trigger();
			lastSpark = millis();
		}
	}

	// Push ledsHSV to leds
	for ( int i = 0; i < NUM_LEDS; i++ ) {
		leds[i] = ledsHSV[i];
	}
}


void Sparkle::trigger () {
	Serial.print("Trigger! Percent = ");
	Serial.println(sparkleNumber_Parameter.getPercent());
	for ( byte sparkle = 0; sparkle < sparkleNumber_Parameter.getValue(); sparkle ++) {
		byte pixelIndex = random(0, NUM_LEDS);
		uint8_t sparkBrightness = 50 + random8(0, sparkleNumber_Parameter.getPercent() * 205. );
		ledsHSV[pixelIndex].setHSV( hue_Parameter.getValue(), 255, sparkBrightness );
	}
}


// ================================================================
// ===                   POWER BAR ANIMATION                    ===
// ================================================================

class Power : public Animation {

	public:
		// Model parameters
		BasicParameter decay_Parameter = BasicParameter("decay", 10, 1, 255); // What's a value that's physically intuitive?
		BasicParameter level_Parameter = BasicParameter("level", 0, 0, NUM_LEDS);
		BasicParameter hue_Parameter = BasicParameter("hue", 120, 0, 150);

		void draw( long _deltaMs );

	private:

		long lastSpark = 0;
		long lastDimming = 0;

};


void Power::draw ( long _deltaTime ) {
	// Decrement the brightness
	for( int i = 0; i < NUM_LEDS; i++ ) {
		ledsHSV[i].val = qsub8( ledsHSV[i].val, int(decay_Parameter.getValue()) );
		ledsHSV[i].hue = hue_Parameter.getValue();
		ledsHSV[i].sat = 200;
	}

	for ( int i = 0; i < level_Parameter.getValue(); i++ ) {
		ledsHSV[i].val = 255;
	}

	// Push ledsHSV to leds
	for ( int i = 0; i < NUM_LEDS; i++ ) {
		leds[i] = ledsHSV[NUM_LEDS-1-i];
	}
}

// ================================================================
// ===              RUNNING RAINBOW ANIMATION                   ===
// ================================================================

class RunningRainbow : public Animation {

	public:
		// Model parameters
		BasicParameter decay_Parameter = BasicParameter("decay", 5, 1, 255); // What's a value that's physically intuitive?
		BasicParameter level_Parameter = BasicParameter("level", 0, 0, 255);
		BasicParameter hue_Parameter = BasicParameter("hue", 120, 0, 150);
		BasicParameter trainSpeed_Parameter = BasicParameter("trSpe", 10, 1, 100);

		void draw( long _deltaMs );

	private:

		long lastMove = 0;
		long lastDimming = 0;
		uint8_t deltaHuePixel = 10;

};


void RunningRainbow::draw ( long _deltaTime ) {
	if ( (millis()-lastMove) > trainSpeed_Parameter.getValue() ) {
		hue_Parameter.setValue( hue_Parameter.getValue() + 5. );
		lastMove = millis();
	}

	// Decrement the brightness
	for( int i = 0; i < NUM_LEDS; i++ ) {
		ledsHSV[i].val = max( qsub8( ledsHSV[i].val, int(decay_Parameter.getValue()) ), level_Parameter.getValue() );
		ledsHSV[i].hue = hue_Parameter.getValue() + i*deltaHuePixel;
		ledsHSV[i].sat = 200;
	}

	// Push ledsHSV to leds
	for ( int i = 0; i < NUM_LEDS; i++ ) {
		leds[i] = ledsHSV[NUM_LEDS-1-i];
	}
}
