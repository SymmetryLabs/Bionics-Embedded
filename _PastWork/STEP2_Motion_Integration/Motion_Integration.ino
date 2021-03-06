/*
    10:30am 10/24/2014 - Got Train compiling and called to as an inheritance from Animation class
    7pm                - Train runs with Basic Parameters, controllable from internal loops
    9:35pm             - Integrated Sparkle with FastLED dimming to best abilities...no good, need to write own
    2PM     10/26/2014 - Several animations running great.  Fixed dimming.  Moving on to xBee integration.  Several sensor running.
*/
#include "FastLED.h"
#define NUM_LEDS 12

CRGB leds[NUM_LEDS];
CHSV ledsHSV[NUM_LEDS];


#include "I2Cdev.h"
#include "MPU6050_6Axis_MotionApps20.h"
// Arduino Wire library is required if I2Cdev I2CDEV_ARDUINO_WIRE implementation
// is used in I2Cdev.h
#if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
    #include "Wire.h"
#endif

VectorInt16 aaReal;     // [x, y, z]            gravity-free accel sensor measurements
float ypr[3];           // [yaw, pitch, roll]   yaw/pitch/roll container and gravity vector



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
// ===                      INITIAL SETUP                       ===
// ================================================================

void setup() {

    // Begin Serial
    Serial.begin(115200);
    // while (!Serial); // wait for Leonardo enumeration, others continue immediately
    
    Serial.println(F("Bionic Framework Setup"));

    // Setup MPU
    MPUsetup();
    Serial.println(F("MPU Setup Complete"));

    // Setup LEDs
    LEDsetup();
    Serial.println(F("LED Setup Complete"));


    Serial.println(F("Setup Complete"));
    Serial.println(F("----------"));
}



// ================================================================
// ===                    MAIN PROGRAM LOOP                     ===
// ================================================================

void loop() {

    // Serial.println(F("Run loop"));

    switch ( state ) {

        case READ_SENSORS:
            MPUread();
            break;

        case ANIMATE:
            LEDrun();
            break;

        case COMMUNICATE:
            // sendCommunications();
            // getCommunications();
            break;

    }

    state++;

    // delay(10);

}
