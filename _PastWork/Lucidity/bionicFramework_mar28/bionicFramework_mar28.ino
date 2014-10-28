#include <SoftwareSerial.h>

#include "SPI.h"
#include "Adafruit_WS2801.h"

#include "I2C.h"
#include "MMA8453_n0m1.h"

#include "SmoothInput.h"

#include "Timer.h"

// For the electronic wiring , you should :
// Connect pinRx to the Pin2 of XBee(Tx , Dout)
// Connect pinTx to the Pin3 of XBee(Rx , Din)
// Define the pins on Arduino for XBee comminication
uint8_t pinRx = 2 , pinTx = 4; // the pin on Arduino
long BaudRate = 9600 , sysTick = 0;
char GotChar;
// Initialize NewSoftSerial
SoftwareSerial mySerial( pinRx , pinTx );

Timer t;
byte event_ind;

int dataPin  = 6;
int clockPin = 7;
const int stripLength = 32;
Adafruit_WS2801 strip = Adafruit_WS2801(stripLength, dataPin, clockPin);

MMA8453_n0m1 accel;
//int accelMag;

int numSamples = 5;
SmoothInput accelMag = SmoothInput(numSamples);

float decay = 0;
float decayCounter = 0;
float decayCoeff = 10;


float alpha = 0.5;
long bumpSpeed = 1000;
unsigned long lastBumpTime;

int hue[stripLength];
byte sat[stripLength];
byte val[stripLength];
uint32_t color;

int hueNow = 0;
byte valNow;

int sine[stripLength];

uint8_t dc[256] = {
    0,   1,   1,   2,   2,   2,   2,   2,   2,   3,   3,   3,   3,   3,   3,   3,
    3,   3,   3,   3,   3,   3,   3,   4,   4,   4,   4,   4,   4,   4,   4,   4,
    4,   4,   4,   5,   5,   5,   5,   5,   5,   5,   5,   5,   5,   6,   6,   6,
    6,   6,   6,   6,   6,   7,   7,   7,   7,   7,   7,   7,   8,   8,   8,   8,
    8,   8,   9,   9,   9,   9,   9,   9,   10,  10,  10,  10,  10,  11,  11,  11,
    11,  11,  12,  12,  12,  12,  12,  13,  13,  13,  13,  14,  14,  14,  14,  15,
    15,  15,  16,  16,  16,  16,  17,  17,  17,  18,  18,  18,  19,  19,  19,  20,
    20,  20,  21,  21,  22,  22,  22,  23,  23,  24,  24,  25,  25,  25,  26,  26,
    27,  27,  28,  28,  29,  29,  30,  30,  31,  32,  32,  33,  33,  34,  35,  35,
    36,  36,  37,  38,  38,  39,  40,  40,  41,  42,  43,  43,  44,  45,  46,  47,
    48,  48,  49,  50,  51,  52,  53,  54,  55,  56,  57,  58,  59,  60,  61,  62,
    63,  64,  65,  66,  68,  69,  70,  71,  73,  74,  75,  76,  78,  79,  81,  82,
    83,  85,  86,  88,  90,  91,  93,  94,  96,  98,  99,  101, 103, 105, 107, 109,
    110, 112, 114, 116, 118, 121, 123, 125, 127, 129, 132, 134, 136, 139, 141, 144,
    146, 149, 151, 154, 157, 159, 162, 165, 168, 171, 174, 177, 180, 183, 186, 190,
    193, 196, 200, 203, 207, 211, 214, 218, 222, 226, 230, 234, 238, 242, 248, 255,
};


void setup() {
  Serial.begin(9600);
  
  // This part is the NewSoftSerial for talking to XBee
  mySerial.begin(BaudRate);
  mySerial.println("Arduino Uno Online!");
  
  accel.setI2CAddr(0x1D);
  accel.dataMode(true, 2);
  
  strip.begin();
  strip.show();
  
  for ( int i=0; i < stripLength; i++ ) {
    hue[i] = i*360/stripLength;
//    hue[i] = 120;
    sat[i] = 255;
//    sine[i] = abs(sin( (float(i)*2*PI)/31. )*255);
//    sat[i] = sine[i];
//    val[i] = sine[i];
    val[i] = 255;
//    Serial.println(sine[i]);
    setPixelHSB(i);
  }
  
  lastBumpTime = millis();
  
  event_ind = t.every( 10, powerFade );
//  hsb[0] = 255;
//  hsb[1] = 255;
//  hsb[2] = 255;
//  HSBtoRGB(hsb, &color);

//  color = Color(0, 0, 255);
//  RGBtoHSB(color,hsb);
//  Serial.print("Hue ");
//  Serial.println(hsb[0]);
//  Serial.print("Sat ");
//  Serial.println(hsb[1]);
//  Serial.print("Val ");
//  Serial.println(hsb[2]);
}


void loop() {
  
  mySerial.println("R255");
  
  accel.update();
  accelMag.smooth(sqrt(pow(accel.x(),2)+pow(accel.y(),2)+pow(accel.z(),2))-256);
//  Serial.println();
//  Serial.println(accel.x());
//  Serial.println(accelMag);
  if ( accelMag.getSmooth() > 350 && millis()-lastBumpTime > 100 ) {
    bumpSpeed = alpha*bumpSpeed + (1-alpha)*constrain((lastBumpTime-millis()),10,1000);
    lastBumpTime = millis();
  }
  
  t.update();
  t._events[event_ind].period = bumpSpeed/10;
  strip.show();
  
//  for ( int i = 0; i<stripLength; i++) {
//    hue[i] = incHue(hue[i]);
//    val[i] = incVal(val[i]);
//    setPixelHSB(i);
//    
//    
//    setPixelHSB(i,incHue(hue[i]),sat,val)
//    
//  }
}




