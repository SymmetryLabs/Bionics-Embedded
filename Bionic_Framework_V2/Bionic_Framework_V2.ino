// HAS TO BE AT TOP (for some reason...should figure out build order)
#include "BasicParameter.h"

#include "OSCBundle.h"

// Uncomment to disable printing to Serial
#define SERIAL_PRINTING
#define SERIAL_PRINTING2
// #define RX_PRINT_DATA_BYTES
 #define RX_PRINT_OSC_BYTES

// Uncomment to disable initial calibration of the MPU offsets
//#define ENABLE_CALIBRATION


// ***************    XBEE    *****************
// ***************    FLAGS   *****************

// #define SEND_INITIAL_TRANSMISSION
#define SEND_TRANSMISSION

const bool LIMIT_TRANSMISSION_RATE = false;
const byte transmissionPeriod = 15; // 30 -> ~30fps

// ********************************************
// ********************************************



// ***************  ANIMATION *****************
// ***************    FLAGS   *****************

#define STARTING_ANIMATION POWER
// #define AUTO_ANIMATION_CHANGER

const unsigned long animationSwitchPeriod = 180 * 1000;

// ********************************************
// ********************************************

// Select which unit
//#define UNIT_TRIP
#define UNIT_DEFAULT
// #define UNIT_ZERO



#ifdef SERIAL_PRINTING
    #define SERIAL_PRINT(x) Serial.print(x)
    #define SERIAL_PRINTLN(x) Serial.println(x)
#else
    #define SERIAL_PRINT(x)
    #define SERIAL_PRINTLN(x)
#endif

#ifdef SERIAL_PRINTING2
    #define SERIAL_PRINT2(x) Serial.print(x)
    #define SERIAL_PRINTLN2(x) Serial.println(x)
#else
    #define SERIAL_PRINT2(x)
    #define SERIAL_PRINTLN2(x)
#endif


// Initialize state changing variable
enum CommMsgType {
    ACC_R,
    ACC_P,
    GYR_R,
    GYR_P
};


// Initial stuff for LEDs and Animations
#include "FastLED.h"
#define NUM_LEDS 12

CRGB leds[NUM_LEDS];
CHSV ledsHSV[NUM_LEDS];

// Had to move this to main file for proper dependencies...
#include "Animation.h"



// Initial stuff for MPU
// #define DEBUG
#include "I2Cdev.h"
#include "MPU6050_6Axis_MotionApps20.h"
// Arduino Wire library is required if I2Cdev I2CDEV_ARDUINO_WIRE implementation
// is used in I2Cdev.h
#if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
    #include "Wire.h"
#endif

VectorInt16 aaReal;     // [x, y, z]            gravity-free accel sensor measurements
float ypr[3];           // [yaw, pitch, roll]   yaw/pitch/roll container and gravity vector


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
// ===                     MODEL VARIABLES                      ===
// ================================================================

float magLPF;
const float magLPFalpha = 0.8;

float rollLPF;
const float rollLPFalpha = 0.960;

float pitchLPF;
const float pitchLPFalpha = 0.960;

float magnitude;
float magnitudePercent;

float rollPercentP;
float pitchPercentP;


short int *model_acc_raw[3] = { &aaReal.x,
                        &aaReal.y,
                        &aaReal.z };

float *model_acc_processed[1] = { &magnitudePercent };

float *model_gyr_raw[3] = { &ypr[0],
                           &ypr[1],
                           &ypr[2] };

float *model_gyr_processed[2] = { &pitchPercentP,
                            &rollPercentP};




// ================================================================
// ===                    Animation SETUP                     ===
// ================================================================


#define NUM_ANIMATIONS 4

//Train train;
Fire fire;
Sparkle sparkle;
Power power;
DualPower dualpower;
//RunningRainbow runningrainbow;
// Noise noise;

// Initialize and list animation objects
Animation *animations[NUM_ANIMATIONS] = { &fire, &sparkle, &power, &dualpower };



enum AnimationState {
    FIRE,
    // TRAIN,
    SPARKLE,
    POWER,
    DUALPOWER
//    RUNNINGRAINBOW
    // NOISE
};

unsigned long timeOfLastAnimationChange = 0;

