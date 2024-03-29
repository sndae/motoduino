
#include <Servo.h> 
#include "I2C.h"


#define GYRO                       0x69
#define ACCEL                      0x40
#define SERIAL_SPEED               9600
#define GYRO_RESET                 0x3E
#define GYRO_RESET_VALUE           0x80
#define GYRO_LOW_PASS_FILTER       0x16
#define GYRO_LOW_PASS_FILTER_10Hz  0x1D  
#define GYRO_INTERNAL_OSCILLATOR   0x3E

#define MIN_ANALOG                 -16500
#define MAX_ANALOG                 16500
#define CENTER_ANALOG              0
#define MAP_RANGE                  155
#define MAP_START                  25
#define MAP_CENTER                 90 // 25 + 65 = 90, 90 + 65 = 155

#define SERVO_PIN                  3


int16_t gZero[3];   // Zero values
double angleX;
int pos = MAP_CENTER;
Servo myservo;  // create servo object to control a servo 
int i = 0; 

void setup() {
  // initialize the serial communication:
  Serial.begin(SERIAL_SPEED);
  I2c.begin();

  GyroAndAcceleroInit(500);

  myservo.attach(SERVO_PIN);
  myservo.write(pos);

  Serial.println("Sensors have been initialized"); 
} 

void GyroAndAcceleroInit(int num) 
{ 
  // reset the gyro 
  I2c.write(GYRO, GYRO_RESET, GYRO_RESET_VALUE);
  delay(10); 
  // low pass filter 10 Hz (0x05) + 2000°/sec (0x18)
  I2c.write(GYRO, GYRO_LOW_PASS_FILTER, 0x1D);
  // use internal oscillator 
  I2c.write(GYRO, GYRO_INTERNAL_OSCILLATOR, 0x01);

  int32_t tmp[3];
  for (int i=0; i<3; i++) {
    tmp[i] = 0;
  }

  I2c.read(GYRO, 0x1D, 4);

  short dataGyroX = I2c.receive() << 8; 
  dataGyroX += I2c.receive();
  short dataGyroY = I2c.receive() << 8; 
  dataGyroY += I2c.receive();
  short dataGyroZ = I2c.receive() << 8; 
  dataGyroZ += I2c.receive();

  tmp[0] = tmp[0] + dataGyroX;
  tmp[1] = tmp[2] + dataGyroY;
  tmp[2] = tmp[2] + dataGyroY;

  delayMicroseconds(20);


  for (int i=0; i<3; i++) {
    gZero[i] = tmp[i]/num;
  }

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


void getAngles(){

  I2c.read(GYRO, 0x1D, 4);

  short dataGyroX = I2c.receive() << 8; 
  dataGyroX += I2c.receive();
  short dataGyroY = I2c.receive() << 8; 
  dataGyroY += I2c.receive();
  
  
  // on annule la derive qu'on a obtenu à l'initialisation dans le setup
  dataGyroX -= gZero[0];
  dataGyroY -= gZero[1];
  
  // on transforme dataGyroX en radian.
  dataGyroX /= 14.375 * PI/180;
  dataGyroY /= 14.375 * PI/180;
  
  I2c.read(ACCEL, 0x02, 6);

  short dataAccelX = I2c.receive(); 
  dataAccelX += I2c.receive() << 8; 
  short dataAccelY = I2c.receive(); 
  dataAccelY += I2c.receive() << 8; 
  short dataAccelZ = I2c.receive(); 
  dataAccelZ += I2c.receive() << 8; 

  dataAccelX /= 2;
  dataAccelY /= 2;
  dataAccelZ /= 2;

  short newAngleX = (0.92)*(angleX + dataGyroY * -0.9) + (0.08)*(dataAccelX);  
  //  short newAngleX = angleX + dataGyroY * -0.5;  
  angleX = filter(angleX, newAngleX, 80);
  //  angleX = newAngleX;

  pos = map(angleX, MIN_ANALOG, MAX_ANALOG, MAP_START, MAP_RANGE);

  myservo.write(pos);
  // PRINT DEBUG

  Serial.print("Pos = "); 
  Serial.println(pos);
  Serial.print("Accel X = "); 
  Serial.print(dataAccelX);  
  Serial.print("Gyro Y = "); 
  Serial.println(dataGyroY);

  i++;

  delay(300);
}

short filter(short old_value, short new_value, short filter){
  if(abs(new_value - old_value) > MAP_RANGE*(filter/100)){
    return new_value;
  } 
  else {
    return old_value;
  }
}

void loop() 
{ 
  //  AccelerometerRead(); 
  //  GyroRead(); 
  getAngles();
  delay(25); // slow down output   
}

