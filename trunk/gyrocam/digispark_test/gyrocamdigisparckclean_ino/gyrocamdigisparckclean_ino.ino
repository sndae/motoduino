#include <TinyWireM.h> 
#include <SoftRcPulseOut.h>

SoftRcPulseOut servo;
int pos = 90;
double angleX;

void setup() 
{ 
    // initialize the digital pin as an output.
  pinMode(5, OUTPUT); // 3.7 V
  digitalWrite(5, HIGH);
  servo.attach(4);
  servo.setMaximumPulse(2200);
  
  TinyWireM.begin(); 
  AccelerometerInit();
  GyroInit();
} 

void AccelerometerInit() 
{ 
  TinyWireM.beginTransmission(0x40); // address of the accelerometer 
  // reset the accelerometer 
  TinyWireM.send(0x10); 
  TinyWireM.send(0xB6); 
  TinyWireM.endTransmission(); 
  delay(10); 
  TinyWireM.beginTransmission(0x40); // address of the accelerometer 
  // low pass filter, range settings 
  TinyWireM.send(0x0D); 
  TinyWireM.send(0x10); 
  TinyWireM.endTransmission(); 
  TinyWireM.beginTransmission(0x40); // address of the accelerometer 
  TinyWireM.send(0x20); // read from here 
  TinyWireM.endTransmission(); 
  TinyWireM.requestFrom(0x40, 1); 
  byte data = TinyWireM.receive(); 
  TinyWireM.beginTransmission(0x40); // address of the accelerometer 
  TinyWireM.send(0x20); 
  TinyWireM.send(data & 0x0F); // low pass filter to 10 Hz 
  TinyWireM.endTransmission();
  TinyWireM.beginTransmission(0x40); // address of the accelerometer 
  TinyWireM.send(0x35); // read from here 
  TinyWireM.endTransmission(); 
  TinyWireM.requestFrom(0x40, 1); 
  data = TinyWireM.receive(); 
  TinyWireM.beginTransmission(0x40); // address of the accelerometer 
  TinyWireM.send(0x35); 
  TinyWireM.send((data & 0xF1) | 0x04); // range +/- 2g 
  TinyWireM.endTransmission(); 
} 

void GyroInit() 
{ 
  TinyWireM.beginTransmission(0x69); // address of the gyro 
  // reset the gyro 
  TinyWireM.send(0x3E); 
  TinyWireM.send(0x80); 
  TinyWireM.endTransmission(); 
  TinyWireM.beginTransmission(0x69); // address of the gyro 
  // low pass filter 10 Hz 
  TinyWireM.send(0x16); 
  TinyWireM.send(0x1D); 
  TinyWireM.endTransmission(); 
  TinyWireM.beginTransmission(0x69); // address of the gyro 
  // use internal oscillator 
  TinyWireM.send(0x3E); 
  TinyWireM.send(0x01); 
  TinyWireM.endTransmission(); 
} 


void getAngles(){
  TinyWireM.beginTransmission(0x69); // address of the gyro 
  TinyWireM.send(0x1D); // set read pointer 
  TinyWireM.endTransmission(); 
  TinyWireM.requestFrom(0x69, 6); 
  short dataGyroX = TinyWireM.receive() << 8; 
  dataGyroX += TinyWireM.receive();
  short dataGyroY = TinyWireM.receive() << 8; 
  dataGyroY += TinyWireM.receive();
  short dataGyroZ = TinyWireM.receive() << 8; 
  dataGyroZ += TinyWireM.receive();

  TinyWireM.beginTransmission(0x40); // address of the accelerometer 
  TinyWireM.send(0x02); // set read pointer to data 
  TinyWireM.endTransmission(); 
  TinyWireM.requestFrom(0x40, 6); 
  short dataAccelX = TinyWireM.receive(); 
  dataAccelX += TinyWireM.receive() << 8; 
  short dataAccelY = TinyWireM.receive(); 
  dataAccelY += TinyWireM.receive() << 8; 
  short dataAccelZ = TinyWireM.receive(); 
  dataAccelZ += TinyWireM.receive() << 8; 

  angleX = (0.900)*(angleX + dataGyroY * -0.8) + (0.10)*(dataAccelX);
  
  pos = map(angleX, -16500, 16500, 35, 155);
}


void loop(){
  getAngles();
  digitalWrite(0, HIGH);   // turn the LED on (HIGH is the voltage level)
  digitalWrite(1, HIGH);
  delay(pos*10);               // wait for a second
  digitalWrite(0, LOW);    // turn the LED off by making the voltage LOW
  digitalWrite(1, LOW); 
  delay(pos*10); 
}

