// HAS TO BE AT TOP (for some reason...should figure out build order)
#include "BasicParameter.h"

#include "FastLED.h"
#define NUM_LEDS 12

CRGB leds[NUM_LEDS];
CHSV ledsHSV[NUM_LEDS];

//#include <iostream>
//#include <fstream>
//#include <vector>
//using namespace std;

// Had to move this to main file for proper dependencies...
#include "Animation.h"

// Initialize state changing variable
enum STATES {
    READ_SENSORS,
    ANIMATE,
    COMMUNICATE
};

inline STATES& operator++(STATES& _state, int)  // <--- note -- must be a reference
{
   const int i = static_cast<int>(_state);
   _state = static_cast<STATES>((i + 1) % 3);
   return _state;
}

enum STATES state = READ_SENSORS;




// ================================================================
// ===                    Animation SETUP                     ===
// ================================================================

// ********************************************
// ***************    FLAGS   *****************

#define STARTING_ANIMATION POWER
#define AUTO_ANIMATION_CHANGER

const unsigned long animationSwitchPeriod = 5 * 1000;

// ********************************************
// ********************************************




//const byte NUM_ANIMATIONS = 3;
#define NUM_ANIMATIONS 3

Sparkle sparkle;
Power power;
RunningRainbow runningrainbow;


// Initialize and list animation objects
Animation *animations[NUM_ANIMATIONS] = { &sparkle, &power, &runningrainbow }; // Does not work




enum AnimationState {
    SPARKLE,
    POWER,
    RUNNINGRAINBOW
};

unsigned long timeOfLastAnimationChange = 0;

unsigned long lastAnimate = 0;

byte currentAnimation = STARTING_ANIMATION;





//vector<Animation*> animations;
//animations.push_back ( new Sparkle() );

//typedef std::vector< unique_ptr<Animation>, NUM_ANIMATIONS > Animations;
//Animations animations = {{ new Sparkle, new Power, new RunningRainbow }};

//Animation *anims[NUM_ANIMATIONS] = { nullptr, nullptr, nullptr }; // Does not work
//animations[0] = &sparkle;
//animations[1] = &powerB;
//animations[2] = &runningrainbow;
//animations[0] = new Sparkle();
//animations[1] = new Power();
//animations[2] = new RunningRainbow();

// ================================================================
// ===                      INITIAL SETUP                       ===
// ================================================================

void setup() {

    // Begin Serial
    Serial.begin(115200);
    // while (!Serial); // wait for Leonardo enumeration, others continue immediately

    LEDS.setBrightness(255);

    // Put ws2801 strip on the hardware SPI pins with a BGR ordering of rgb and limited to a 1Mhz data rate
    // LEDS.addLeds<WS2801, 11, 13, BGR, DATA_RATE_MHZ(1)>(leds, NUM_LEDS);
    LEDS.addLeds<WS2801, 11, 13, RGB, DATA_RATE_MHZ(1)>(leds, NUM_LEDS);

    delay(1000);

    timeOfLastAnimationChange = millis();
}


// ================================================================
// ===                    MAIN PROGRAM LOOP                     ===
// ================================================================

unsigned long deltaMs = 15;

void loop() {
    Serial.print("anim 0 decay = "); Serial.println(sparkle.decay_Parameter.getValue());
    Serial.print("anim 1 decay = "); Serial.println(power.decay_Parameter.getValue());
    Serial.print("anim 2 decay = "); Serial.println(runningrainbow.decay_Parameter.getValue());

    Serial.println();
    Serial.print("currentAnimation = "); Serial.println(currentAnimation);

    float currentDecay = animations[currentAnimation]->decay_Parameter.getValue();
    Serial.print("currentAnimation decay = "); Serial.println(currentDecay);

    animations[currentAnimation]->draw( deltaMs );

    if ( millis()-timeOfLastAnimationChange > 1000 ) {
      currentAnimation++;
      timeOfLastAnimationChange = millis();
    }
    if ( currentAnimation > 2 ) currentAnimation = 0;
    
    delay(100);
}
