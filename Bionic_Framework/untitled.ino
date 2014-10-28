// Engine variables
uint8_t POWERBAR_level = 0;

// Engine parameters
uint8_t POWERBAR_parameter_decay = 10;
uint8_t POWERBAR_parameter_color = CRGB::Blue;

// External functions
void POWERBAR_draw(long _deltaMs) {
	// Decay existing pixels
	for ( int node=0; node < NUM_PIXELS; node++) {
		leds[node].v( dim8_raw( leds[node]::v );
}