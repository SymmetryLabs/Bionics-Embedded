// Had to move this to main file for proper dependencies...
// #include "BasicParameter.h"

// ================================================================
// ===                     ABSTRACT CLASS                       ===
// ================================================================

class Animation {

	public:
		// Construction
		Animation() {}
//		Animation() {}

		// Draw routine, to be called every run
		// Setting =0 makes it a pure virtual function
		// Responsible function for changing the leds[] array
		virtual void draw( unsigned long _deltaTime ) = 0;
		// void draw( unsigned long _deltaTime ) = 0;

		BasicParameter level_Parameter;
		BasicParameter decay_Parameter;
		BasicParameter hue_Parameter;
};



// ================================================================
// ===                     SPARKLE ANIMATION                    ===
// ================================================================

class Sparkle : public Animation {

	public: 
		// Model parameters
		BasicParameter decay_Parameter = BasicParameter("deca", 5, 5, 25); // What's a value that's physically intuitive?
		BasicParameter level_Parameter = BasicParameter("lvl", 3, 1, 7);
		BasicParameter hue_Parameter = BasicParameter("hue", 120, 0, 150);

		void draw( unsigned long _deltaMs );
		void trigger();


	private:
		void newSparkles();

		unsigned long lastSpark = 0;
		unsigned long lastDimming = 0;
		unsigned long sparkPeriod = 2000;

		bool isRandomSparkingOn = false;

};


void Sparkle::draw ( unsigned long _deltaTime ) {
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
	Serial.println(level_Parameter.getPercent());
	for ( byte sparkle = 0; sparkle < int(level_Parameter.getValue()); sparkle ++) {
		byte pixelIndex = random8(0, NUM_LEDS-1);

		uint8_t brightnessOffset = 150;
		uint8_t sparkBrightness = brightnessOffset + random8(0, int( level_Parameter.getPercent() * (255.-brightnessOffset) ) );
		ledsHSV[pixelIndex].setHSV( hue_Parameter.getValue(), 255, sparkBrightness );
	}
}


// ================================================================
// ===                   POWER BAR ANIMATION                    ===
// ================================================================

class Power : public Animation {

	public:
		// Model parameters
		BasicParameter decay_Parameter = BasicParameter("deca", 15, 5, 25); // What's a value that's physically intuitive?
		BasicParameter level_Parameter = BasicParameter("lvl", 0, 0, NUM_LEDS);
		BasicParameter hue_Parameter = BasicParameter("hue", 120, 0, 255);

		void draw( unsigned long _deltaMs );

	private:

		long lastSpark = 0;
		long lastDimming = 0;

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

class RunningRainbow : public Animation {

	public:
		// Model parameters
		BasicParameter decay_Parameter = BasicParameter("deca", 5, 1, 255); // What's a value that's physically intuitive?
		BasicParameter level_Parameter = BasicParameter("lvl", 0, 0, 255);
		BasicParameter hue_Parameter = BasicParameter("hue", 120, 0, 150);
		BasicParameter trainSpeed_Parameter = BasicParameter("spd", 10, 1, 100);

		void draw( unsigned long _deltaMs );

	private:

		long lastMove = 0;
		long lastDimming = 0;
		uint8_t deltaHuePixel = 10;

};


void RunningRainbow::draw ( unsigned long _deltaMs ) {
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