int incHue(int h, byte amt) {
  if ( h+amt < 360) h+=amt;
  else h+=(amt-360);
  return h;
}

byte incVal(byte v) {
  if ( v < 255) v++;
  else v=0;
  return v;
}

byte decVal(byte v) {
  if ( v > 0) v--;
  else v=255;
  return v;
}

  

void setPixelHSB(int i) {
  byte valAdj = dc[val[i]];
  int hsb[3] = {hue[i], sat[i], valAdj};
  uint32_t color;
  HSBtoRGB(hsb, &color);
  strip.setPixelColor(i, color);
}



// PATTERN FUNCTIONS


void powerFade() {
  int i;
  int az = abs(accel.z());
  valNow = map(constrain(accelMag.getSmooth(),0,512),0,512,0,255);
  if ( valNow > decay ) {
    decay = valNow;
    decayCounter = 0;
  }
  else {
    decayCounter++;
    decay-=decayCoeff*pow(decayCounter,2);
  }
  if ( decay < 0 ) {
    decay = 0;
    decayCounter = 0;
  }
  for ( i=0; i < stripLength; i++ ) {
    val[i] = decay;
    hue[i] = hueNow;
    setPixelHSB(i);
  }
}

void power() {
  int i;
  int az = abs(accel.z());
  valNow = map(constrain(accelMag.getSmooth(),0,512),0,512,0,255);
//  if ( az > 500) hueNow = incHue(hueNow,30);
  for ( i=0; i < stripLength; i++ ) {
    val[i] = valNow;
    hue[i] = hueNow;
    setPixelHSB(i);
  }
}

void fireAnts() {
  int i;
  int cutoff = 300;
  int antSize = 3;
  int az = abs(accel.z());
  shiftPixelsUp();
  if ( az > 500 ) hueNow = incHue(hueNow,30);
  if ( accelMag.getSmooth() > cutoff ) {
    for ( i = stripLength-1; i > stripLength-1-antSize; i-- ) {
      hue[i] = hueNow;
      setPixelHSB(i);
//      strip.setPixelColor(i,Color(-102*i+3227,0,0));
    }
  }
}


void shiftPixelsUp() {
  for (int i = 0; i < stripLength-1; i++ )  strip.setPixelColor(i,strip.getPixelColor(i+1));
  strip.setPixelColor(stripLength-1,Color(0,0,0));
}

void shiftPixelsUpCont() {
  strip.setPixelColor(stripLength-1,strip.getPixelColor(0));
  for (int i = 0; i < stripLength-1; i++ )  strip.setPixelColor(i,strip.getPixelColor(i+1));
}

void shiftPixelsDown() {
  for (int i = stripLength-1; i > 0; i-- )  strip.setPixelColor(i,strip.getPixelColor(i-1));
  strip.setPixelColor(0,Color(0,0,0));
}




// COLOR FUNCTIONS

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



void HSBtoRGB(int hsb[], uint32_t *c) { 
  
  int r;
  int g;
  int b;
  int base;
  int hue;
  int sat;
  int val;
  uint32_t color;
  hue = hsb[0];
  sat = hsb[1];
  val = hsb[2];
  
  if (sat == 0) { // Acromatic color (gray). Hue doesn't mind.
  
    r   = val;
    g = val;
    b  = val; 
   
  } else  { 
    
    base = ((255 - sat) * val)>>8;
  
    switch(hue/60) {
	case 0:
		r = val;
		g = (((val-base)*hue)/60)+base;
		b = base;
	break;
	
	case 1:
		r = (((val-base)*(60-(hue%60)))/60)+base;
		g = val;
		b = base;
	break;
	
	case 2:
		r = base;
		g = val;
		b = (((val-base)*(hue%60))/60)+base;
	break;
	
	case 3:
		r = base;
		g = (((val-base)*(60-(hue%60)))/60)+base;
		b = val;
	break;
	
	case 4:
		r = (((val-base)*(hue%60))/60)+base;
		g = base;
		b = val;
	break;
	
	case 5:
		r = val;
		g = base;
		b = (((val-base)*(60-(hue%60)))/60)+base;
	break;
    }
      
    color = r;
    color <<= 8;
    color |= g;
    color <<= 8;
    color |= b;
  }   
  *c = color;
}




void RGBtoHSB(uint32_t c, int hsb[]) {
  byte r;
  byte g;
  byte b;
  byte minRGB;
  byte maxRGB;
  byte d;
  byte h;
  int hue;
  byte sat;
  byte val;
  
  b = color&0xFF;
  color >>= 8;
  g = color&0xFF;
  color >>= 8;
  r = color&0xFF;
  
//  Serial.print("red ");
//  Serial.println(r);
//  Serial.print("green ");
//  Serial.println(g);
//  Serial.print("blue ");
//  Serial.println(b);
  
  minRGB = min( r, min(g,b) );
  maxRGB = max( r, max(g,b) );
  
//  Serial.print("minRGB ");
//  Serial.println(minRGB);
//  Serial.print("maxRGB ");
//  Serial.println(maxRGB);
  
  if( minRGB==maxRGB ) {
    hue = 0;
    sat = 0;
    val = minRGB;
  }
  else {
    d = (r==minRGB) ? g-b : ( (b==minRGB) ? r-g : b-r );
//    Serial.print("d ");
//    Serial.println(d);
    h = (r==minRGB) ? 3 : ( (b==minRGB) ? 1 : 5 );
//    Serial.print("h ");
//    Serial.println(h);
    hue = 60*h - (60*d)/(maxRGB-minRGB);
    sat = 255*float(maxRGB-minRGB)/maxRGB;
    val = maxRGB;
  }
  
  hsb[0] = hue;
  hsb[1] = sat;
  hsb[2] = val;
}
