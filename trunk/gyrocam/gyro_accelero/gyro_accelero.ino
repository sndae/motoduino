#include <Wire.h> 
#include <Servo.h> 
#define PI 3.14159265


Servo myservo;  // create servo object to control a servo 
int servopin = 0;  // analog pin used to connect the potentiometer
int pos = 0;
double angleX;
boolean clockWiseX;
double angleY;
double angleZ;
int i = 0; 


void setup() 
{ 
  Serial.begin(9600); 
  Wire.begin(); 
   
  Serial.println("Demo started, initializing sensors"); 
   
  AccelerometerInit(); 
  GyroInit(); 
  
  angleX = angleY = angleZ = 0.0; 
  clockWiseX = true;
  myservo.attach(3);
  myservo.write(pos);
  
  Serial.println("Sensors have been initialized"); 
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

void AccelerometerRead() 
{ 
  Wire.beginTransmission(0x40); // address of the accelerometer 
  Wire.write(0x02); // set read pointer to data 
  Wire.endTransmission(); 
  Wire.requestFrom(0x40, 6); 
   
  // read in the 3 axis data, each one is 16 bits 
  // print the data to terminal 
  Serial.print("Accelerometer: X = "); 
  short data = Wire.read(); 
  data += Wire.read() << 8; 
  double X_acc = data * 0.000595;
  Serial.print(X_acc); 
  Serial.print(" , Y = "); 
  data = Wire.read(); 
  data += Wire.read() << 8; 
  double Y_acc = data * 0.000595;
  Serial.print(Y_acc); 
  Serial.print(" , Z = "); 
  data = Wire.read(); 
  data += Wire.read() << 8; 
  double Z_acc = data * 0.000595;
  Serial.print(Z_acc); 
  Serial.println(); 
} 

//
//void AccelerometerReadDegrees() 
//{ 
//  Wire.beginTransmission(0x40); // address of the accelerometer 
//  Wire.write(0x02); // set read pointer to data 
//  Wire.endTransmission(); 
//  Wire.requestFrom(0x40, 6); 
//   
//  // read in the 3 axis data, each one is 16 bits 
//  // print the data to terminal 
//  Serial.print("Accelerometer: X = "); 
//  short data = Wire.read(); 
//  data += Wire.read() << 8; 
//  double X_acc = data * 0.000595 * 180 / PI;
//  Serial.print(X_acc); 
//  Serial.print(" , Y = "); 
//  data = Wire.read(); 
//  data += Wire.read() << 8; 
//  double Y_acc = data * 0.000595 * 180 / PI;
//  Serial.print(Y_acc); 
//  Serial.print(" , Z = "); 
//  data = Wire.read(); 
//  data += Wire.read() << 8; 
//  double Z_acc = data * 0.000595 * 180 / PI;
//  Serial.print(Z_acc); 
//  Serial.println(); 
//} 

void AccelerometerReadRaw() 
{ 
  Wire.beginTransmission(0x40); // address of the accelerometer 
  Wire.write(0x02); // set read pointer to data 
  Wire.endTransmission(); 
  Wire.requestFrom(0x40, 6); 
   
  // read in the 3 axis data, each one is 16 bits 
  // print the data to terminal 
  Serial.print("Accelerometer: X = "); 
  short data = Wire.read(); 
  data += Wire.read() << 8; 
  Serial.print(data); 
  Serial.print(" , Y = "); 
  data = Wire.read(); 
  data += Wire.read() << 8; 
  Serial.print(data); 
  Serial.print(" , Z = "); 
  data = Wire.read(); 
  data += Wire.read() << 8; 
  Serial.print(data); 
  Serial.println(); 
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

void GyroRead() 
{ 
  Wire.beginTransmission(0x69); // address of the gyro 
  Wire.write(0x1D); // set read pointer 
  Wire.endTransmission(); 
   
  Wire.requestFrom(0x69, 6); 
   
  // read in 3 axis of data, 16 bits each, print to terminal 
  short data = Wire.read() << 8; 
  data += Wire.read(); 
  Serial.print("Gyro: X = "); 
  Serial.print(data); 
  Serial.print(" , Y = "); 
  data = Wire.read() << 8; 
  data += Wire.read(); 
  Serial.print(data); 
  Serial.print(" , Z = "); 
  data = Wire.read() << 8; 
  data += Wire.read(); 
  Serial.print(data); 
  Serial.println(); 
} 

void getAngles(){
//  Serial.println("begin");  
  Wire.beginTransmission(0x69); // address of the gyro 
  Wire.write(0x1D); // set read pointer 
  Wire.endTransmission(); 
  Wire.requestFrom(0x69, 6); 
  short dataGyroX = Wire.read() << 8; 
  dataGyroX += Wire.read();
  short dataGyroY = Wire.read() << 8; 
  dataGyroY += Wire.read();
  short dataGyroZ = Wire.read() << 8; 
  dataGyroZ += Wire.read();
//    Serial.println("end gyro");  
  
//    Serial.println("begin");  
  Wire.beginTransmission(0x40); // address of the accelerometer 
  Wire.write(0x02); // set read pointer to data 
  Wire.endTransmission(); 
  Wire.requestFrom(0x40, 6); 
  short dataAccelX = Wire.read(); 
  dataAccelX += Wire.read() << 8; 
  short dataAccelY = Wire.read(); 
  dataAccelY += Wire.read() << 8; 
  short dataAccelZ = Wire.read(); 
  dataAccelZ += Wire.read() << 8; 
//    Serial.println("end accel");
  
    
//  if(dataGyroX < 0) {
//    if(clockWiseX) { // changement de direction !!
//      angleX = (0.700)*(angleX + dataGyroX*0.005) + (0.300)*(dataAccelX);
//    } else { // meme direction
//      angleX = (0.980)*(angleX + dataGyroX*0.005) + (0.020)*(dataAccelX);
//    }
//    clockWiseX = false;
//  } else {
//    if(clockWiseX) { // meme direction !!
//      angleX = (0.980)*(angleX + dataGyroX*0.005) + (0.020)*(dataAccelX);
//    } else { // changement de  direction !!
//      angleX = (0.700)*(angleX + dataGyroX*0.005) + (0.300)*(dataAccelX);
//    } 
//    clockWiseX = true;
//  }
  angleX = (0.900)*(angleX + dataGyroY * -0.8) + (0.10)*(dataAccelX);
  
  angleY = (0.966)*(angleY + dataGyroY*0.025) + (0.034)*(dataAccelY);
  angleZ = (0.966)*(angleZ + dataGyroZ*0.025) + (0.034)*(dataAccelZ);
  
  pos = map(angleX, -16500, 16500, 35, 155);
  if(i%5)
    myservo.write(pos);
  
  if(i == 20){
//  Serial.print("Pos = "); 
//  Serial.print(pos);
//  Serial.print(", X = "); 
//  Serial.print(dataGyroX);  
//  Serial.print(", Y = "); 
//  Serial.println(dataGyroY);
  Serial.print("AngleX = (0.900)*("); 
  Serial.print(angleX);
  Serial.print(" + "); 
  Serial.print(dataGyroY);  
  Serial.print("*0.10) + (0.10)*("); 
  Serial.print(dataAccelX);
  Serial.println(")");

  i = 0;
  }  
  i++;
}


void loop() 
{ 
//  AccelerometerRead(); 
//  GyroRead(); 
  getAngles();
  delay(25); // slow down output   
}
