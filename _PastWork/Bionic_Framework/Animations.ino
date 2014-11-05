
class Animation {
// class Animation() {

	public:
		// Construction
		Animation() {

		}

		// void draw(long _deltaTime, CRGB _leds[NUM_LEDS]);  // Does this need to be a pointer?
		void draw(long _deltaTime) {

		}
};



// compute model



// translate to LEDS

/*
class Fire extends Animation {

	public:
		// Model parameters
		// BasicParameter diffusionConstant = new BasicParameter();
		uint8_t diffusionConstant = 0.5;
		uint8_t coolingConstant = 0.5;
		uint8_t sparkingConstant = 0.5;

	private:
		// Initialize my internal model storage variables
		uint8_t heat[NUM_LEDS] = 0;


	CRGB draw(long _deltaTime) {
		runModel(_deltaTime);
		return heatMapping();
	}

	void runModel(long _deltaTime) {
		// Store next model state while computing
		uint8_t newHeat[NUM_LEDS];

		// Run diffusion
		// NEED TO ADD DIFFUSION CONSTANT
		newHeat[0] = ( heat[0] + heat[1] ) / 2;
		for ( int node = 1; node < NUM_LEDS-1; node++ ) newHeat[node] = ( heat[node-1] + heat[node] + heat[node+1];
		newHeat[NUM_LEDS-1] = ( heat[NUM_LEDS-2] + heat[NUM_LEDS-1] ) / 2;

		// Run cooling
		for ( int node = 0; node < NUM_LEDS; node++ ) newHeat[node] = coolingConstant * newHeat[node];

		// Run sparking
		uint8_t newSpark = random(0, 255);
		newHeat[0] = constrain( newHeat[0] + newSpark, 0, 255);
	}

	private heatMapping() {

	}

}
*/


/*
class Train extends Animation {

	public:
		// Model parameters
		float trainSpeed = 100; // Milliseconds per pixel
		uint8_t trainLength = 2;
		long trainPeriod = 2000; // In milliseconds
		CRGB trainColor = CRGB::Blue;

	private:
		long lastMove = 0;
		long lastTrainArrival = 0;


	// Empty constructor
	Train::Train() {

	}


	void draw(long _deltaMs) {  // Would be great to add a point to LEDs here...
		// Move existing trains along
		if ( (millis()-lastMove) > trainSpeed ) {
			shiftPixels();
			lastMove = millis();
		}

		// Determine arrival of new trains
		if ( (millis()-lastArival) > trainPeriod ) {
			newTrain();
			lastTrainArrival = millis();
		}
	}


	void shiftPixels() {
		leds[0] = CRGB::Black;
		for ( int node = 1; node < NUM_LEDS; node++ ) leds[node] = leds[node-1];
	}

	void newTrain() {
		for ( int node = 0; node < trainLength; node++ ) leds[node] = trainColor;
	}

}
*/


class Train {

	public:
		// Model parameters
		float trainSpeed = 100; // Milliseconds per pixel
		uint8_t trainLength = 2;
		long trainPeriod = 2000; // In milliseconds
		CRGB trainColor = CRGB::Blue;


	private:
		long lastMove = 0;
		long lastTrainArrival = 0;

	public Train() {

	}

	public void draw(long _deltaMs) {  // Would be great to add a point to LEDs here...
		// Move existing trains along
		if ( (millis()-lastMove) > trainSpeed ) {
			shiftPixels();
			lastMove = millis();
		}

		// Determine arrival of new trains
		if ( (millis()-lastArival) > trainPeriod ) {
			newTrain();
			lastTrainArrival = millis();
		}
	}


	private void shiftPixels() {
		leds[0] = CRGB::Black;
		for ( int node = 1; node < NUM_LEDS; node++ ) leds[node] = leds[node-1];
	}

	private void newTrain() {
		for ( int node = 0; node < trainLength; node++ ) leds[node] = trainColor;
	}

};