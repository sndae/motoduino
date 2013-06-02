
#include "imu.h"

#define SERIAL_SPEED               115200

IMU imu;
    
void setup() {
  // initialize the serial communication:
  Serial.begin(SERIAL_SPEED);
  
    // Begin Arduino services.
    I2c.begin();

    // Start system.

    imu.init();
} 

void loop() 
{ 

  // unsigned long nextRuntime = micros();
    
  imu.update();   // Run this ASAP when loop starts so gyro integration is as accurate as possible.
  
  delay(500);
    
}
