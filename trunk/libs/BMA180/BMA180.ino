
#include <Servo.h> 
#include <Wire.h>
#include "math.h"

#define address 0x40

Servo myservo;  // create servo object to control a servo 
double pos = 180;
double min = 0.5,max = 0.5;
void setup()
{
  Wire.begin();
  Serial.begin(9600);
  initBMA180();
  delay(2000);
  myservo.attach(2);
  myservo.write(pos);
}

void loop()
{

  int X,Y,Z=0;

  //  Serial.println(" X: ");
  //  X = readAccel(0x03, 0x02);
  //  Serial.println(X);
  //  X = readAccel2(0x03, 0x02);
  //  Serial.println(X);
  X = readAccel3(0x03, 0x02);
  //  Serial.println(X);
  double dX;
  dX = X * 0.00025;
//  Serial.print(dX);
//  Serial.print(", ");

  //  Serial.println(" Y: ");
  //  Y = readAccel(0x05, 0x04);  
  //  Serial.println(Y);
  //  Y = readAccel2(0x05, 0x04);  
  //  Serial.println(Y);
  Y = readAccel3(0x05, 0x04); 
  // Serial.println(Y); 
  double dY;
  dY = Y * 0.00025;
//  Serial.print(dY);
//  Serial.print(", ");

  //  Serial.println(" Z: ");
  //  Z = readAccel(0x07, 0x06);
  //  Serial.println(Z);
  //  Z = readAccel2(0x07, 0x06);
  //  Serial.println(Z);
  Z = readAccel3(0x07, 0x06);
  //  Serial.println(Z);
  double dZ;
  dZ = Z * 0.00025;
//  Serial.println(dZ);

  // sur la plaquette polystyrène : roTation par Y
//  if( dY > 1)
//    dY = 1;
//  if( dY < -1)
//    dY = -1;
//    pos = acos(dY);
//  pos = pos * 57.295; // convert radian to degress
//  Serial.print("Acos de dY : ");
//  Serial.print(pos);
//  if (pos < 0)
//    pos = 0;
//  if (pos > 360)
//    pos = 360; 
//  myservo.write(pos);

  if (Y > max)
     max = Y;
  if (Y < min)
     min = Y;
  

  Serial.print("min : ");
  Serial.print(min);
  Serial.print(", max : ");
  Serial.print(max);
  Serial.println();
  delay(500);
}

int readAccel3(byte msbAddr, byte lsbAddr)
{
  short temp, result, temp2;
  byte lsb, msb = 0;

  temp2 = 0;
  temp = 0;

  while(temp != 1)
  {
    Wire.beginTransmission(address);
    //    Wire.write(0x03);
    Wire.write(msbAddr);
    Wire.requestFrom(address, 1);
    while(Wire.available())
    {
      msb = Wire.read();
      temp = msb & 0x01;
    }
  }

  Wire.beginTransmission(address);
  //  Wire.write(0x02);
  Wire.write(lsbAddr);
  Wire.requestFrom(address, 1);
  while(Wire.available())
  {
    lsb = Wire.read();
    temp |= lsb;
    temp = temp >> 2;
  }
  result = Wire.endTransmission();
  /*
  Serial.print("lsb: ");
   Serial.println(lsb, BIN);
   Serial.print("msb: ");
   Serial.println(msb, BIN);
   Serial.print("temp: ");
   Serial.println(temp, BIN);
   */
  msb = msb >> 2;

  if(lsb & B10000000){
    //Serial.println("negative");

    lsb &= B01111111;
    temp2 = lsb << 6;
    temp2 |= msb;       

    temp2 = 0 - temp2;
  }
  else{
    temp2 = lsb << 6;
    temp2 |= msb;    
  }
  /*
  Serial.print("temp2: ");
   Serial.println(temp2, BIN);
   Serial.println(temp2, DEC);
   */

  //  Serial.println(temp2, DEC);

  return temp2;
}

