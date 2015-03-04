// Had to move this to main file for proper dependencies...
//#include "BasicParameter.h"

// ================================================================
// ===                     ABSTRACT CLASS                       ===
// ================================================================

class Animation {

	public:
		// Construction
		Animation() {}

		// Draw routine, to be called every run
		// Setting =0 makes it a pure virtual function
		// Responsible function for changing the leds[] array
		virtual void draw( unsigned long _deltaTime ) = 0;
		// void draw( unsigned long _deltaTime ) = 0;

		BasicParameter decay_Parameter; // What's a value that's physically intuitive?
		BasicParameter level_Parameter;
		BasicParameter hue_Parameter;
};



// ================================================================
// ===                     FIRE ANIMATION                       ===
// ================================================================

/*
class Fire : public Animation {

	public:
		// Model parameters
		// BasicParameter diffusionConstant = new BasicParameter();
		// What should ranges be for these parameters?  How do they affect time-based animations?
		BasicParameter diffusionParameter = BasicParameter("diff", 1, 0, 1);
		BasicParameter coolingParameter = BasicParameter("cool", 100, 20, 100);
		BasicParameter sparkingParameter = BasicParameter("spar", 50, 50, 200);

		void draw( unsigned long _deltaTime );

	private:
		// Initialize my internal model storage variables
		uint8_t heat[NUM_LEDS];

		void runModel( unsigned long _deltaTime );
		CRGB heatColor( uint8_t _temperature );

};


void Fire::draw ( unsigned long _deltaTime ) {
	runModel(_deltaTime);

	// Push model to LEDs
    for( int j = 0; j < NUM_LEDS; j++) {
        leds[j] = heatColor( heat[j] );
        SERIAL_PRINTLN(heat[j]);
        SERIAL_PRINTLN(leds[j]);
        SERIAL_PRINTLN(leds[j]);
        SERIAL_PRINTLN(leds[j]);
        SERIAL_PRINTLN();
    }
}

void Fire::runModel ( unsigned long _deltaTime ) {

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

*/


// ================================================================
// ===                     TRAIN ANIMATION                      ===
// ================================================================
/*
class Train : public Animation {

	public:
		// Model parameters
		BasicParameter trainSpeed_Parameter = BasicParameter("spd", 300, 1, 500);
		BasicParameter trainLength_Parameter = BasicParameter("len", 3, 1, 5);
		BasicParameter trainPeriod_Parameter = BasicParameter("perd", 2000, 500, 4000);
		BasicParameter hue_Parameter = BasicParameter("hue", 120, 0, 255);

		void draw( unsigned long _deltaMs );


	private:
		unsigned long lastMove = 0;
		unsigned long lastTrainArrival = 0;

		void shiftPixels();
		void newTrain();

};


void Train::draw ( unsigned long _deltaTime ) {
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
*/

// ================================================================
// ===                     SPARKLE ANIMATION                    ===
// ================================================================

class Sparkle : public Animation {

	public:
		Sparkle() {
			// Model parameters
			decay_Parameter.initBasicParameter("dec", 8, 5, 25); // What's a value that's physically intuitive?
			level_Parameter.initBasicParameter("lvl", 3, 1, 7);
			hue_Parameter.initBasicParameter("hue", 150, 80, 200);

			lastSpark = 0;
			lastDimming = 0;
			sparkPeriod = 200;

			sparkBarrier = 300;

			isRandomSparkingOn = false;
		}

		void draw( unsigned long _deltaMs );
		void trigger();


	private:
		void newSparkles();

		unsigned long lastSpark;
		unsigned long lastDimming;
		unsigned long sparkPeriod;

		unsigned long sparkBarrier;

		bool isRandomSparkingOn;

};


