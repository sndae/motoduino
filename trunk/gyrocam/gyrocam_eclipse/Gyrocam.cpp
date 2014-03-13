// Do not remove the include below
#include "Gyrocam.h"

//BMA180 bma;
ITG3200 itg;

//The setup function is called once at startup of the sketch
void setup() {
	Serial.begin(9600);
	I2c.begin();
	//i2c.scan();
	//bma.poll();
	spn("start calib");
	itg.calibrate(5);
	spn("stop calib");
}

// The loop function is called in an endless loop
void loop() {
	delay(2000);
//	bma.poll();
	itg.poll();
	sp(itg.get(0));
	sp(", ");
	sp(itg.get(1));
	sp(", ");
	spn(itg.get(2));

}
