#include <FastLED.h>

#include "Guino.h"
#include "EEPROM.h"
#include "EasyTransfer.h"

int16_t speedID;
int16_t scaleID;
int16_t fpsID;

//
// Mark's xy coordinate mapping code.  See the XYMatrix for more information on it.
//

// Params for width and height
const uint8_t kMatrixWidth = 16;
const uint8_t kMatrixHeight = 16;
#define MAX_DIMENSION ((kMatrixWidth>kMatrixHeight) ? kMatrixWidth : kMatrixHeight)
#define NUM_LEDS (kMatrixWidth * kMatrixHeight)
// Param for different pixel layouts
const bool    kMatrixSerpentineLayout = true;


uint16_t XY( uint8_t x, uint8_t y)
{
  uint16_t i;

  if( kMatrixSerpentineLayout == false) {
    i = (y * kMatrixWidth) + x;
  }

  if( kMatrixSerpentineLayout == true) {
    if( y & 0x01) {
      // Odd rows run backwards
      uint8_t reverseX = (kMatrixWidth - 1) - x;
      i = (y * kMatrixWidth) + reverseX;
    } 
    else {
      // Even rows run forwards
      i = (y * kMatrixWidth) + x;
    }
  }

  return i;
}

// The leds
CRGB leds[kMatrixWidth * kMatrixHeight];

// The 32bit version of our coordinates
static uint16_t x;
static uint16_t y;
static uint16_t z;

// We're using the x/y dimensions to map to the x/y pixels on the matrix.  We'll
// use the z-axis for "time".  speed determines how fast time moves forward.  Try
// 1 for a very slow moving effect, or 60 for something that ends up looking like
// water.
// uint16_t speed = 1; // almost looks like a painting, moves very slowly
int16_t speed = 20; // a nice starting speed, mixes well with a scale of 100
// uint16_t speed = 33;
// uint16_t speed = 100; // wicked fast!

// Scale determines how far apart the pixels in our noise matrix are.  Try
// changing these values around to see how it affects the motion of the display.  The
// higher the value of scale, the more "zoomed out" the noise iwll be.  A value
// of 1 will be so zoomed in, you'll mostly see solid colors.

// uint16_t scale = 1; // mostly just solid colors
// uint16_t scale = 4011; // very zoomed out and shimmery
int16_t scale = 311;

// This is the array that we keep our computed noise values in
uint8_t noise[MAX_DIMENSION][MAX_DIMENSION];


unsigned long lastAnimation;
int16_t fps = 60;
unsigned long animatePeriod = 1000./float(fps);

void setup() {
  // uncomment the following lines if you want to see FPS count information
  // Serial.begin(38400);
  // Serial.println("resetting!");
  delay(3000);
  LEDS.addLeds<WS2801,11, 13,RGB>(leds,NUM_LEDS);
  LEDS.setBrightness(96);

  gBegin(34220); 

  // Initialize our coordinates to some random values
  x = random16();
  y = random16();
  z = random16();

  lastAnimation = millis();
}

// Fill the x/y array of 8-bit noise values using the inoise8 function.
void fillnoise8() {
  for(int i = 0; i < MAX_DIMENSION; i++) {
    int ioffset = scale * i;
    for(int j = 0; j < MAX_DIMENSION; j++) {
      int joffset = scale * j;
      noise[i][j] = inoise8(x + ioffset,y + joffset,z);
    }
  }
  z += speed;
}


void loop() {
  guino_update();
  animatePeriod = 1000./float(fps);

  unsigned long timeSinceLastAnimate = millis() - lastAnimation;

  if ( timeSinceLastAnimate > animatePeriod) {

    static uint8_t ihue=0;
    fillnoise8();
    for(int i = 0; i < kMatrixWidth; i++) {
      for(int j = 0; j < kMatrixHeight; j++) {
        // We use the value at the (i,j) coordinate in the noise
        // array for our brightness, and the flipped value from (j,i)
        // for our pixel's hue.
        leds[XY(i,j)] = CHSV(noise[j][i],255,noise[i][j]);

        // You can also explore other ways to constrain the hue used, like below
        // leds[XY(i,j)] = CHSV(ihue + (noise[j][i]>>2),255,noise[i][j]);
      }
    }
    ihue+=1;

    LEDS.show();
    
    lastAnimation = millis();
  }

  // gUpdateValue(&potMeter);
  // gUpdateValue(&graphValue);
}





void gInit()
{
  scaleID = gAddSlider(1,4000,"NOISE SCALE",&scale);
  speedID = gAddSlider(1,30,"NOISE SPEED",&speed);
  fpsID = gAddSlider(2, 60,"FPS", &fps);
}

// Method called everytime a button has been pressed in the int16_terface.
void gButtonPressed(int16_t id)
{

}

void gItemUpdated(int16_t id)
{
  // if ( id = speedID ) 
}

