// I2C device class (I2Cdev) demonstration Arduino sketch for MPU6050 class using DMP (MotionApps v2.0)
// 2013-05-08 by Jeff Rowberg <jeff@rowberg.net>

// #include "I2Cdev.h"
// #include "MPU6050_6Axis_MotionApps20.h"
// // Arduino Wire library is required if I2Cdev I2CDEV_ARDUINO_WIRE implementation
// // is used in I2Cdev.h
// #if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
//     #include "Wire.h"
// #endif

MPU6050 mpu;


// uncomment "OUTPUT_READABLE_QUATERNION" if you want to see the actual
// quaternion components in a [w, x, y, z] format (not best for parsing
// on a remote host such as Processing or something though)
//#define OUTPUT_READABLE_QUATERNION

// uncomment "OUTPUT_READABLE_EULER" if you want to see Euler angles
// (in degrees) calculated from the quaternions coming from the FIFO.
// Note that Euler angles suffer from gimbal lock (for more info, see
// http://en.wikipedia.org/wiki/Gimbal_lock)
//#define OUTPUT_READABLE_EULER

// uncomment "OUTPUT_READABLE_YAWPITCHROLL" if you want to see the yaw/
// pitch/roll angles (in degrees) calculated from the quaternions coming
// from the FIFO. Note this also requires gravity vector calculations.
// Also note that yaw/pitch/roll angles suffer from gimbal lock (for
// more info, see: http://en.wikipedia.org/wiki/Gimbal_lock)
// #define OUTPUT_READABLE_YAWPITCHROLL

// uncomment "OUTPUT_READABLE_REALACCEL" if you want to see acceleration
// components with gravity removed. This acceleration reference frame is
// not compensated for orientation, so +X is always +X according to the
// sensor, just without the effects of gravity. If you want acceleration
// compensated for orientation, us OUTPUT_READABLE_WORLDACCEL instead.
// #define OUTPUT_READABLE_REALACCEL

// uncomment "OUTPUT_READABLE_WORLDACCEL" if you want to see acceleration
// components with gravity removed and adjusted for the world frame of
// reference (yaw is relative to initial orientation, since no magnetometer
// is present in this case). Could be quite handy in some cases.
//#define OUTPUT_READABLE_WORLDACCEL

// uncomment "OUTPUT_TEAPOT" if you want output that matches the
// format used for the InvenSense teapot demo
//#define OUTPUT_TEAPOT

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
// VectorInt16 aaReal;     // [x, y, z]            gravity-free accel sensor measurements
VectorInt16 aaWorld;    // [x, y, z]            world-frame accel sensor measurements
VectorFloat gravity;    // [x, y, z]            gravity vector
float euler[3];         // [psi, theta, phi]    Euler angle container
// float ypr[3];           // [yaw, pitch, roll]   yaw/pitch/roll container and gravity vector


// For Calibration
// #ifdef UNIT_TRIP
    // long offsets[6] = { -1243, -1034, 641, 86, 20, -19 }; // Trip's UNIT
// #elseif UNIT_ZERO
    // long offsets[6] = { 0, 0, 0, 0, 0, 0 };
// #elseif UNIT_DEFAULT
    long offsets[6] = { 0, 0, 1788, 220, 76, -85 };
// #else
    // long offsets[6] = { -1243, -1034, 641, 86, 20, -19 };
// #endif


enum OFFSET_TYPE {
    ACCEL_X,
    ACCEL_Y,
    ACCEL_Z,
    GYRO_X,
    GYRO_Y,
    GYRO_Z
};



// ================================================================
// ===                      INITIAL SETUP                       ===
// ================================================================

