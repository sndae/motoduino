#include <SoftwareSerial.h>

/*
  Blink
  Turns on an LED on for one second, then off for one second, repeatedly.
 
  This example code is in the public domain.
 */
 
// Pin 13 has an LED connected on most Arduino boards.
// give it a name:
int led = 13;

// the setup routine runs once when you press reset:
void setup() {                
  // initialize the digital pin as an output.
  pinMode(led, OUTPUT);     
  Serial.begin(9600);
  Serial.flush();
}

// the loop routine runs over and over again forever:
void loop() {
  while(Serial.available() < 1){
  }
  int c = Serial.parseInt();
Serial.flush();
  Serial.println(c);
  digitalWrite(led, HIGH);   // turn the LED on (HIGH is the voltage level)
  Serial.write("On\n");
  //delay(1000);               // wait for a second
  c = Serial.parseInt();
  Serial.read();
  Serial.println(c);
  digitalWrite(led, LOW);    // turn the LED off by making the voltage LOW
  Serial.write("Off\n");
  //delay(1000);               // wait for a second
}
