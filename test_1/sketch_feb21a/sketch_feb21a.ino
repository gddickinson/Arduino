#include <Wire.h>
#include <Adafruit_MotorShield.h>
#include "utility/Adafruit_PWMServoDriver.h"

Adafruit_MotorShield AFMS = Adafruit_MotorShield(); 
Adafruit_DCMotor *myMotor = AFMS.getMotor(4);

void setup() {
  // put your setup code here, to run once:
  AFMS.begin();
  myMotor->setSpeed(100); 
}

void loop() {
  // put your main code here, to run repeatedly:
  myMotor->run(FORWARD);
  delay(1000);
  myMotor->run(RELEASE);
  delay(1000);
}
