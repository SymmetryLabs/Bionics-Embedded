/*

    BEFORE THE REWORK!!!

    10:30am 10/24/2014 - Got Train compiling and called to as an inheritance from Animation class
    7pm                - Train runs with Basic Parameters, controllable from internal loops
*/

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
    while (!Serial); // wait for Leonardo enumeration, others continue immediately
    
    Serial.println(F("Bionic Framework Setup"));

    // Start xBee
    xbeeSetup();
    Serial.println(F("xBee Setup Complete"));
    delay(1000);

    // Setup LEDs
    LEDsetup();
    Serial.println(F("LED Setup Complete"));

    // Setup MPU
    MPUsetup();
    Serial.println(F("MPU Setup Complete"));

    Serial.println(F("Setup Complete"));
    Serial.println(F("----------"));
}



// ================================================================
// ===                    MAIN PROGRAM LOOP                     ===
// ================================================================

void loop() {

    Serial.print(F("State = "));
    Serial.println(state);

    switch ( state ) {

        case READ_SENSORS:
            MPUread();
            break;

        case ANIMATE:
            LEDrun();
            break;

        case COMMUNICATE:
            sendCommunications();
            // getCommunications();
            break;

    }

    state++;
}