unsigned long lastAnimate = 0;

byte currentAnimation = STARTING_ANIMATION;




// ================================================================
// ===                        OSC SETUP                         ===
// ================================================================


// #include "OSCBundle.h"

OSCMessage osc_tx;
OSCMessage osc_rx;

class OSCBuffer : public Print {

  private:
    uint8_t *byteArray;
    size_t arraySize;
    size_t currentIndex = 0;

  public:

    OSCBuffer(uint8_t *_byteArray,size_t _arraySize) : byteArray(_byteArray), arraySize(_arraySize) {}

    size_t write(uint8_t b) {
      uint8_t *currentAddress = byteArray + currentIndex;
      *currentAddress = b;
      currentIndex++;
      return 1;
    }

    size_t write(uint8_t *bytes, size_t length) {
      while(length--) write(*bytes++);
      return 1;
    }

    void reset() {
      currentIndex = 0;
    }

};



// ================================================================
// ===                    XBEE SETUP                     ===
// ================================================================


#include "XBee.h"

// xBee variables
XBee xbee = XBee();

#define MAX_PACKED_DATA 100
uint8_t packed_data[MAX_PACKED_DATA];
uint8_t packed_data_length;

OSCBuffer oscbuffer(packed_data, MAX_PACKED_DATA);

/*
Transmission variables
Tx16Request(uint16_t addr16, uint8_t option, uint8_t *data, uint8_t dataLength, uint8_t frameId)

OPTIONS (whether or not local radio retries failed tranmissions)
    #define ACK_OPTION 0
    #define DISABLE_ACK_OPTION 1
    #define BROADCAST_OPTION 4

FRAME ID (whether or not target radio sends confirmation)
    #define DEFAULT_FRAME_ID 1
    #define NO_RESPONSE_FRAME_ID 0
*/

// xBee TX options
#define ACK_OPTION 0
#define DISABLE_ACK_OPTION 1
#define BROADCAST_OPTION 4

// Everything currently sent for no retries, no ACK
const uint16_t ADDRESS_COORDINATOR = 0x0001;
Tx16Request tx = Tx16Request( ADDRESS_COORDINATOR, DISABLE_ACK_OPTION, packed_data, sizeof(packed_data), NO_RESPONSE_FRAME_ID );
TxStatusResponse txStatus = TxStatusResponse();
unsigned long timeOfLastTransmission = 0;;


// Receiving variables
XBeeResponse response = XBeeResponse(); 
Rx16Response rx16 = Rx16Response(); // create reusable response objects for responses we expect to handle



float eq[] = {0, 0};

void eqControl(OSCMessage &_msg) {
    SERIAL_PRINTLN2("eqControl called back");

    // Get size of message
    uint8_t numData = _msg.size();

    // Load data entries into eq array
    for( uint8_t dataIndex=0; dataIndex < numData; dataIndex++ ) {
        if ( _msg.isFloat(dataIndex) ) {
            eq[dataIndex] = _msg.getFloat(dataIndex);
        }
        else {
            SERIAL_PRINT2("eqControl ERROR with dataIndex = ");
            SERIAL_PRINTLN2(dataIndex);
        }
    }
}

void midiControl( OSCMessage &_msg ) {
    SERIAL_PRINTLN2("midiControl called back");
    // data = [ int channel, int number, int value ]
    // channel = 2 for rotary
    // number = which knob
    // value -> 0-127

    enum MidiFighterData {
        PROPERTY = 0,
        KNOB = 1,
        VALUE = 2
    };

    enum MidiFighterProperty {
        SWITCH = 1,
        ROTARY = 0
    };

    // Only deal with rotary for now
    if ( _msg.getInt(PROPERTY) == ROTARY ) {
//      if ( _msg.getInt(0) == 2 ) {

        uint8_t knob = _msg.getInt(KNOB);
        SERIAL_PRINT2("knob = "); SERIAL_PRINTLN2(knob);

        uint8_t value = _msg.getInt(VALUE);
        SERIAL_PRINT2("value = "); SERIAL_PRINTLN2(value);
        
        // Select which animation parameter with NUMBER
        if ( knob < 4 ) animations[ currentAnimation ]->parameters[ knob ]->setPercent( float(value)/127. );

    }

}


