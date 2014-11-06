#include <FastLED.h>

#include "Guino.h"
#include "EEPROM.h"
#include "EasyTransfer.h"

int16_t speedID;
int16_t scaleID;
int16_t fpsID;
int16_t brightnessID;


// The leds
#define NUM_LEDS 12

// The 32bit version of our coordinates
static uint16_t x;
static uint16_t y;
static uint16_t z;
// Between 1-100
int16_t speed = 20; // a nice starting speed, mixes well with a scale of 100
// Between 1 - 4000
int16_t scale = 311;
// This is the array that we keep our computed noise values in
uint8_t noise[NUM_LEDS];
CRGB leds[NUM_LEDS];

int16_t stripBrightness = 96;

unsigned long lastAnimation;
int16_t fps = 60;
unsigned long animatePeriod = 1000./float(fps);


void setup() {
  // uncomment the following lines if you want to see FPS count information
  // Serial.begin(38400);
  // Serial.println("resetting!");
  delay(3000);
  LEDS.addLeds<WS2801,11, 13,RGB>(leds,NUM_LEDS);
  LEDS.setBrightness(stripBrightness);

  gBegin(34220); 

  // Initialize our coordinates to some random values
  x = random16();
  y = random16();
  z = random16();

  lastAnimation = millis();
}


// Fill the x/y array of 8-bit noise values using the inoise8 function.
void fillnoise8() {
  for(int i = 0; i < NUM_LEDS; i++) {
    int ioffset = scale * i;
    noise[i] = inoise8(x + ioffset,x + ioffset,z);
  }
  z += speed;
}


void loop() {
  guino_update();
  
  // Update variables based on Guino update
  LEDS.setBrightness(uint8_t(stripBrightness));
  animatePeriod = 1000./float(fps);

  // Only run animation at desired framerate
  unsigned long timeSinceLastAnimate = millis() - lastAnimation;
  if ( timeSinceLastAnimate > animatePeriod) {

    fillnoise8();
    for(int i = 0; i < NUM_LEDS; i++) {
      leds[i] = CHSV(noise[i],255,noise[i]);
    }

    LEDS.show();
    
    lastAnimation = millis();
  }

  // gUpdateValue(&potMeter);
  // gUpdateValue(&graphValue);
}





void gInit()
{
  gAddLabel("NOISE ANIMATION VARIABLES",1);
  gAddSpacer(1);
  
  scaleID = gAddSlider(1,4000,"NOISE SCALE",&scale);
  speedID = gAddSlider(1,100,"NOISE SPEED",&speed);
  fpsID = gAddSlider(2, 60,"FPS", &fps);
  brightnessID = gAddSlider(0, 255, "BRIGHTNESS", &stripBrightness);
}

// Method called everytime a button has been pressed in the int16_terface.
void gButtonPressed(int16_t id)
{

}

void gItemUpdated(int16_t id)
{
  // if ( id = speedID ) 
}