void MPUsetup() {
    // join I2C bus (I2Cdev library doesn't do this automatically)
    #if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
        Wire.begin();
        TWBR = 24; // 400kHz I2C clock (200kHz if CPU is 8MHz)
    #elif I2CDEV_IMPLEMENTATION == I2CDEV_BUILTIN_FASTWIRE
        Fastwire::setup(400, true);
    #endif

    // initialize device
    SERIAL_PRINTLN(F("Initializing I2C devices..."));
    mpu.initialize();

    // verify connection
    SERIAL_PRINTLN(F("Testing device connections..."));
    SERIAL_PRINTLN(mpu.testConnection() ? F("MPU6050 connection successful") : F("MPU6050 connection failed"));

    // load and configure the DMP
    SERIAL_PRINTLN(F("Initializing DMP..."));
    devStatus = mpu.dmpInitialize();

    // supply your own gyro offsets here, scaled for min sensitivity
    // mpu.setXGyroOffset( offsets[GYRO_X] );
    // mpu.setYGyroOffset( offsets[GYRO_Y] );
    // mpu.setZGyroOffset( offsets[GYRO_Z] );
    // mpu.setXAccelOffset( offsets[ACCEL_X] );
    // mpu.setYAccelOffset( offsets[ACCEL_Y] );
    // mpu.setZAccelOffset( offsets[ACCEL_Z] );

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

    #ifdef ENABLE_CALIBRATION
      SERIAL_PRINTLN("Begin MPU Calibration...");
      calibrateAccelerations();
      SERIAL_PRINTLN("MPU Calibration COMPLETE");
    #endif

}


const int MAX_ACCEPTABLE_ERROR = 50;
const byte MAX_ERROR_COUNTS = 50;
const byte MAX_OFFSET_ITERATIONS = 10;

/*
// Ideally everything can be vectorized and the proper functions passed...makes for shorter code
// But I don't know how so refer to the following in the meantime
void calibrateMPU() {
    for ( byte i=0; i < sizeof(offsets); i++ ) {

        int sum = 0;
        byte averageCount = 0;
        int average = 0;

        // Average the current reading together for a bit
        while( averageCount < maxAverageCounts ) {
            MPUread();
            int currentError = aaReal.
        }


        // Based on that average, subtract the current offset
        // Exit after max attempts or error is within acceptable range


    }
}
*/


void calibrateAccelerations() {

    byte offsetIteration = 0;
    int averageError = 0;
    
    do {
      averageError = calculateAverageError( &aaReal.x );
      SERIAL_PRINT("Average Error = "); SERIAL_PRINTLN(averageError);
      long deltaOffset = averageError/(offsetIteration+2);
      mpu.setXAccelOffset( offsets[ACCEL_X] -= deltaOffset );
      SERIAL_PRINT("Current X Offset = "); SERIAL_PRINTLN(offsets[ACCEL_X]); SERIAL_PRINTLN();
      offsetIteration++;
    } while ( abs(averageError) > MAX_ACCEPTABLE_ERROR  &&  offsetIteration < MAX_OFFSET_ITERATIONS );
    SERIAL_PRINT("Final X offset = "); SERIAL_PRINT(offsets[ACCEL_X]);
    SERIAL_PRINT(" with Final Error = "); SERIAL_PRINTLN(averageError); SERIAL_PRINTLN();


    offsetIteration = 0;
    averageError = 0;
    do {
      averageError = calculateAverageError( &aaReal.y );
      SERIAL_PRINT("Average Error = "); SERIAL_PRINTLN(averageError);
      long deltaOffset = averageError/(offsetIteration+2);
      mpu.setYAccelOffset( offsets[ACCEL_Y] -= deltaOffset );
      SERIAL_PRINT("Current Y Offset = "); SERIAL_PRINTLN(offsets[ACCEL_Y]); SERIAL_PRINTLN();
      offsetIteration++;
    } while ( abs(averageError) > MAX_ACCEPTABLE_ERROR  &&  offsetIteration < MAX_OFFSET_ITERATIONS );
    SERIAL_PRINT("Final Y offset = "); SERIAL_PRINT(offsets[ACCEL_Y]);
    SERIAL_PRINT(" with Final Error = "); SERIAL_PRINTLN(averageError); SERIAL_PRINTLN();


    offsetIteration = 0;
    averageError = 0;
    do {
      averageError = calculateAverageError( &aaReal.z );
      SERIAL_PRINT("Average Error = "); SERIAL_PRINTLN(averageError);
      long deltaOffset = averageError/(offsetIteration+2);
      mpu.setZAccelOffset( offsets[ACCEL_Z] -= deltaOffset );
      SERIAL_PRINT("Current Z Offset = "); SERIAL_PRINTLN(offsets[ACCEL_Z]); SERIAL_PRINTLN();
      offsetIteration++;
    } while ( abs(averageError) > MAX_ACCEPTABLE_ERROR  &&  offsetIteration < MAX_OFFSET_ITERATIONS );
    SERIAL_PRINT("Final Z offset = "); SERIAL_PRINT(offsets[ACCEL_Z]);
    SERIAL_PRINT(" with Final Error = "); SERIAL_PRINTLN(averageError); SERIAL_PRINTLN();

  SERIAL_PRINTLN("Calibration COMPLETE");
  
  SERIAL_PRINT("xOffset = "); SERIAL_PRINT(offsets[ACCEL_X]);
  SERIAL_PRINT("  yOffset = "); SERIAL_PRINT(offsets[ACCEL_Y]);
  SERIAL_PRINT("  zOffset = "); SERIAL_PRINTLN(offsets[ACCEL_Z]);
  SERIAL_PRINTLN();

}



