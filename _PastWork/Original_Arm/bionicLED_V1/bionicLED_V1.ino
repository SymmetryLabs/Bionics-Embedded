/*
  Created 3/7/13
  Last modified 3/7/13

  First LED Bionic Arm Code
*/

#include "SPI.h"
#include "Adafruit_WS2801.h"
#include "SmoothInput.h"
#include "I2C.h"
#include "MMA8453_n0m1.h"
//#include "MoodLight.h"


int dataPin  = 6;
int clockPin = 7;
const int stripLength = 32;
Adafruit_WS2801 strip = Adafruit_WS2801(stripLength, dataPin, clockPin);

//MoodLight ml = MoodLight();
//int hue = 125;

MMA8453_n0m1 accel;

int numSamples = 20;
SmoothInput zSmooth = SmoothInput(numSamples);

double iLevel;
int cutoff = 300;


void setup() {

  Serial.begin(9600);
  
  strip.begin();
  strip.show();

  accel.setI2CAddr(0x1D); //change your device address if necessary, default is 0x1C
  accel.dataMode(true, 2);

}


void loop() {
  
//  if (hue >= 255) {
//    hue = 0;
//  }
//  else {
//    hue = hue + 10;
//  }
//  ml.setHSB(hue, 255, 255);
  
  accel.update();
  zSmooth.smooth(abs(accel.z()));

//  Serial.print("x: ");
//  Serial.print(accel.x());
//  Serial.print(" y: ");
//  Serial.print(accel.y());
//  Serial.print(" z: ");
//  Serial.println(accel.z());

  // Set level indicator for LEDs based on acceleration on X axis
  iLevel = map(zSmooth.getSmooth(),0,512,0,strip.numPixels());

  powerBar(Color(0,255,0));
//  randomBands(Color(ml.getRed(),ml.getGreen(),ml.getBlue()),zSmooth.getSmooth(), 15);
}

// Create a 24 bit color value from R,G,B
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

void powerBar(uint32_t c) {
  // Set # LEDs on dependent on acceleration magnitude
  for (int i=0; i < strip.numPixels(); i++) {
    if (i < iLevel) {
      strip.setPixelColor(i, c);
    }
    else {
      strip.setPixelColor(i, Color(0,0,0));
    }
  }
  strip.show();
}

void fireAnts(uint32_t c, int accel, int wait) {
  int antSize, i;
  if (accel > cutoff) {
    antSize = map(accel,cutoff,512,1,4);  // map accelerometer value to pixels of ant
    for ( i = strip.numPixels()-1; i > antSize-1; i-- ) {
      strip.setPixelColor(i,strip.getPixelColor(i-1));
    }
    for ( i = antSize-1; i >= 0; i-- ) {
//      strip.setPixelColor(i,c/(antSize-i));
        strip.setPixelColor(i,c);
    }
  }
  else {
    for ( i = strip.numPixels()-1; i > 0; i-- )  strip.setPixelColor(i,strip.getPixelColor(i-1));
    strip.setPixelColor(0,Color(0,0,0));
  }
  
  strip.show();
  delay(wait);
}


void randomBands(uint32_t c, int accel, int wait) {
  
  // Fade out existing pixels slowly
  for ( int i = 0; i < strip.numPixels(); i++ )  strip.setPixelColor(i,strip.getPixelColor(i)*2/5);
  
  if (accel > cutoff) {
    int antSize = map(accel,cutoff,512,1,3);  // map accelerometer value to (band size - 1)/2
    int antCenter = random(0, strip.numPixels());
    for ( int j = antCenter-antSize ; j <= antCenter+antSize ; j++ ) {
      if ( j > 0 && j < strip.numPixels() ) {
        float coeff = -abs(j)*.8/antSize + 1;
        strip.setPixelColor(j, coeff*c);
      }
    }
  }
  
  strip.show();
  delay(wait);
}
