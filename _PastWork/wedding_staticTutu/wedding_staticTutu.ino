#include "SPI.h"
#include "Adafruit_WS2801.h"

#include "Timer.h"


int dataPin  = 4;
int clockPin = 5;

const int stripLength = 32;
Adafruit_WS2801 strip = Adafruit_WS2801(stripLength, dataPin, clockPin);

int hue[stripLength];
byte sat[stripLength];
byte val[stripLength];

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



Timer sampleAndAnimate_timer;
byte animateTimerIndex;


int counter1 = 0;
int counter2 = 0;

byte state = 1;

// ================================================================
// ===               INTERRUPT DETECTION ROUTINE                ===
// ================================================================

void sample() {

}

void convertAndAnimate() {
  switch (state) {
    case 1:
      animation_rainbowDualFillContinuous();
      break;
    case 2:
      animation_fadeSparkle();
      break;
    case 3:
      Serial.println("Rainbow!");
      animation_rainbow();
      break;
    case 4:
      animation_spin();
      break;
  }
}

void stateChange() {
  if (state==3) state = 1;
  else state++;
}





void animation_rainbow() {
  if ( sampleAndAnimate_timer._events[animateTimerIndex].period != 30 ) sampleAndAnimate_timer._events[animateTimerIndex].period = 30;
  int i;
  if (counter1 < 256) counter1++;
  else counter1 = 0;
  
  for (i=0; i < strip.numPixels(); i++) {
    strip.setPixelColor(i, Wheel( (i + counter1) % 255));
  }  
  strip.show();   // write all the pixels out
}


void animation_rainbowDualFill() {
  if ( sampleAndAnimate_timer._events[animateTimerIndex].period != 90 ) sampleAndAnimate_timer._events[animateTimerIndex].period = 90;
  int i;
  
  counter2++;
  if ( counter2 >= strip.numPixels()/2 ) {
    counter2= 0;
    counter1+=147;
    if (counter1 > 256) counter1 -= 256;
  }
  strip.setPixelColor( (strip.numPixels()/2-1) - counter2 , Wheel(counter1));
  strip.setPixelColor( (strip.numPixels()/2) + counter2, Wheel(counter1));
  strip.show();   // write all the pixels out
}


void animation_rainbowDualFillContinuous() {
  if ( sampleAndAnimate_timer._events[animateTimerIndex].period != 90 ) sampleAndAnimate_timer._events[animateTimerIndex].period = 90;
  int i;
  
  counter2++;
  if ( counter2 >= strip.numPixels() ) counter2 = 0;
  if ( counter2 == 0 || counter2 == strip.numPixels()/2 ) counter1+=147;
  if (counter1 > 256) counter1 -= 256;
  
  if ( counter2 < strip.numPixels()/2 ) {
    strip.setPixelColor( (strip.numPixels()/2-1) - counter2 , Wheel(counter1));
    strip.setPixelColor( (strip.numPixels()/2) + counter2, Wheel(counter1));
  }
  else {
    strip.setPixelColor( counter2 - strip.numPixels()/2 , Wheel(counter1));
//    Serial.println((counter2 - strip.numPixels()/2));
    strip.setPixelColor( (1.5*strip.numPixels()-1) - counter2 , Wheel(counter1));
//    Serial.println((1.5*strip.numPixels()-1) - counter2);
  }
  strip.show();   // write all the pixels out
}


void animation_spin() {
  if ( sampleAndAnimate_timer._events[animateTimerIndex].period != 30 ) sampleAndAnimate_timer._events[animateTimerIndex].period = 30;
  int i;
  
  counter2++;
  if ( counter2 >= strip.numPixels() ) counter2 = 0;
  counter1+=27;
  if (counter1 > 256) counter1 -= 256;
  if ( counter2 == 0 ) {
    
    
    for (int j=0; j<strip.numPixels();j++) strip.setPixelColor(j,Color(0,0,0));
    for (int j=0; j<2; j++) strip.setPixelColor(j,Wheel(counter1));
  }
  else {
    strip.setPixelColor(strip.numPixels()-1,strip.getPixelColor(0));
    for (int i = 0; i<strip.numPixels(); i++) strip.setPixelColor(i,strip.getPixelColor(i+1));
  }
  strip.show();   // write all the pixels out
}


void animation_fadeSparkle() {
  if ( sampleAndAnimate_timer._events[animateTimerIndex].period != 20 ) sampleAndAnimate_timer._events[animateTimerIndex].period = 20;
  
  byte pixelIndex = -1;
  
  counter1++;
  if (counter1>5) {
    counter1=0;
    pixelIndex = random( 0, strip.numPixels()-1 );
    val[pixelIndex] = random( 50, 255 );
  }
  
  for(int i=0; i<strip.numPixels(); i++) {
    if( i != pixelIndex) val[i] = constrain(val[i]-2, 0, 255);
    setPixelHSB(i);
  }
  strip.show();
}



// ================================================================
// ===                      INITIAL SETUP                       ===
// ================================================================

void setup() {

    Serial.begin(9600);

    
    strip.begin();
    strip.show();
    
    sampleAndAnimate_timer.every( 10, sample );
    animateTimerIndex = sampleAndAnimate_timer.every( 20 , convertAndAnimate );
    
    sampleAndAnimate_timer.every(20000, stateChange);
    
    for ( int i=0; i < stripLength; i++ ) {
      hue[i] = 0;
      sat[i] = 255;
      val[i] = 255;
      setPixelHSB(i);
    }
    
}






// ================================================================
// ===                    MAIN PROGRAM LOOP                     ===
// ================================================================

void loop() {
  
  sampleAndAnimate_timer.update();

}





// ================================================================
// ===                      COLOR FUNCTIONS                     ===
// ================================================================

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

//Input a value 0 to 255 to get a color value.
//The colours are a transition r - g -b - back to r
uint32_t Wheel(byte WheelPos)
{
  if (WheelPos < 85) {
   return Color(WheelPos * 3, 255 - WheelPos * 3, 0);
  } else if (WheelPos < 170) {
   WheelPos -= 85;
   return Color(255 - WheelPos * 3, 0, WheelPos * 3);
  } else {
   WheelPos -= 170; 
   return Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
}


void setPixelHSB(int i) {
  byte valAdj = dc[val[i]];
  int hsb[3] = {hue[i], sat[i], valAdj};
  uint32_t color;
  HSBtoRGB(hsb, &color);
  strip.setPixelColor(i, color);
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




void RGBtoHSB(uint32_t color, int hsb[]) {
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