void Sparkle::draw ( unsigned long _deltaTime ) {
	// Decrement the brightness
	for( int i = 0; i < NUM_LEDS; i++ ) {
                // uint8_t decrement;
//                if ( ledsHSV[i].val > 200 ) decrement = decay_Parameter.getValue();
//                else if ( ledsHSV[i].val > 150 ) decrement = decay_Parameter.getValue()*2/3;
//                else if ( ledsHSV[i].val > 50 ) decrement = decay_Parameter.getValue()/3;
		ledsHSV[i].val = qsub8( ledsHSV[i].val, decay_Parameter.getValue() );
		// ledsHSV[i].val = qsub8( ledsHSV[i].val, decrement );
		ledsHSV[i].hue = hue_Parameter.getValue();
//		ledsHSV[i].hue = 200;
		ledsHSV[i].sat = 200;
	}

	// Trigger random sparkles
	if ( isRandomSparkingOn ) {
		if ( (millis()-lastSpark) > sparkPeriod ) {
                        level_Parameter.setValue(random(1,3));
			trigger();
                        level_Parameter.setPercent(0);
			lastSpark = millis();
                        sparkPeriod = random(100, 500);
		}
	}

	// Push ledsHSV to leds
	for ( int i = 0; i < NUM_LEDS; i++ ) {
		leds[i] = ledsHSV[i];
	}
}


void Sparkle::trigger () {
  if ( millis() - lastSpark > sparkBarrier )
	SERIAL_PRINT("Trigger! Percent = ");
	SERIAL_PRINTLN(level_Parameter.getPercent());
	for ( byte sparkle = 0; sparkle < int(level_Parameter.getValue()); sparkle ++) {
		byte pixelIndex = random8(0, NUM_LEDS-1);

		uint8_t brightnessOffset = 80;
		uint8_t sparkBrightness = brightnessOffset + random(0, int( level_Parameter.getPercent() * (255.-brightnessOffset) ) );
		ledsHSV[pixelIndex].val = max( sparkBrightness, ledsHSV[pixelIndex].val );
                if ( pixelIndex > 0 && pixelIndex < NUM_LEDS-1 )  {
                  ledsHSV[pixelIndex-1].val = max( sparkBrightness/6, ledsHSV[pixelIndex-1].val );
                  ledsHSV[pixelIndex+1].val = max( sparkBrightness/6, ledsHSV[pixelIndex+1].val );
                }
	}
        lastSpark = millis();
}


// ================================================================
// ===                   POWER BAR ANIMATION                    ===
// ================================================================

class Power : public Animation {

	public:
		Power() {
			// Model parameters
			decay_Parameter.initBasicParameter("dec", 25, 5, 25); // What's a value that's physically intuitive?
			level_Parameter.initBasicParameter("lvl", 0, 0, NUM_LEDS);
			hue_Parameter.initBasicParameter("hue", 150, 120, 255); // Trip is 200

			long lastSpark = 0;
			long lastDimming = 0;
		}

		void draw( unsigned long _deltaMs );

	private:

		long lastSpark;
		long lastDimming;

};


