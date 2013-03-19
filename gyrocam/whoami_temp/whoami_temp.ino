
#include "I2C.h" 
#include <Servo.h> 

#define GYRO                       0x69
#define ACCEL                      0x40
#define SERIAL_SPEED               115200
#define GYRO_RESET                 0x3E
#define GYRO_RESET_VALUE           0x80
#define GYRO_LOW_PASS_FILTER       0x16
#define GYRO_LOW_PASS_FILTER_10Hz  0x1D  
#define GYRO_INTERNAL_OSCILLATOR   0x3E

#define MIN_ANALOG                 -16500
#define MAX_ANALOG                 16500
#define MAP_RANGE                  155
#define MAP_START                  33
#define MAP_IDLE                   MAP_START+(MAP_RANGE-MAP_START)


void setup() {
  // initialize the serial communication:
  Serial.begin(SERIAL_SPEED);
  I2c.begin();

  GyroAndAcceleroInit();

  Serial.println("Sensors have been initialized"); 
} 

void GyroAndAcceleroInit() 
{ 
  // reset the gyro 
  I2c.write(GYRO, GYRO_RESET, GYRO_RESET_VALUE);
  delay(10); 
  // low pass filter 10 Hz 
  I2c.write(GYRO, GYRO_LOW_PASS_FILTER, GYRO_LOW_PASS_FILTER_10Hz);
  // use internal oscillator 
  I2c.write(GYRO, GYRO_INTERNAL_OSCILLATOR, 0x01);

  // reset the accelerometer 
  I2c.write(ACCEL, 0x10, 0xB6); 
  delay(10); 
  // low pass filter, range settings 
  I2c.write(ACCEL, 0x0D, 0x10); 
  // low pass filter to 10 Hz 
  I2c.read(ACCEL, 0x20, 1);
  byte data = I2c.receive();
  I2c.write(ACCEL, 0x20, data & 0x0F); 
  // range +/- 2g 
  I2c.read(ACCEL, 0x35, 1);
  data = I2c.receive();
  I2c.write(ACCEL, 0x35, (data & 0xF1) | 0x04); 
} 

void readGyro(int _addr, int _nbytes, int _buff[]) {
  I2c.read(GYRO, _addr, _nbytes); 
  int i = 0; 
  while (I2c.available()) {
    _buff[i] = I2c.receive(); // receive DATA
    i++;
  }
}

void loop() 
{ 
  
  // TEMP SENSOR
  int _buff[6];  
  readGyro(0x1B, 2, _buff); 
  float c = 35 + ((_buff[0] << 8 | _buff[1]) + 13200) / 280.0;
  Serial.print(c); 
  Serial.println(". end");
  
  // WHO AM I
  I2c.read(GYRO, 0x00, 1);

  while(I2c.available()){
    delay(100);
    short id = I2c.receive();
    Serial.print(id); 
  }
  Serial.println(". end");
  
  //pos = map(angleX, MIN_ANALOG, MAX_ANALOG, MAP_START, MAP_RANGE);
  delay(100); // slow down output   
}

