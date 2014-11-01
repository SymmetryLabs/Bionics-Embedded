#include <FastLED.h>

// #define CLK_PIN     11
// #define DATA_PIN    13
// #define BRIGHTNESS  96
// #define LED_TYPE    WS2811
// #define COLOR_ORDER GRB

// Params for width and height
#define NUM_LEDS 12
// Param for different pixel layouts
//const bool    kMatrixSerpentineLayout = true;

// The leds
CRGB leds[NUM_LEDS];
CHSV ledsHSV[NUM_LEDS];

// The 32bit version of our coordinates
static uint16_t x;
static uint16_t y;
static uint16_t z;

// We're using the x/y dimensions to map to the x/y pixels on the matrix.  We'll
// use the z-axis for "time".  speed determines how fast time moves forward.  Try
// 1 for a very slow moving effect, or 60 for something that ends up looking like
// water.
// uint16_t speed = 1; // almost looks like a painting, moves very slowly
uint16_t speed = 15; // a nice starting speed, mixes well with a scale of 100
// uint16_t speed = 33;
// uint16_t speed = 100; // wicked fast!

// Scale determines how far apart the pixels in our noise matrix are.  Try
// changing these values around to see how it affects the motion of the display.  The
// higher the value of scale, the more "zoomed out" the noise iwll be.  A value
// of 1 will be so zoomed in, you'll mostly see solid colors.

// uint16_t scale = 1; // mostly just solid colors
// uint16_t scale = 4011; // very zoomed out and shimmery
uint16_t scale = 3500;

#define DECAY 2
#define SHIFT_BRIGHTNESS 120
#define SHIFT_SATURATION 50



// This is the array that we keep our computed noise values in
uint8_t noise[NUM_LEDS];


void setup() {
  // uncomment the following lines if you want to see FPS count information
  Serial.begin(38400);
  Serial.println("resetting!");
  delay(3000);

  LEDS.addLeds<WS2801, 11, 13, RGB, DATA_RATE_MHZ(1)>(leds, NUM_LEDS);
  LEDS.setBrightness(255);

  // Initialize our coordinates to some random values
  x = random16();
  y = random16();
  z = random16();
}


// Fill the x/y array of 8-bit noise values using the inoise8 function.
void fillnoise8() {
  for(int i = 0; i < NUM_LEDS; i++) {
    int ioffset = scale * i;
    noise[i] = inoise8( x + ioffset, 0 , z );
  }
  z += speed;
}



void loop() {
  static uint8_t ihue=0;
  fillnoise8();
  for(int i = 0; i < NUM_LEDS; i++) {
    // We use the value at the (i,j) coordinate in the noise
    // array for our brightness, and the flipped value from (j,i)
    // for our pixel's hue.
//    uint8_t thisSaturation = map( constrain( noise[i]-SHIFT_SATURATION, 0, 255 ),
//                                   0, (255-SHIFT_SATURATION), 0, 255 );
    uint8_t thisSaturation = 255;
    
//    uint8_t noiseBrightness = map( constrain( noise[i]-SHIFT_BRIGHTNESS, 0, 255 ),
//                                   0, (255-SHIFT_BRIGHTNESS), 0, 255 );
//                                   
//    uint8_t thisBrightness = max( ledsHSV[i].val-DECAY, noiseBrightness );

    
    
//    ledsHSV[i] = CHSV( 0, thisSaturation, thisBrightness );
//    float thisBrightness = 0.8 * ( exp( 0.023*noise[i] ) - 1 );
    ledsHSV[i] = CHSV( 120, 255, max( ledsHSV[i].val-DECAY, int(thisBrightness) ) );
    
    leds[i] = ledsHSV[i];
  }
  ihue+=1;

  LEDS.show();
  delay(10);
}
