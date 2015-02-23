// Contains Vector16Int as part of helper_3dmath.h
#include "I2Cdev.h"
#include "MPU6050_6Axis_MotionApps20.h"
#if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
    #include "Wire.h"
#endif

#include "Model.h"


VectorInt16 accel { 0, 0, 0 };
float ypr[3] = { 0, 0, 0 };

Pipe_MagnitudeAcceleration pipe_magAccel( &accel );
Pipe_LPFRoll pipe_LPFRoll( ypr );

Pipe *pipes[2] = { &pipe_magAccel, &pipe_LPFRoll };
// Pipe *pipes[2] = { &pipe_magAccel, &pipe_magAccel };

long time_lastUpdate;


void setup() {
	time_lastUpdate = millis();
}



void loop() {
	long deltaMs = millis() - time_lastUpdate;

	for ( int i=0; i<2; ++i ) {
		pipes[i]->update( deltaMs );
	}

	time_lastUpdate = millis();
}