// Controlling a servo position using a potentiometer (variable resistor) 
// by Michal Rinott <http://people.interaction-ivrea.it/m.rinott> 

#include <Servo.h> 
#include <Wire.h>
#include <ITG3200.h>

ITG3200 gyro = ITG3200();
float  x,y,z;
int ix, iy, iz;


Servo myservo;  // create servo object to control a servo 
int ReceptionOctet=0; // variable de stockage des octets reçus par port série
int ReceptionNombre=0; // variable de calcul du nombre reçu par port série
int impulsion=1500; // variable pour impulsion - valeur médiane initiale
int pos = 0;    // variable to store the servo position 
  

void setup() 
{ 
  Serial.begin(9600);
  Wire.begin();      // if experiencing gyro problems/crashes while reading XYZ values
                     // please read class constructor comments for further info.
  delay(1000);
  // Use ITG3200_ADDR_AD0_HIGH or ITG3200_ADDR_AD0_LOW as the ITG3200 address 
  // depending on how AD0 is connected on your breakout board, check its schematics for details
  gyro.init(ITG3200_ADDR_AD0_HIGH); 
  
  Serial.print("zeroCalibrating...");
  gyro.zeroCalibrate(2500, 2);
  Serial.println("done.");
  
  myservo.attach(2);  // attaches the servo on pin 9 to the servo object 
} 

void loop()  
{ 
  int pos = 180;    // variable to store the servo position 
   
  while (gyro.isRawDataReady()) {
      // Reads calibrated values in deg/sec    
    gyro.readGyro(&x,&y,&z); 
    Serial.print("X3:"); 
    Serial.print(x); 
    Serial.print("  Y:"); 
    Serial.print(y);
    pos += -0.15*y; 
    Serial.print("  Z:"); 
    Serial.println(z);
    
    myservo.write(pos);
    
    delay(100); 
  }
} 

