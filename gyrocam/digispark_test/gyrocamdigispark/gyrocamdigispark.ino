#include <TinyWireM.h> 
#define PI 3.14159265


int pos = 0;
double angleX;
boolean clockWiseX;
double angleY;
double angleZ;
int i = 0; 


void setup() 
{ 
    // initialize the digital pin as an output.
  pinMode(5, OUTPUT); // 3.7 V
  digitalWrite(5, HIGH);
  
  
  Serial.begin(9600); 
  TinyWireM.begin(); 
   
  Serial.println("Demo started, initializing sensors"); 
   
  AccelerometerInit(); 
  GyroInit(); 
  
  angleX = angleY = angleZ = 0.0; 
  clockWiseX = true;

  Serial.println("Sensors have been initialized"); 
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

void AccelerometerRead() 
{ 
  TinyWireM.beginTransmission(0x40); // address of the accelerometer 
  TinyWireM.send(0x02); // set read pointer to data 
  TinyWireM.endTransmission(); 
  TinyWireM.requestFrom(0x40, 6); 
   
  // read in the 3 axis data, each one is 16 bits 
  // print the data to terminal 
  Serial.print("Accelerometer: X = "); 
  short data = TinyWireM.receive(); 
  data += TinyWireM.receive() << 8; 
  double X_acc = data * 0.000595;
  Serial.print(X_acc); 
  Serial.print(" , Y = "); 
  data = TinyWireM.receive(); 
  data += TinyWireM.receive() << 8; 
  double Y_acc = data * 0.000595;
  Serial.print(Y_acc); 
  Serial.print(" , Z = "); 
  data = TinyWireM.receive(); 
  data += TinyWireM.receive() << 8; 
  double Z_acc = data * 0.000595;
  Serial.print(Z_acc); 
  Serial.println(); 
} 

//
//void AccelerometerReadDegrees() 
//{ 
//  TinyWireM.beginTransmission(0x40); // address of the accelerometer 
//  TinyWireM.send(0x02); // set read pointer to data 
//  TinyWireM.endTransmission(); 
//  TinyWireM.requestFrom(0x40, 6); 
//   
//  // read in the 3 axis data, each one is 16 bits 
//  // print the data to terminal 
//  Serial.print("Accelerometer: X = "); 
//  short data = TinyWireM.receive(); 
//  data += TinyWireM.receive() << 8; 
//  double X_acc = data * 0.000595 * 180 / PI;
//  Serial.print(X_acc); 
//  Serial.print(" , Y = "); 
//  data = TinyWireM.receive(); 
//  data += TinyWireM.receive() << 8; 
//  double Y_acc = data * 0.000595 * 180 / PI;
//  Serial.print(Y_acc); 
//  Serial.print(" , Z = "); 
//  data = TinyWireM.receive(); 
//  data += TinyWireM.receive() << 8; 
//  double Z_acc = data * 0.000595 * 180 / PI;
//  Serial.print(Z_acc); 
//  Serial.println(); 
//} 

void AccelerometerReadRaw() 
{ 
  TinyWireM.beginTransmission(0x40); // address of the accelerometer 
  TinyWireM.send(0x02); // set read pointer to data 
  TinyWireM.endTransmission(); 
  TinyWireM.requestFrom(0x40, 6); 
   
  // read in the 3 axis data, each one is 16 bits 
  // print the data to terminal 
  Serial.print("Accelerometer: X = "); 
  short data = TinyWireM.receive(); 
  data += TinyWireM.receive() << 8; 
  Serial.print(data); 
  Serial.print(" , Y = "); 
  data = TinyWireM.receive(); 
  data += TinyWireM.receive() << 8; 
  Serial.print(data); 
  Serial.print(" , Z = "); 
  data = TinyWireM.receive(); 
  data += TinyWireM.receive() << 8; 
  Serial.print(data); 
  Serial.println(); 
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

void GyroRead() 
{ 
  TinyWireM.beginTransmission(0x69); // address of the gyro 
  TinyWireM.send(0x1D); // set read pointer 
  TinyWireM.endTransmission(); 
   
  TinyWireM.requestFrom(0x69, 6); 
   
  // read in 3 axis of data, 16 bits each, print to terminal 
  short data = TinyWireM.receive() << 8; 
  data += TinyWireM.receive(); 
  Serial.print("Gyro: X = "); 
  Serial.print(data); 
  Serial.print(" , Y = "); 
  data = TinyWireM.receive() << 8; 
  data += TinyWireM.receive(); 
  Serial.print(data); 
  Serial.print(" , Z = "); 
  data = TinyWireM.receive() << 8; 
  data += TinyWireM.receive(); 
  Serial.print(data); 
  Serial.println(); 
} 

void getAngles(){
//  Serial.println("begin");  
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
//    Serial.println("end gyro");  
  
//    Serial.println("begin");  
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