void Power::draw ( unsigned long _deltaTime ) {
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

/*
class RunningRainbow : public Animation {

	public:
		RunningRainbow() {
			// Model parameters
			decay_Parameter.initBasicParameter(5, 1, 255); // What's a value that's physically intuitive?
			level_Parameter.initBasicParameter(0, 0, 255);
			hue_Parameter.initBasicParameter(120, 0, 150);
		}

		BasicParameter trainSpeed_Parameter = BasicParameter("spd", 10, 1, 100);

		void draw( unsigned long _deltaMs );

	private:

		long lastMove = 0;
		long lastDimming = 0;
		uint8_t deltaHuePixel = 10;

};


void RunningRainbow::draw ( unsigned long _deltaTime ) {
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
*/


// ================================================================
// ===                      NOISE ANIMATION                     ===
// ================================================================

class Noise : public Animation {

	public:
		Noise() {
			// Model parameters
			decay_Parameter.initBasicParameter("dec", 5, 1, 255); // What's a value that's physically intuitive?
			level_Parameter.initBasicParameter("lvl", 0, 0, 255);
			hue_Parameter.initBasicParameter("hue", 120, 0, 150);
			x = random16();
			y = random16();
			z = random16();

			speedy = 10;
	//		uint16_t scale = 3500;
			scale = 2000;
			SHIFT_BRIGHTNESS = 10;
			SHIFT_SATURATION = 50;
		}

		void draw( unsigned long _deltaMs );

	private:
		uint16_t speedy;
		uint16_t scale;
		uint8_t SHIFT_BRIGHTNESS;
		uint8_t SHIFT_SATURATION;
		uint8_t noise[NUM_LEDS];

		uint16_t x;
		uint16_t y;
		uint16_t z;

		void fillnoise8();
};

// Fill the x/y array of 8-bit noise values using the inoise8 function.
void Noise::fillnoise8() {
  for(int i = 0; i < NUM_LEDS; i++) {
    int ioffset = scale * i;
    noise[i] = inoise8( x + ioffset, 0 , z );
  }
  z += speedy;
}

void Noise::draw ( unsigned long _deltaTime ) {

//	scale = 2000. * level_Parameter.getPercent()+1;
	// speedy = 50. * level_Parameter.getPercent()+5;
//        SERIAL_PRINT("Noise SCALE = "); SERIAL_PRINTLN(scale);
        SERIAL_PRINT("Noise SPEED = "); SERIAL_PRINTLN(speedy);


	fillnoise8();

	// Push ledsHSV to leds
	for ( int i = 0; i < NUM_LEDS; i++ ) {
		uint8_t noiseBrightness = map( constrain( noise[i]-SHIFT_BRIGHTNESS, 0, 255 ),
                                  		0, (255-SHIFT_BRIGHTNESS), 0, 255 );
		uint8_t thisBrightness = noiseBrightness;

//		ledsHSV[i] = CHSV( 120, 255, max( ledsHSV[i].val-DECAY, int(thisBrightness) ) );
		ledsHSV[i] = CHSV( 120, 255, int(thisBrightness) );
		leds[i] = ledsHSV[i];
	}
}




// ================================================================
// ===                   DUAL POWER BAR ANIMATION                    ===
// ================================================================


class DualPower : public Animation {

	public:
		DualPower() {
			// Model parameters
			decay_Parameter.initBasicParameter("dec", 35, 5, 35); // What's a value that's physically intuitive?
			// This probably does not make level1 and level the same construct...
			level1_Parameter = this->level_Parameter;
			level1_Parameter.initBasicParameter("lv1", 0, 0, NUM_LEDS);
			level2_Parameter.initBasicParameter("lv2", 0, 0, NUM_LEDS);
			hue_Parameter.initBasicParameter("hue", 200, 120, 255);
		}

		BasicParameter level1_Parameter;
		BasicParameter level2_Parameter;

		void draw( unsigned long _deltaMs );

};


void DualPower::draw ( unsigned long _deltaTime ) {
	// Decrement the brightness
	for( int i = 0; i < NUM_LEDS; i++ ) {
		ledsHSV[i].val = qsub8( ledsHSV[i].val, int(decay_Parameter.getValue()) );
		ledsHSV[i].sat = 200;
	}

	for ( int i = 0; i < level1_Parameter.getValue()/2; i++ ) {
		ledsHSV[i].val = 255;
		ledsHSV[i].hue = hue_Parameter.getValue();
	}
	for ( int i = NUM_LEDS-1; i > NUM_LEDS-1-level2_Parameter.getValue()/2; i-- ) {
		ledsHSV[i].val = 255;
		byte hueTop = hue_Parameter.getValue() - 130;
		ledsHSV[i].hue = hueTop;
	}

	// Push ledsHSV to leds
	for ( int i = 0; i < NUM_LEDS; i++ ) {
		leds[i] = ledsHSV[NUM_LEDS-1-i];
	}
}
