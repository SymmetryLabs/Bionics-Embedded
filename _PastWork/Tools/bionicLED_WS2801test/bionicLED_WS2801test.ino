/*
  Created 10/2/12
  Last modified 10/2/12

  Power Level Tester for Bionics
*/

#include "SPI.h"
#include "Adafruit_WS2801.h"

int dataPin  = 2;
int clockPin = 3;

Adafruit_WS2801 strip = Adafruit_WS2801(32, dataPin, clockPin);


int iLevel;


void setup() {

  Serial.begin(9600);
  
  strip.begin();
  strip.show();

}


void loop() {

  // Set level indicator for LEDs based on acceleration on X axis
//  iLevel = map(abs(accel.x()),0,512,0,strip.numPixels()-1);
  iLevel = 8;

  // Set # LEDs on dependent on acceleration magnitude
  for (int i=0; i < strip.numPixels(); i++) {
    if (i < iLevel) {
      strip.setPixelColor(i,Color(255,0,0));
    }
    else {
      strip.setPixelColor(i,Color(0,0,0));
    }
  }
  strip.show();

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

