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
#ifdef UNIT_TRIP
  long offsets[6] = { -1243, -1034, 641, 86, 20, -19 }; // Trip's UNIT
#elif UNIT_ZERO
  long offsets[6] = { 0, 0, 0, 0, 0, 0 };
#else
  long offsets[6] = { -1243, -1034, 641, 86, 20, -19 };
#endif
  
//long offsets[6] = { -6046, 832, 2391, 0, 0, 0 };

                    
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
    Serial.println(F("Initializing I2C devices..."));
    mpu.initialize();

    // verify connection
    Serial.println(F("Testing device connections..."));
    Serial.println(mpu.testConnection() ? F("MPU6050 connection successful") : F("MPU6050 connection failed"));

    // load and configure the DMP
    Serial.println(F("Initializing DMP..."));
    devStatus = mpu.dmpInitialize();

    // supply your own gyro offsets here, scaled for min sensitivity
    mpu.setXGyroOffset( offsets[GYRO_X] );
    mpu.setYGyroOffset( offsets[GYRO_Y] );
    mpu.setZGyroOffset( offsets[GYRO_Z] );
    mpu.setXAccelOffset( offsets[ACCEL_X] );
    mpu.setYAccelOffset( offsets[ACCEL_Y] );
    mpu.setZAccelOffset( offsets[ACCEL_Z] );

    // make sure it worked (returns 0 if so)
    if (devStatus == 0) {
        // turn on the DMP, now that it's ready
        Serial.println(F("Enabling DMP..."));
        mpu.setDMPEnabled(true);

        mpuIntStatus = mpu.getIntStatus();

        // set our DMP Ready flag so the main loop() function knows it's okay to use it
        Serial.println(F("DMP ready! Waiting for first interrupt..."));
        dmpReady = true;

        // get expected DMP packet size for later comparison
        packetSize = mpu.dmpGetFIFOPacketSize();
    } else {
        // ERROR!
        // 1 = initial memory load failed
        // 2 = DMP configuration updates failed
        // (if it's going to break, usually the code will be 1)
        Serial.print(F("DMP Initialization failed (code "));
        Serial.print(devStatus);
        Serial.println(F(")"));
    }

    Serial.println("Begin MPU Calibration...");
    calibrateAccelerations();
    Serial.println("MPU Calibration COMPLETE");

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
      Serial.print("Average Error = "); Serial.println(averageError);
      long deltaOffset = averageError/(offsetIteration+2);
      mpu.setXAccelOffset( offsets[ACCEL_X] -= deltaOffset );
      Serial.print("Current X Offset = "); Serial.println(offsets[ACCEL_X]); Serial.println();
      offsetIteration++;
    } while ( abs(averageError) > MAX_ACCEPTABLE_ERROR  &&  offsetIteration < MAX_OFFSET_ITERATIONS );
    Serial.print("Final X offset = "); Serial.print(offsets[ACCEL_X]);
    Serial.print(" with Final Error = "); Serial.println(averageError); Serial.println();


    offsetIteration = 0;
    averageError = 0;
    do {
      averageError = calculateAverageError( &aaReal.y );
      Serial.print("Average Error = "); Serial.println(averageError);
      long deltaOffset = averageError/(offsetIteration+2);
      mpu.setYAccelOffset( offsets[ACCEL_Y] -= deltaOffset );
      Serial.print("Current Y Offset = "); Serial.println(offsets[ACCEL_Y]); Serial.println();
      offsetIteration++;
    } while ( abs(averageError) > MAX_ACCEPTABLE_ERROR  &&  offsetIteration < MAX_OFFSET_ITERATIONS );
    Serial.print("Final Y offset = "); Serial.print(offsets[ACCEL_Y]);
    Serial.print(" with Final Error = "); Serial.println(averageError); Serial.println();


    offsetIteration = 0;
    averageError = 0;
    do {
      averageError = calculateAverageError( &aaReal.z );
      Serial.print("Average Error = "); Serial.println(averageError);
      long deltaOffset = averageError/(offsetIteration+2);
      mpu.setZAccelOffset( offsets[ACCEL_Z] -= deltaOffset );
      Serial.print("Current Z Offset = "); Serial.println(offsets[ACCEL_Z]); Serial.println();
      offsetIteration++;
    } while ( abs(averageError) > MAX_ACCEPTABLE_ERROR  &&  offsetIteration < MAX_OFFSET_ITERATIONS );
    Serial.print("Final Z offset = "); Serial.print(offsets[ACCEL_Z]);
    Serial.print(" with Final Error = "); Serial.println(averageError); Serial.println();

  Serial.println("Calibration COMPLETE");
  
  Serial.print("xOffset = "); Serial.print(offsets[ACCEL_X]);
  Serial.print("  yOffset = "); Serial.print(offsets[ACCEL_Y]);
  Serial.print("  zOffset = "); Serial.println(offsets[ACCEL_Z]);
  Serial.println();

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
    // Serial.print("IntStatus ");
    // Serial.println(mpuIntStatus, HEX);

    // get current FIFO count
    fifoCount = mpu.getFIFOCount();
    // Serial.print(F("FIFO Count "));
    // Serial.println(fifoCount);

    // check for overflow (this should never happen unless our code is too inefficient)
    if ((mpuIntStatus & 0x10) || fifoCount == 1024) {
        // reset so we can continue cleanly
        Serial.println(F("FIFO Overflow, resetting..."));
        mpu.resetFIFO();

    // otherwise, check for DMP data ready interrupt (this should happen frequently)
    } else if (mpuIntStatus & 0x01) {

        // wait for correct available data length, should be a VERY short wait
        while (fifoCount < packetSize) {
            fifoCount = mpu.getFIFOCount();
            // Serial.print(F("Wait - FIFO Count "));
            // Serial.println(fifoCount);
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
            Serial.print("euler\t");
            Serial.print(euler[0] * 180/M_PI);
            Serial.print("\t");
            Serial.print(euler[1] * 180/M_PI);
            Serial.print("\t");
            Serial.println(euler[2] * 180/M_PI);
        #endif

        #ifdef OUTPUT_READABLE_YAWPITCHROLL
            // display Euler angles in degrees
            mpu.dmpGetQuaternion(&quat, fifoBuffer);
            mpu.dmpGetGravity(&gravity, &quat);
            mpu.dmpGetYawPitchRoll(ypr, &quat, &gravity);
            Serial.print("ypr\t");
            Serial.print(ypr[0] * 180/M_PI);
            Serial.print("\t");
            Serial.print(ypr[1] * 180/M_PI);
            Serial.print("\t");
            Serial.println(ypr[2] * 180/M_PI);
        #endif

        #ifdef OUTPUT_READABLE_REALACCEL
            // display real acceleration, adjusted to remove gravity
            mpu.dmpGetQuaternion(&quat, fifoBuffer);
            mpu.dmpGetAccel(&aa, fifoBuffer);
            mpu.dmpGetGravity(&gravity, &quat);
            mpu.dmpGetLinearAccel(&aaReal, &aa, &gravity);
            // Serial.print("areal\t");
            // Serial.print(aaReal.x);
            // Serial.print("\t");
            // Serial.print(aaReal.y);
            // Serial.print("\t");
            // Serial.println(aaReal.z);
        #endif

        #ifdef OUTPUT_READABLE_WORLDACCEL
            // display initial world-frame acceleration, adjusted to remove gravity
            // and rotated based on known orientation from quaternion
            mpu.dmpGetQuaternion(&quat, fifoBuffer);
            mpu.dmpGetAccel(&aa, fifoBuffer);
            mpu.dmpGetGravity(&gravity, &quat);
            mpu.dmpGetLinearAccel(&aaReal, &aa, &gravity);
            mpu.dmpGetLinearAccelInWorld(&aaWorld, &aaReal, &quat);
            Serial.print("aworld\t");
            Serial.print(aaWorld.x);
            Serial.print("\t");
            Serial.print(aaWorld.y);
            Serial.print("\t");
            Serial.println(aaWorld.z);
        #endif

        #ifdef OUTPUT_CUSTOM
            // display real acceleration, adjusted to remove gravity
            mpu.dmpGetQuaternion(&quat, fifoBuffer);
            mpu.dmpGetAccel(&aa, fifoBuffer);
            mpu.dmpGetGravity(&gravity, &quat);
            mpu.dmpGetLinearAccel(&aaReal, &aa, &gravity);
            Serial.print("areal\t");
            Serial.print(aaReal.x);
            Serial.print("\t");
            Serial.print(aaReal.y);
            Serial.print("\t");
            Serial.println(aaReal.z);
            mpu.dmpGetYawPitchRoll(ypr, &quat, &gravity);
            Serial.print("ypr\t");
            Serial.print(ypr[0] * 180/M_PI);
            Serial.print("\t");
            Serial.print(ypr[1] * 180/M_PI);
            Serial.print("\t");
            Serial.println(ypr[2] * 180/M_PI);
        #endif

    }

    // delay(25);


}