int calculateAverageError( int16_t *_measurement ) {

    int sumError = 0;
    byte errorCount = 0;
    int averageError;

    // Average the current reading together for a bit
    while( errorCount < MAX_ERROR_COUNTS ) {
        MPUread();
        int16_t *currentError = _measurement;
        sumError += *currentError;
        errorCount++;
    }
    averageError = sumError / errorCount;

    return averageError;
}


// ================================================================
// ===                    MAIN PROGRAM LOOP                     ===
// ================================================================

void MPUread() {
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
        SERIAL_PRINTLN(F("FIFO Overflow, resetting..."));
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

        #ifdef OUTPUT_READABLE_EULER
            // display Euler angles in degrees
            mpu.dmpGetQuaternion(&quat, fifoBuffer);
            mpu.dmpGetEuler(euler, &quat);
            SERIAL_PRINT("euler\t");
            SERIAL_PRINT(euler[0] * 180/M_PI);
            SERIAL_PRINT("\t");
            SERIAL_PRINT(euler[1] * 180/M_PI);
            SERIAL_PRINT("\t");
            SERIAL_PRINTLN(euler[2] * 180/M_PI);
        #endif

        #ifdef OUTPUT_READABLE_YAWPITCHROLL
            // display Euler angles in degrees
            mpu.dmpGetQuaternion(&quat, fifoBuffer);
            mpu.dmpGetGravity(&gravity, &quat);
            mpu.dmpGetYawPitchRoll(ypr, &quat, &gravity);
            SERIAL_PRINT("ypr\t");
            SERIAL_PRINT(ypr[0] * 180/M_PI);
            SERIAL_PRINT("\t");
            SERIAL_PRINT(ypr[1] * 180/M_PI);
            SERIAL_PRINT("\t");
            SERIAL_PRINTLN(ypr[2] * 180/M_PI);
        #endif

        #ifdef OUTPUT_READABLE_REALACCEL
            // display real acceleration, adjusted to remove gravity
            mpu.dmpGetQuaternion(&quat, fifoBuffer);
            mpu.dmpGetAccel(&aa, fifoBuffer);
            mpu.dmpGetGravity(&gravity, &quat);
            mpu.dmpGetLinearAccel(&aaReal, &aa, &gravity);
            // SERIAL_PRINT("areal\t");
            // SERIAL_PRINT(aaReal.x);
            // SERIAL_PRINT("\t");
            // SERIAL_PRINT(aaReal.y);
            // SERIAL_PRINT("\t");
            // SERIAL_PRINTLN(aaReal.z);
        #endif

        #ifdef OUTPUT_READABLE_WORLDACCEL
            // display initial world-frame acceleration, adjusted to remove gravity
            // and rotated based on known orientation from quaternion
            mpu.dmpGetQuaternion(&quat, fifoBuffer);
            mpu.dmpGetAccel(&aa, fifoBuffer);
            mpu.dmpGetGravity(&gravity, &quat);
            mpu.dmpGetLinearAccel(&aaReal, &aa, &gravity);
            mpu.dmpGetLinearAccelInWorld(&aaWorld, &aaReal, &quat);
            SERIAL_PRINT("aworld\t");
            SERIAL_PRINT(aaWorld.x);
            SERIAL_PRINT("\t");
            SERIAL_PRINT(aaWorld.y);
            SERIAL_PRINT("\t");
            SERIAL_PRINTLN(aaWorld.z);
        #endif

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

    // delay(25);


}
