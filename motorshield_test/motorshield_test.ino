
#include <Wire.h>
#include <Adafruit_MotorShield.h>
#include "utility/Adafruit_PWMServoDriver.h"

// Create the motor shield object with the default I2C address
Adafruit_MotorShield AFMS = Adafruit_MotorShield();
// Or, create it with a different I2C address (say for stacking)
// Adafruit_MotorShield AFMS = Adafruit_MotorShield(0x61);

// Select which 'port' M1, M2, M3 or M4. In this case, M1
//Adafruit_DCMotor *myMotor4 = AFMS.getMotor(4);
Adafruit_DCMotor *myMotor3 = AFMS.getMotor(3);
Adafruit_DCMotor *myMotor2 = AFMS.getMotor(2);
Adafruit_DCMotor *myMotor1 = AFMS.getMotor(1);

// You can also make another motor on port M2
//Adafruit_DCMotor *myOtherMotor = AFMS.getMotor(2);

void setup() {
  Serial.begin(9600);           // set up Serial library at 9600 bps

  AFMS.begin();  // create with the default frequency 1.6KHz
  //AFMS.begin(1000);  // OR with a different frequency, say 1KHz

  // turn on motor
  myMotor1->run(RELEASE);
  myMotor2->run(RELEASE);
  myMotor3->run(RELEASE);
  //myMotor4->run(RELEASE);


}

void loop() {
  // put your main code here, to run repeatedly:

  startUpTest();

}

void motorSetSpeed(int motorSpeed, int motor) {
  if (motorSpeed > 255) {
    motorSpeed = 255;
  }
  if (motorSpeed < -255) {
    motorSpeed = -255;
  }

  if (motor == 1) {
    motorRun1(motorSpeed);
  }
  if (motor == 2) {
    motorRun2(motorSpeed);
  }
  if (motor == 3) {
    motorRun3(motorSpeed);
  }
  if (motor == 4) {
    motorRun4(motorSpeed);
  }
}

void motorRun1(int motorSpeed) {
  if (motorSpeed >= 0) {
    myMotor1->run(BACKWARD);
    myMotor1->setSpeed(motorSpeed);
  }
  else {
    myMotor1->run(FORWARD);
    myMotor1->setSpeed(abs(motorSpeed));
  }
}

void motorRun2(int motorSpeed) {
  if (motorSpeed >= 0) {
    myMotor2->run(BACKWARD);
    myMotor2->setSpeed(motorSpeed);
  }
  else {
    myMotor2->run(FORWARD);
    myMotor2->setSpeed(abs(motorSpeed));
  }
}

void motorRun3(int motorSpeed) {
  if (motorSpeed >= 0) {
    myMotor3->run(FORWARD);
    myMotor3->setSpeed(motorSpeed);
  }
  else {
    myMotor3->run(BACKWARD);
    myMotor3->setSpeed(abs(motorSpeed));
  }
}

//void motorRun4(int motorSpeed) {
//  if (motorSpeed >= 0) {
//    myMotor4->run(FORWARD);
//    myMotor4->setSpeed(motorSpeed);
//  }
//  else {
//    myMotor4->run(BACKWARD);
//    myMotor4->setSpeed(abs(motorSpeed));
//  }
}

void startUpTest() {
  uint8_t i;

  for (i=0; i<4; i++){
  motorSetSpeed(100,i);
  delay(1000);
  motorSetSpeed(0,i);
  delay(1000);
  motorSetSpeed(-100,i);
  delay(1000);
  //motorSetSpeed(0,i);
  //delay(1000);
  }
}

