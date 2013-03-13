// Controlling a servo position using a potentiometer (variable resistor) 
// by Michal Rinott <http://people.interaction-ivrea.it/m.rinott> 

#include <Servo.h> 

Servo myservo;  // create servo object to control a servo 

int ReceptionOctet=0; // variable de stockage des octets reçus par port série
int ReceptionNombre=0; // variable de calcul du nombre reçu par port série
int impulsion=1500; // variable pour impulsion - valeur médiane initiale

// 
void setup() 
{ 
  int pos = 0;    // variable to store the servo position 

  myservo.attach(3);  // attaches the servo on pin 9 to the servo object 
} 

void loop()  
{ int pos = 0;    // variable to store the servo position 

  for(pos = 0; pos < 180; pos += 1)  // goes from 0 degrees to 180 degrees 
  {                                  // in steps of 1 degree 
    myservo.write(pos);              // tell servo to go to position in variable 'pos' 
    delay(15);                       // waits 15ms for the servo to reach the position 
  } 
  for(pos = 180; pos>=1; pos-=1)     // goes from 180 degrees to 0 degrees 
  {                                
    myservo.write(pos);              // tell servo to go to position in variable 'pos' 
    delay(15);                       // waits 15ms for the servo to reach the position 
  } 
} 

