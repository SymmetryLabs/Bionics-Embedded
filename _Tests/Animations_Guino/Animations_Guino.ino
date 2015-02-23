// HAS TO BE AT TOP (for some reason...should figure out build order)
#include "BasicParameter.h"

// Initial stuff for LEDs and Animations
#include "FastLED.h"
#define NUM_LEDS 12
CRGB leds[NUM_LEDS];
CHSV ledsHSV[NUM_LEDS];

// HAS TO BE AFTER DEFINITION OF FASTLED
#include "Animation.h"

#include "EEPROM.h"
#include "EasyTransfer.h"
#include "Guino.h"


// Uncomment to disable printing to Serial
//#define SERIAL_PRINTING
#ifdef SERIAL_PRINTING
    #define SERIAL_PRINT(x) Serial.print(x)
    #define SERIAL_PRINTLN(x) Serial.println(x)
    #define SERIAL_BEGIN(x) Serial.begin(x)
#else
    #define SERIAL_PRINT(x)
    #define SERIAL_PRINTLN(x)
    #define SERIAL_BEGIN(x)
#endif



// ================================================================
// ===                    Animation SETUP                     ===
// ================================================================

// ********************************************
// ***************    FLAGS   *****************

#define STARTING_ANIMATION SPARKLE
 // #define AUTO_ANIMATION_CHANGER

const unsigned long animationSwitchPeriod = 180 * 1000;

// ********************************************
// ********************************************



#define NUM_ANIMATIONS 8
Train train;
Fire fire;
Sparkle sparkle;
Power power;
DualPower dualpower;
RunningRainbow runningrainbow;
Noise noise;
Rain rain;

// Initialize and list animation objects
Animation *animations[NUM_ANIMATIONS] = { &train, &fire, &sparkle, &power, &dualpower, &runningrainbow, &noise, &rain };



enum AnimationState {
    FIRE,
    TRAIN,
    SPARKLE,
    POWER,
    DUALPOWER,
    RUNNINGRAINBOW,
    NOISE,
    RAIN
};

unsigned long timeOfLastAnimationChange = 0;
unsigned long lastAnimate = 0;

int16_t currentAnimation = STARTING_ANIMATION;


int16_t fps = 60;
unsigned long frameDuration = 1000/fps;
int16_t decay = 1;



// ================================================================
// ===                      INITIAL SETUP                       ===
// ================================================================

void setup() {
    gBegin(12347);

    // Begin Serial
    SERIAL_BEGIN(115200);
    // while (!Serial); // wait for Leonardo enumeration, others continue immediately

    delay(1000);
    
    SERIAL_PRINTLN();
    SERIAL_PRINTLN();
    SERIAL_PRINTLN(F("--------------------"));
    SERIAL_PRINTLN(F("--------------------"));
    SERIAL_PRINTLN(F("BIONIC FRAMEWORK START"));
    SERIAL_PRINTLN(F("--------------------"));
    SERIAL_PRINTLN(F("--------------------"));
    SERIAL_PRINTLN();

    SERIAL_PRINTLN(F("Bionic Framework SETUP")); SERIAL_PRINTLN();

    // Setup LEDs
    SERIAL_PRINTLN("--");
    LEDsetup();
    SERIAL_PRINTLN(F("--LED Setup Complete")); SERIAL_PRINTLN();

    #ifdef AUTO_ANIMATION_CHANGER
        SERIAL_PRINTLN("IMPORTANT!!!  Automatic Animation Changer ENABLED");
    #endif

    SERIAL_PRINTLN(F("SETUP Complete"));
    SERIAL_PRINTLN(F("--------------------"));
    SERIAL_PRINTLN(F("--------------------")); SERIAL_PRINTLN();


    // Initialize timers
    timeOfLastAnimationChange = millis();
}



// ================================================================
// ===                    MAIN PROGRAM LOOP                     ===
// ================================================================

void loop() {
    guino_update();
    frameDuration = 1000/fps;
    animations[currentAnimation]->decay_Parameter.setPercent(float(decay)/255.);

    SERIAL_PRINTLN(F("Run loop"));
    SERIAL_PRINTLN("-----");

    unsigned long deltaMs = millis() - lastAnimate;
    if ( deltaMs > frameDuration ) {
        SERIAL_PRINT("Animation state = "); SERIAL_PRINTLN(currentAnimation);

        // Calculate deltaTime since last animate (animations are time dependent)
        // This allows for non-fixed framerate running - the Teensy does its best
        SERIAL_PRINT("dT Animation "); SERIAL_PRINTLN(deltaMs);
        LEDrun( deltaMs );
        lastAnimate = millis();
    }


    // FOR TESTING - Animation changing timer
    #ifdef AUTO_ANIMATION_CHANGER
        unsigned long timeSinceLastAnimationChange = millis() - timeOfLastAnimationChange;
        if ( timeSinceLastAnimationChange > animationSwitchPeriod ) {
            SERIAL_PRINTLN("ANIMATION CHANGE!");
            currentAnimation = (currentAnimation + 1) % NUM_ANIMATIONS;
            timeOfLastAnimationChange = millis();
        }
    #endif

    SERIAL_PRINTLN("-----");
    SERIAL_PRINTLN();

}



// ================================================================
// ===                     GUINO FUNCTIONS                      ===
// ================================================================

byte fpsID;
byte currentAnimationID;
byte decayID;

// This is where you setup your interface 
void gInit()
{
    fpsID = gAddSlider(2, 60,"FPS", &fps);
    currentAnimationID = gAddSlider(0, NUM_ANIMATIONS-1,"Animation Selector", &currentAnimation);
    decayID = gAddSlider(1, 255,"Decay", &decay);

    gAddColumn();

    gAddLabel("Noise",1);
    gAddSpacer(1);
    gAddSlider( int16_t(noise.scale_Parameter.getMin()), int16_t(noise.scale_Parameter.getMax()), "Noise - Scale", &noise.scale );
    gAddSlider( int16_t(noise.speed_Parameter.getMin()), int16_t(noise.speed_Parameter.getMax()), "Noise - Speed", &noise.speedy );
  // gAddSlider( 0, INT16_MAX, "LPF Alpha", &LPFint);
  
  // gAddColumn();

  // // Last parameter in moving graph defines the size 10 = normal
  // gAddMovingGraph("Raw Magnitude", 0, INT16_MAX, &magRaw, 20);
  // gAddMovingGraph("LPF Magnitude", 0,INT16_MAX, &magLPF, 20);
}


// Method called everytime a button has been pressed in the interface.
void gButtonPressed(int16_t id)
{
 
}

void gItemUpdated(int16_t id)
{

}