int readAccel2(byte lsbAddr, byte msbAddr){
  byte lsb, msb = 0;
  short result;

  //read LSB first...
  while(!(lsb & 1)){
    Wire.beginTransmission(address);
    Wire.write(lsbAddr);
    Wire.requestFrom(address, 1);
    while(Wire.available()){
      lsb = Wire.read();
    }
    Wire.endTransmission();
  }

  Serial.print("lsb: ");
  Serial.println(lsb, BIN);

  if(lsb & 1){
    Serial.println("new_data");
  }

  //remove bit 0 and bit 1
  lsb = lsb >> 2;
  Serial.print("lsb: ");
  Serial.println(lsb, BIN);


  //now MSB
  Wire.beginTransmission(address);
  Wire.write(msbAddr);
  Wire.requestFrom(address, 1);
  while(Wire.available()){
    msb = Wire.read();
  }
  Wire.endTransmission();

  Serial.print("msb: ");
  Serial.println(msb, BIN);

  result = msb << 6;
  Serial.print("res: ");
  Serial.println(result, BIN);

  result |= lsb;
  Serial.print("res: ");
  Serial.println(result, BIN);

  Serial.print("res: ");
  Serial.println(result, DEC);

  return 0;
}

//int x;

int readAccel(byte msb, byte lsb)
{
  int temp, result;

  temp = 0;

  while(temp != 1)
  {
    Wire.beginTransmission(address);
    //    Wire.write(0x03);
    Wire.write(msb);
    Wire.requestFrom(address, 1);
    while(Wire.available())
    {
      temp = Wire.read() & 0x01;
    }
  }

  Wire.beginTransmission(address);
  //  Wire.write(0x02);
  Wire.write(lsb);
  Wire.requestFrom(address, 1);
  while(Wire.available())
  {
    temp |= Wire.read();
    temp = temp >> 2;
  }
  result = Wire.endTransmission();
  return temp;
}

void initBMA180()
{
  int temp, result, error;

  Wire.beginTransmission(address);
  Wire.write(0x00);
  Wire.requestFrom(address, 1);
  while(Wire.available())
  {
    temp = Wire.read();
  }
  Serial.print("Id = ");
  Serial.println(temp);
  result = Wire.endTransmission();
  checkResult(result);
  if(result > 0)
  {
    error = 1;
  }
  delay(10);
  if(temp == 3)
  {
    // Connect to the ctrl_reg1 register and set the ee_w bit to enable writing.
    Wire.beginTransmission(address);
    Wire.write(0x0D);
    Wire.write(B0001);
    result = Wire.endTransmission();
    checkResult(result);
    if(result > 0)
    {
      error = 1;
    }
    delay(10);
    // Connect to the bw_tcs register and set the filtering level to 10hz.
    Wire.beginTransmission(address);
    Wire.write(0x20);
    Wire.write(B00001000);
    result = Wire.endTransmission();
    checkResult(result);
    if(result > 0)
    {
      error = 1;
    }
    delay(10);
    // Connect to the offset_lsb1 register and set the range to +- 2.
    Wire.beginTransmission(address);
    Wire.write(0x35);
    Wire.write(B0100);
    result = Wire.endTransmission();
    checkResult(result);
    if(result > 0)
    {
      error = 1;
    }
    delay(10);
  }

  if(error == 0)
  {
    Serial.print("BMA180 Init Successful");
  }
}

void checkResult(int result)
{
  if(result >= 1)
  {
    Serial.print("PROBLEM..... Result code is ");
    Serial.println(result);
  }
  else
  {
    Serial.println("Read/Write success");
  }
}

void readId()
{
  int temp, result;

  Wire.beginTransmission(address);
  Wire.write(0x00);
  Wire.requestFrom(address, 1);
  while(Wire.available())
  {
    temp = Wire.read();
  }
  Serial.print("Id = ");
  Serial.println(temp);
  result = Wire.endTransmission();
  checkResult(result);
  delay(10);
} 


