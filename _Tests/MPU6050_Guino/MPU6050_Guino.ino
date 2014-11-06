 #include "EEPROM.h"
 #include "EasyTransfer.h"
#include "Guino.h"

// I2Cdev and MPU6050 must be installed as libraries, or else the .cpp/.h files
// for both classes must be in the include path of your project
#include "I2Cdev.h"

#include "MPU6050_6Axis_MotionApps20.h"
//#include "MPU6050.h" // not necessary if using MotionApps include file

// Arduino Wire library is required if I2Cdev I2CDEV_ARDUINO_WIRE implementation
// is used in I2Cdev.h
#if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
    #include "Wire.h"
#endif


MPU6050 mpu;


#ifdef SERIAL_PRINTING
    #define SERIAL_PRINT(x) Serial.print(x)
    #define SERIAL_PRINTLN(x) Serial.println(x)
    #define SERIAL_BEGIN(x) Serial.begin(x)
#else
    #define SERIAL_PRINT(x)
    #define SERIAL_PRINTLN(x)
    #define SERIAL_BEGIN(x)
#endif

#define OUTPUT_CUSTOM


// MPU control/status vars
bool dmpReady = false;  // set true if DMP init was successful

uint8_t mpuIntStatus;   // holds actual interrupt status byte from MPU
uint8_t devStatus;      // return status after each device operation (0 = success, !0 = error)
uint16_t packetSize;    // expected DMP packet size (default is 42 bytes)
uint16_t fifoCount;     // count of all bytes currently in FIFO
uint8_t fifoBuffer[64]; // FIFO storage buffer

// orientation/motion vars
Quaternion quat;           // [w, x, y, z]         quaternion container
VectorInt16 aa;         // [x, y, z]            accel sensor measurements
VectorInt16 aaReal;     // [x, y, z]            gravity-free accel sensor measurements
VectorInt16 aaWorld;    // [x, y, z]            world-frame accel sensor measurements
VectorFloat gravity;    // [x, y, z]            gravity vector
float euler[3];         // [psi, theta, phi]    Euler angle container
float ypr[3];           // [yaw, pitch, roll]   yaw/pitch/roll container and gravity vector

int16_t magLPF = 0;
int16_t magRaw = 0;
float LPFalpha = 0.960;
int16_t LPFint = LPFalpha * INT16_MAX;


// ================================================================
// ===                      INITIAL SETUP                       ===
// ================================================================

void setup() {
    gBegin(12346);


    // join I2C bus (I2Cdev library doesn't do this automatically)
    #if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
        Wire.begin();
        TWBR = 24; // 400kHz I2C clock (200kHz if CPU is 8MHz)
    #elif I2CDEV_IMPLEMENTATION == I2CDEV_BUILTIN_FASTWIRE
        Fastwire::setup(400, true);
    #endif

    // initialize serial communication
    // (115200 chosen because it is required for Teapot Demo output, but it's
    // really up to you depending on your project)
    SERIAL_BEGIN(115200);
    // while (!Serial); // wait for Leonardo enumeration, others continue immediately

    // initialize device
    SERIAL_PRINTLN(F("Initializing I2C devices..."));
    mpu.initialize();

    // verify connection
    SERIAL_PRINTLN(F("Testing device connections..."));
    SERIAL_PRINTLN(mpu.testConnection() ? F("MPU6050 connection successful") : F("MPU6050 connection failed"));

    // wait for ready
    // SERIAL_PRINTLN(F("\nSend any character to begin DMP programming and demo: "));
    // while (Serial.available() && Serial.read()); // empty buffer
    // while (!Serial.available());                 // wait for data
    // while (Serial.available() && Serial.read()); // empty buffer again

    // load and configure the DMP
    SERIAL_PRINTLN(F("Initializing DMP..."));
    devStatus = mpu.dmpInitialize();

    // supply your own gyro offsets here, scaled for min sensitivity
    mpu.setXGyroOffset(220);
    mpu.setYGyroOffset(76);
    mpu.setZGyroOffset(-85);
    mpu.setZAccelOffset(1788); // 1688 factory default for my test chip

    // make sure it worked (returns 0 if so)
    if (devStatus == 0) {
        // turn on the DMP, now that it's ready
        SERIAL_PRINTLN(F("Enabling DMP..."));
        mpu.setDMPEnabled(true);

        mpuIntStatus = mpu.getIntStatus();

        // set our DMP Ready flag so the main loop() function knows it's okay to use it
        SERIAL_PRINTLN(F("DMP ready! Waiting for first interrupt..."));
        dmpReady = true;

        // get expected DMP packet size for later comparison
        packetSize = mpu.dmpGetFIFOPacketSize();
    } else {
        // ERROR!
        // 1 = initial memory load failed
        // 2 = DMP configuration updates failed
        // (if it's going to break, usually the code will be 1)
        SERIAL_PRINT(F("DMP Initialization failed (code "));
        SERIAL_PRINT(devStatus);
        SERIAL_PRINTLN(F(")"));
    }
}



