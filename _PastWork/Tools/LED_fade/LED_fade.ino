#include "SPI.h"
#include "Adafruit_WS2801.h"
#include "MoodLight.h"
#include "TimerOne.h"


int dataPin  = 6;
int clockPin = 7;
const int stripLength = 32;
Adafruit_WS2801 strip = Adafruit_WS2801(stripLength, dataPin, clockPin);
int val = 0;

MoodLight ml = MoodLight();
int hue = 100;
int sat = 255;

void setup() {

  Serial.begin(9600);
  
  strip.begin();
  strip.show();
  
//  Timer1.initialize(100000);
//  Timer1.attachInterrupt( timerIsr );
  
}

void timerIsr() {
   fade(0);
   Serial.println("trigger");
}

void loop() {
  fade(0);
}


void fade(int wait) {
  for (int i=0; i < stripLength; i++ ) {
    if (val < 256) {
      val++;
    }
    else {
      val = 0;
    }
    ml.setHSB(hue, sat, val);
//    uint32_t color = ml.getColor();
    uint32_t color = Color(ml.getRed(),ml.getGreen(),ml.getBlue());
    Serial.print("Color ");
    Serial.println(color);
    Serial.print("Val ");
    Serial.println(val);
    Serial.print("DC val ");
    Serial.println(ml.getDc(val));
    Serial.println();
    strip.setPixelColor(i, color);
  }
  strip.show();
  delay(wait);
}

//void fadeRGB(int wait) {
//  for (int i=0; i < stripLength; i++ ) {
//    if (values[i] > 0) {
//      values[i]--;
//    }
//    else {
//      values[i] = 255;
//    }
//    strip.setPixelColor(i, Color(values[i],0,0));
//  }
//  strip.show();
//  delay(wait);
//}
//
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
