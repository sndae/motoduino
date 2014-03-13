
#include <Servo.h> 
#include <Math.h> 

#include "I2C.h"
#include "triMath.h"

#define  SERVO_PIN  3
#define  MAP_RANGE  90
#define  PI         3.1415926535

float stepVolt = 0.0048828125;
float ecart = 26; // nb cm entre les 2 capteurs
float left, right, tangant;
short new_angle, old_angle, angle;

void setup() {
  Serial.begin(9600);

  pinMode(A4, INPUT);
  pinMode(A5, INPUT);

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
  delay(200);

// moyenne de valeur sur 10 prises
  for(int i = 0; i < 10; i ++){
    left = analogRead(A4) * stepVolt;    
    right = analogRead(A5) * stepVolt;

    left = 35*pow(left, -1.10);
    right = 35*pow(right, -1.10);

    tangant = (right-left)/ecart;  
    new_angle = atan(tangant)*180/PI;
    new_angle = filter(old_angle, new_angle, 20);
    old_angle = new_angle;

    angle += new_angle;
  }
  Serial.print("angle : ");
  Serial.println(angle/10);
  angle = 0;
}