// ================================================================
// ===                      INITIAL SETUP                       ===
// ================================================================

void setup() {

    // Begin Serial
    Serial.begin(115200);
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

    // Start xBee
    SERIAL_PRINTLN("--");
    xbeeSetup();
    SERIAL_PRINTLN(F("--xBee Setup Complete")); SERIAL_PRINTLN();

    // Setup MPU
    SERIAL_PRINTLN("--");
    LEDS.showColor(CRGB::Red);
    MPUsetup();
    LEDS.showColor(CRGB::Black);
    SERIAL_PRINTLN(F("--MPU Setup Complete")); SERIAL_PRINTLN();

    #ifdef AUTO_ANIMATION_CHANGER
        SERIAL_PRINTLN("IMPORTANT!!!  Automatic Animation Changer ENABLED");
    #endif


    SERIAL_PRINTLN(F("SETUP Complete"));
    SERIAL_PRINTLN(F("--------------------"));
    SERIAL_PRINTLN(F("--------------------")); SERIAL_PRINTLN();
    LEDS.showColor(CRGB::Green);
    delay(250);
    LEDS.showColor(CRGB::Black);
    delay(250);
    LEDS.showColor(CRGB::Green);
    delay(250);
    LEDS.showColor(CRGB::Black);
    delay(250);


    // Initialize timers
    timeOfLastTransmission = millis();
    timeOfLastAnimationChange = millis();
}



// ================================================================
// ===                    MAIN PROGRAM LOOP                     ===
// ================================================================

void loop() {

    SERIAL_PRINTLN(F("Run loop"));

    switch ( state ) {

        case READ_SENSORS:
            SERIAL_PRINTLN("----------");
            SERIAL_PRINTLN("Start STATE MACHINE");
            MPUread();
            break;

        case ANIMATE:
            SERIAL_PRINTLN("-----");
            LEDrun();
            SERIAL_PRINT("Animation state = "); SERIAL_PRINTLN(currentAnimation);
            break;

        case COMMUNICATE:
            SERIAL_PRINTLN("-----");
            
            // LOAD INTO MESSAGE STRUCTURES?
            // Loop internally to collect as many messages as were sent? Maybe not necessary anymore...
           getCommunications();
            
            #ifdef SEND_TRANSMISSION
                SERIAL_PRINTLN("Sending transmission!");

                unsigned long timeSinceLastTransmission = millis() - timeOfLastTransmission;
                if ( timeSinceLastTransmission > transmissionPeriod | !LIMIT_TRANSMISSION_RATE ) {

                    // List the desired things to send here
                    // *model_acc -> /m/acc/r
                    // *model_acc_processed -> /m/acc/p
                    // *model_gyro -> /m/gyr/r
                    // *model_gyro_processing -> /m/gyr/p
                    // Assume everything is a float
                    // packOSC( dataDictionary, lengthDictionary )
                    
                    sendCommunications_Report( ACC_R );


                    // SELECT HERE WHICH COMMUNICATIONS TO SEND
                    // WANT TO BE ABLE TO SWTICH BETWEEN: magnitude, all acc, all gyro, everything together
                    // Should I send a bundle if we do multiple?
                    // Basically need a main function that handle loading the bundle / message
                    // Give it consistent information on what to load
                    // Address and data pairs
                    // Mapping function from what's desired to the appropriate addresses

                    timeOfLastTransmission = millis();
                }
            #endif
            

            SERIAL_PRINTLN("----------");
            SERIAL_PRINTLN();
            break;

    }

    state++;


    // FOR TESTING - Animation changing timer
    #ifdef AUTO_ANIMATION_CHANGER
        unsigned long timeSinceLastAnimationChange = millis() - timeOfLastAnimationChange;
        if ( timeSinceLastAnimationChange > animationSwitchPeriod ) {
            SERIAL_PRINTLN("ANIMATION CHANGE!");
            currentAnimation = (currentAnimation + 1) % NUM_ANIMATIONS;
            timeOfLastAnimationChange = millis();
        }
    #endif

    // Delay if you want to slow down the effective framerate of the unit
    // delay(20);

}
