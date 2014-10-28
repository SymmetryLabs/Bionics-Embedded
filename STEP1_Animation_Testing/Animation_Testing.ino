/*
    10:30am 10/24/2014 - Got Train compiling and called to as an inheritance from Animation class
    7pm                - Train runs with Basic Parameters, controllable from internal loops
*/

#include "FastLED.h"

#define NUM_LEDS 24
CRGB leds[NUM_LEDS];


// ================================================================
// ===                      INITIAL SETUP                       ===
// ================================================================

void setup() {

    // Begin Serial
    Serial.begin(115200);
    while (!Serial); // wait for Leonardo enumeration, others continue immediately
    
    Serial.println(F("Bionic Framework Setup"));

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

    Serial.println(F("Run loop"));

    LEDrun();

    delay(10);

}
