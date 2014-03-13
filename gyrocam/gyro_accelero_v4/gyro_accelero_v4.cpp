// Do not remove the include below
#include "gyro_accelero_v4.h"

IMU imu;

//The setup function is called once at startup of the sketch
void setup() {
	// Begin Arduino services.
	I2c.begin();

	// Start system.

	imu.init();
}

// The loop function is called in an endless loop
void loop() {
	// unsigned long nextRuntime = micros();

	imu.update(); // Run this ASAP when loop starts so gyro integration is as accurate as possible.

	delay(500);
}
