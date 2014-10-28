#include "SPI.h"
#include "Adafruit_WS2801.h"
#include "MoodLight.h"


int dataPin  = 6;
int clockPin = 7;
const int stripLength = 32;
Adafruit_WS2801 strip = Adafruit_WS2801(stripLength, dataPin, clockPin);

MoodLight ml = MoodLight();
int hue = 125;
int val = 0;

double iLevel;
int cutoff = 300;

int accel = 0;
int impulse = 0;

void setup() {
  Serial.begin(9600);
  strip.begin();
  strip.show();
  ml.setHSB(hue, 255, val);
}

void loop() {

//  randomBands(Color(ml.getRed(),ml.getGreen(),ml.getBlue()),impulse, 5);
  fadeBand(Color(ml.getRed(),ml.getGreen(),ml.getBlue()),impulse, 5);
  
}


void fadeBand(uint32_t c, int acc, int wait) {
  for ( int i = 0; i < strip.numPixels(); i++ ) {
    if ( acc > cutoff ) {
      strip.setPixelColor(i,c);
    }
    else {
      strip.setPixelColor(i,strip.getPixelColor(i)*4/5);
    }
  }
  strip.show();
  delay(wait);
}


void randomBands(uint32_t c, int accel, int wait) {
  
  // Fade out existing pixels slowly
  for ( int i = 0; i < strip.numPixels(); i++ )  strip.setPixelColor(i,strip.getPixelColor(i)*9/10);
  
  if (accel > cutoff) {
    int antSize = map(accel,cutoff,512,1,3);  // map accelerometer value to (band size - 1)/2
    int antCenter = random(0, strip.numPixels());
    for ( int j = antCenter-antSize ; j <= antCenter+antSize ; j++ ) {
      if ( j > 0 && j < strip.numPixels() ) {
        float coeff = -abs(j)*.9/antSize + 1;
        strip.setPixelColor(j, coeff*c);
      }
    }
  }
  
  strip.show();
  delay(wait);
}

uint32_t Color(byte r, byte g, byte b)
{
  uint32_t c;
  c = r;
  c <<= 8;
  c |= g;
  c <<= 8;
  c |= b;
  return c;
}