// ================================================================
// ===                    MAIN PROGRAM LOOP                     ===
// ================================================================

void loop() {
    // **** Main update call for the guino
    guino_update();
    LPFalpha = float(LPFint) / float(INT16_MAX);

    // if programming failed, don't try to do anything
    if (!dmpReady) return;

    // reset interrupt flag and get INT_STATUS byte
    mpuIntStatus = mpu.getIntStatus();
    // SERIAL_PRINT("IntStatus ");
    // SERIAL_PRINTLN(mpuIntStatus, HEX);

    // get current FIFO count
    fifoCount = mpu.getFIFOCount();
    // SERIAL_PRINT(F("FIFO Count "));
    // SERIAL_PRINTLN(fifoCount);

    // check for overflow (this should never happen unless our code is too inefficient)
    if ((mpuIntStatus & 0x10) || fifoCount == 1024) {
        // reset so we can continue cleanly
        SERIAL_PRINTLN(F("Eep!"));
        mpu.resetFIFO();

    // otherwise, check for DMP data ready interrupt (this should happen frequently)
    } else if (mpuIntStatus & 0x01) {

        // wait for correct available data length, should be a VERY short wait
        while (fifoCount < packetSize) {
            fifoCount = mpu.getFIFOCount();
            // SERIAL_PRINT(F("Wait - FIFO Count "));
            // SERIAL_PRINTLN(fifoCount);
        }

        while (fifoCount >= packetSize){

            // read a packet from FIFO
            mpu.getFIFOBytes(fifoBuffer, packetSize);
            
            // track FIFO count here in case there is > 1 packet available
            // (this lets us immediately read more without waiting for an interrupt)
            fifoCount -= packetSize;
        }
        #ifdef OUTPUT_CUSTOM
            // display real acceleration, adjusted to remove gravity
            mpu.dmpGetQuaternion(&quat, fifoBuffer);
            mpu.dmpGetAccel(&aa, fifoBuffer);
            mpu.dmpGetGravity(&gravity, &quat);
            mpu.dmpGetLinearAccel(&aaReal, &aa, &gravity);
            SERIAL_PRINT("areal\t");
            SERIAL_PRINT(aaReal.x);
            SERIAL_PRINT("\t");
            SERIAL_PRINT(aaReal.y);
            SERIAL_PRINT("\t");
            SERIAL_PRINTLN(aaReal.z);
            mpu.dmpGetYawPitchRoll(ypr, &quat, &gravity);
            SERIAL_PRINT("ypr\t");
            SERIAL_PRINT(ypr[0] * 180/M_PI);
            SERIAL_PRINT("\t");
            SERIAL_PRINT(ypr[1] * 180/M_PI);
            SERIAL_PRINT("\t");
            SERIAL_PRINTLN(ypr[2] * 180/M_PI);
        #endif
    }

    delay(20);
    
    magRaw = aaReal.getMagnitude();
    magLPF = LPFalpha * magRaw + magLPF * (1-LPFalpha);
    gUpdateValue(&magRaw);
    gUpdateValue(&magLPF);
    
}



// This is where you setup your interface 
void gInit()
{
  gAddLabel("GRAPHS",1);
  gAddSpacer(1);
  gAddSlider( 0, INT16_MAX, "LPF Alpha", &LPFint);
  
  gAddColumn();

  // Last parameter in moving graph defines the size 10 = normal
  gAddMovingGraph("Raw Magnitude", 0, INT16_MAX, &magRaw, 20);
  gAddMovingGraph("LPF Magnitude", 0,INT16_MAX, &magLPF, 20);
}


// Method called everytime a button has been pressed in the interface.
void gButtonPressed(int16_t id)
{
 
}

void gItemUpdated(int16_t id)
{

}
