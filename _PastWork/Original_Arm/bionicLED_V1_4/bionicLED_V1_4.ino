// Built in Imp code
// Updated with timer to slow output to remote imp
// V1.4 - added color flag

#include "SPI.h"
#include "Adafruit_WS2801.h"
#include "SmoothInput.h"
#include "I2C.h"
#include "MMA8453_n0m1.h"
//#include "MoodLight.h"
#include "SoftwareSerial.h"

int blue = 0;
uint32_t color;
uint32_t colorRemote;

int dataPin  = 6;
int clockPin = 7;
const int stripLength = 32;
Adafruit_WS2801 strip = Adafruit_WS2801(stripLength, dataPin, clockPin);

//MoodLight ml = MoodLight();
//int hue = 125;

MMA8453_n0m1 accel;

int numSamples = 3;
SmoothInput zSmooth = SmoothInput(numSamples);

int iLevel;
int remoteLevel = 0;
int cutoff = 300;

int timer = millis();
int interval = 20;

//SoftwareSerial remoteImp(8,9);


void setup() {

//  Serial.begin(9600);
  
  strip.begin();
  strip.show();

  accel.setI2CAddr(0x1D); //change your device address if necessary, default is 0x1C
  accel.dataMode(true, 2);
  
//  remoteImp.begin(19200);

  if ( blue == 0 ) {
    color = Color(0,255,0);
    colorRemote = Color(0,0,255);
  }
  else {
    color = Color(0,0,255);
    colorRemote = Color(0,255,0);
  }

}


void loop() {

  
  
  if ( millis() - timer > interval ) {
    
//    if (remoteImp.available()) {
//    remoteLevel = remoteImp.read();
//    Serial.println(remoteLevel);
//    }
    
//    Serial.println("Interval");
    
    accel.update();
    zSmooth.smooth(abs(accel.z()));
    
    // Set level indicator for LEDs based on acceleration on X axis
    if (zSmooth.getSmooth() > cutoff) {
      iLevel = map(zSmooth.getSmooth(),cutoff,512,0,strip.numPixels()-1);
//      remoteImp.write(iLevel);
    }
    else {
      if (iLevel != 0) {
        iLevel = 0;
//        remoteImp.write(iLevel);
      }
      iLevel = 0;
    }
//    Serial.println(iLevel);

    fireAntsRev(zSmooth.getSmooth(),10);
//    powerBar();
//    randomBands(Color(ml.getRed(),ml.getGreen(),ml.getBlue()),zSmooth.getSmooth(), 15);
    
    timer = millis();
  }
  
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

void powerBar() {
  // Set # LEDs on dependent on acceleration magnitude
  for (int i=0; i < strip.numPixels(); i++) {
    if (i < iLevel && i < remoteLevel) {
      strip.setPixelColor(i, Color(255,0,0));
    }
    else if (i < iLevel && i >= remoteLevel) {
      strip.setPixelColor(i, color);
    }
    else if (i >= iLevel && i < remoteLevel) {
      strip.setPixelColor(i, colorRemote);
    }
    else {
      strip.setPixelColor(i, Color(0,0,0));
    }
  }
  strip.show();
}

void fireAnts(int accel, int wait) {
  int antSize, i;
  if (accel > cutoff) {
    antSize = map(accel,cutoff,512,2,5);  // map accelerometer value to pixels of ant
    for ( i = strip.numPixels()-1; i > antSize-1; i-- ) {
      strip.setPixelColor(i,strip.getPixelColor(i-1));
    }
    for ( i = antSize-1; i >= 0; i-- ) {
//      strip.setPixelColor(i,c/(antSize-i));
        strip.setPixelColor(i,color);
    }
  }
  else {
    for ( i = strip.numPixels()-1; i > 0; i-- )  strip.setPixelColor(i,strip.getPixelColor(i-1));
    strip.setPixelColor(0,Color(0,0,0));
  }
  
  strip.show();
  delay(wait);
}

void fireAntsRev(int accel, int wait) {
  int antSize, i;
  if (accel > cutoff) {
    antSize = map(accel,cutoff,512,2,4);  // map accelerometer value to pixels of ant
    for ( i = 0; i < stripLength-antSize; i++ ) {
      strip.setPixelColor(i,strip.getPixelColor(i+1));
    }
    for ( i = stripLength-antSize; i < stripLength; i++ ) {
//      strip.setPixelColor(i,c/(antSize-i));
        strip.setPixelColor(i,color>>(i-(stripLength-antSize)));
    }
  }
  else {
    for ( i = 0; i < stripLength-1; i++ )  strip.setPixelColor(i,strip.getPixelColor(i+1));
    strip.setPixelColor(stripLength-1,Color(0,0,0));
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
