
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
#define CENTER_ANALOG              0
#define MAP_RANGE                  155
#define MAP_START                  25
#define MAP_CENTER                 90 // 25 + 65 = 90, 90 + 65 = 155

#define SERVO_PIN                  3



double angleX;
double sensUpDown;
boolean sensDown;
int pos = MAP_CENTER;
Servo myservo;  // create servo object to control a servo 
int i = 0; 

void setup() {
  // initialize the serial communication:
  Serial.begin(SERIAL_SPEED);
  I2c.begin();
  
  GyroAndAcceleroInit();

  angleX, sensUpDown = 0.0; 
  sensDown = false;
  myservo.attach(SERVO_PIN);
  myservo.write(pos);
  
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


void getAngles(){

  I2c.read(GYRO, 0x1D, 4);
  
  short dataGyroX = I2c.receive() << 8; 
  dataGyroX += I2c.receive();
  short dataGyroY = I2c.receive() << 8; 
  dataGyroY += I2c.receive();
  
  I2c.read(ACCEL, 0x02, 6);
  
  short dataAccelX = I2c.receive(); 
  dataAccelX += I2c.receive() << 8; 
  short dataAccelY = I2c.receive(); 
  dataAccelY += I2c.receive() << 8; 
  short dataAccelZ = I2c.receive(); 
  dataAccelZ += I2c.receive() << 8; 

  sensUpDown = (0.9) * sensUpDown + (0.1) * dataAccelZ;
  
  // Si l'axe Z vaut au moins 8000, on est presque sur de la position, sinon on attend, je système est trop instable
  if(sensUpDown > MAX_ANALOG/2){ // à l'endroit
    if(sensDown){
       sensDown = false;
       dataAccelX *= -1;
       Serial.println("Reversing !!!");
    }
  } else if(sensUpDown < MIN_ANALOG/2) { // la tête à l'envers, on inverse
    if(!sensDown){ // is sens Up
      sensDown = true;
      dataAccelX *= -1;
      Serial.println("Reversing !!!");
    } 
  }      
  
  short newAngleX = (0.900)*(angleX + dataGyroY * -0.5) + (0.10)*(dataAccelX);  
  angleX = filter(angleX, newAngleX, 30);
  
  pos = map(angleX, MIN_ANALOG, MAX_ANALOG, MAP_START, MAP_RANGE);

  // TUR N THE SERVO
  if(i%5){
    
    if(!sensDown){
      myservo.write(pos);
    } else { // la tête à l'envers, on inverse
      //pos = ((pos - MAP_CENTER)*(-1) + MAP_CENTER);
      myservo.write(pos);      
    }      
  }

  // PRINT DEBUG
  if(i == 20){
    //Serial.print("Pos = "); 
    Serial.println(pos);
  Serial.print("Accel X = "); 
  Serial.print(dataAccelX);  
  Serial.print("Gyro Y = "); 
  Serial.println(dataGyroY);
  i = 0;
  }  
  i++;
}

short filter(short old_value, short new_value, short filter){
  if(abs(new_value - old_value) > MAP_RANGE*(filter/100)){
     return new_value; 
  } else {
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
