/*
  Graph

 http://www.arduino.cc/en/Tutorial/Graph
 */

#include <Wire.h> 


double angleX;
double angleY;
double angleZ;


void setup() {
  // initialize the serial communication:
  Serial.begin(115200);
  GyroInit();
  AccelerometerInit();
}


void GyroInit() 
{ 
  Wire.beginTransmission(0x69); // address of the gyro 
  // reset the gyro 
  Wire.write(0x3E); 
  Wire.write(0x80); 
  Wire.endTransmission(); 
   
  Wire.beginTransmission(0x69); // address of the gyro 
  // low pass filter 10 Hz 
  Wire.write(0x16); 
  Wire.write(0x1D); 
  Wire.endTransmission(); 
   
  Wire.beginTransmission(0x69); // address of the gyro 
  // use internal oscillator 
  Wire.write(0x3E); 
  Wire.write(0x01); 
  Wire.endTransmission(); 
} 
void AccelerometerInit() 
{ 
  Wire.beginTransmission(0x40); // address of the accelerometer 
  // reset the accelerometer 
  Wire.write(0x10); 
  Wire.write(0xB6); 
  Wire.endTransmission(); 
  delay(10); 
   
  Wire.beginTransmission(0x40); // address of the accelerometer 
  // low pass filter, range settings 
  Wire.write(0x0D); 
  Wire.write(0x10); 
  Wire.endTransmission(); 
   
  Wire.beginTransmission(0x40); // address of the accelerometer 
  Wire.write(0x20); // read from here 
  Wire.endTransmission(); 
  Wire.requestFrom(0x40, 1); 
  byte data = Wire.read(); 
  Wire.beginTransmission(0x40); // address of the accelerometer 
  Wire.write(0x20); 
  Wire.write(data & 0x0F); // low pass filter to 10 Hz 
  Wire.endTransmission(); 
   
  Wire.beginTransmission(0x40); // address of the accelerometer 
  Wire.write(0x35); // read from here 
  Wire.endTransmission(); 
  Wire.requestFrom(0x40, 1); 
  data = Wire.read(); 
  Wire.beginTransmission(0x40); // address of the accelerometer 
  Wire.write(0x35); 
  Wire.write((data & 0xF1) | 0x04); // range +/- 2g 
  Wire.endTransmission(); 
} 


void getAngles(){
//  Serial.println("begin");  
  Wire.beginTransmission(0x69); // address of the gyro 
  Wire.write(0x1D); // set read pointer 
  //Wire.endTransmission(); 
  
  //Wire.beginTransmission(0x69); // address of the gyro 
  Wire.requestFrom(0x69, 6); 
  short dataGyroX = Wire.read() << 8; 
  dataGyroX += Wire.read();
  short dataGyroY = Wire.read() << 8; 
  dataGyroY += Wire.read();
  short dataGyroZ = Wire.read() << 8; 
  dataGyroZ += Wire.read();
  Wire.endTransmission(); 
  
//    Serial.println("begin");  
  Wire.beginTransmission(0x40); // address of the accelerometer 
  Wire.write(0x02); // set read pointer to data 
  //Wire.endTransmission(); 
  
  //Wire.beginTransmission(0x40); // address of the accelerometer 
  Wire.requestFrom(0x40, 6); 
  short dataAccelX = Wire.read(); 
  dataAccelX += Wire.read() << 8; 
  short dataAccelY = Wire.read(); 
  dataAccelY += Wire.read() << 8; 
  short dataAccelZ = Wire.read(); 
  dataAccelZ += Wire.read() << 8; 
  Wire.endTransmission(); 
  
  angleX = (0.900)*(angleX + dataGyroY * -0.8) + (0.10)*(dataAccelX);
  
  //angleY = (0.966)*(angleY + dataGyroY*0.025) + (0.034)*(dataAccelY);
  //angleZ = (0.966)*(angleZ + dataGyroZ*0.025) + (0.034)*(dataAccelZ);
  
  // send the value of analog input 0:
  Serial.println(map(angleX, -16500, 16500, 0, 1023));
}

void loop() {
  getAngles();

  delay(100);
}

