// Alternate Control 
// Read Serial string  
//Responds to serial commands formatted $L255Z (Left motor forward 255), $R255Z (right motor forward 255)

#include <Wire.h>
#include <Adafruit_MotorShield.h>
#include "utility/Adafruit_PWMServoDriver.h"
#include <Arduino.h>
#include <SoftwareSerial.h>
#include <Wire.h>
#include <Servo.h> 

int frontLeft = 4;
int frontRight = 3;
int backLeft = 1;
int backRight = 2;

Adafruit_MotorShield AFMS = Adafruit_MotorShield(); 
Adafruit_DCMotor *myMotor1 = AFMS.getMotor(frontLeft);
Adafruit_DCMotor *myMotor2 = AFMS.getMotor(frontRight);
Adafruit_DCMotor *myMotor3 = AFMS.getMotor(backLeft);
Adafruit_DCMotor *myMotor4 = AFMS.getMotor(backRight);

// Use built in Arduino Servo library
Servo servo1;
Servo servo2;

/// These values are received from the Processing sketch
int left = 0;
int right = 0;
int top = 75;
int bottom = 75;

int deadband = 5;

//Sets characters to mark start and end of message
String readString;
char command_end = 'Z';
char command_begin = '$';

char current_char;


void setup() {

  Serial.begin(19200); 

  AFMS.begin();  // create with the default frequency 1.6KHz
  //AFMS.begin(100);  // OR with a different frequency, say 1KHz
  // Set the speed to start, from 0 (off) to 255 (max speed)

  //turn on motors
  myMotor1->run(RELEASE);
  myMotor2->run(RELEASE);
  myMotor3->run(RELEASE);
  myMotor4->run(RELEASE);


  //attach servo to pins #9 & #10
  servo1.attach(10);
  servo2.attach(9);

  servo1.write(75);
  delay(25);
  servo2.write(75);
  delay(1000);

  readString = "";

}


void loop() {

  ////////// use Serial
  while (Serial.available()) {
    current_char = Serial.read();  //gets one byte from serial buffer

    if(current_char == command_begin){ // when we get a begin character, start reading
      readString = "";
      while(current_char != command_end){ // stop reading when we get the end character
        current_char = Serial.read();  //gets one byte from serial buffer
        if(current_char != command_end){
          //Serial.println(current_char);
          readString += current_char;
        }
      }
      if(current_char == command_end){ // since we have the end character, send the whole command to the command handler and reset readString.
        //Serial.println("foo");
        handle_command(readString);
        readString = "";
      }
    } 
  } 
  //makes the string readString


  // Here we decide whether the motors should go forward or reverse.
  // if the value is positive, go forward - if the value is negative, go reverse
  // We use a deadband to allow for some "Neutral" space around the center - I set deadband = 5, you can change this, though I wouldn't really go any lower.
  // If no deadband is used, a sporadic signal could cause movement of the bot even with no user input.

  // first determine direction for the left-side motors
    motorSetSpeed(left,2);
    delay(25);
    motorSetSpeed(left,4);
    delay(25);
  // then determine direction for the  right-side motors
    motorSetSpeed(right,1);
    delay(25);
    motorSetSpeed(right,3);
    delay(25); 
  //set servos
    bottomServoPosition(bottom);
    delay(25);
    topServoPosition(top);
    delay(25);

  
  // end of loop

}


void set_left_value(String the_string){
  if(the_string.substring(0,1) == "L"){
    char temp[20];
    the_string.substring(1).toCharArray(temp, 19);
    int l_val = atoi(temp);
    left = l_val;
  }
}

void set_right_value(String the_string){
  if(the_string.substring(0,1) == "R"){
    char temp[20];
    the_string.substring(1).toCharArray(temp, 19);
    int r_val = atoi(temp);
    right = r_val;
  }
}

void set_bottomServo_value(String the_string){
  if(the_string.substring(0,1) == "B"){
    char temp[20];
    the_string.substring(1).toCharArray(temp, 19);
    int b_val = atoi(temp);
    bottom = b_val;
  }
}

void set_topServo_value(String the_string){
  if(the_string.substring(0,1) == "T"){
    char temp[20];
    the_string.substring(1).toCharArray(temp, 19);
    int t_val = atoi(temp);
    top = t_val;
  }
}


void handle_command(String readString){

  set_left_value(readString);
  set_right_value(readString);
  set_bottomServo_value(readString);
  set_topServo_value(readString);
  


  // Here you can send the values back to your Computer and read them on the Processing terminal.
  // Sending these values over Xbee can take slow the sketch down, so I comment them out after testing. 
  
   //Serial.print("left: ");
   //Serial.print(left);
   //Serial.print("     ");
   //Serial.print("right: ");
   //Serial.print(right);
   //Serial.println("     ");
   //Serial.print("bottom: ");
   //Serial.print(bottom);
   //Serial.print("     ");
   //Serial.print("top: ");
   //Serial.print(top);
   //Serial.println("     ");    

}

//motor controls

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

void motorRun4(int motorSpeed) {
  if (motorSpeed >= 0) {
    myMotor4->run(FORWARD);
    myMotor4->setSpeed(motorSpeed);
  }
  else {
    myMotor4->run(BACKWARD);
    myMotor4->setSpeed(abs(motorSpeed));
  }
}

//Servo controls

void bottomServoPosition(int b_pos){
  if (b_pos >175) {b_pos = 175;}
  if (b_pos <5) {b_pos = 5;}
  servo1.write(b_pos);
  delay(25);
}

void topServoPosition(int pos){
  if (pos >175) {pos = 175;}
  if (pos <5) {pos = 5;}
  if (pos >180) {pos = 180;}
  servo2.write(pos);
  delay(25);
}

// end of sketch
