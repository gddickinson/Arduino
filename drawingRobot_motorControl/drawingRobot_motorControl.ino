//Responds to serial commands formatted $L255Z (Left motor forward 255), $R255Z (right motor forward 255)
#include <Wire.h>
#include <Adafruit_MotorShield.h>
#include "utility/Adafruit_PWMServoDriver.h"
#include <Arduino.h>
#include <SoftwareSerial.h>
#include <Servo.h> 


int motor1 = 1;
int motor2 = 2;
int motor3 = 3;
int motor4 = 4;

Adafruit_MotorShield AFMS = Adafruit_MotorShield(); 
Adafruit_DCMotor *myMotor1 = AFMS.getMotor(motor1);
Adafruit_DCMotor *myMotor2 = AFMS.getMotor(motor2);
Adafruit_DCMotor *myMotor3 = AFMS.getMotor(motor3);
Adafruit_DCMotor *myMotor4 = AFMS.getMotor(motor4);

//for serial control
int newmotor1= 0;
int newmotor2 = 0;
int newmotor3 = 0;
int newmotor4 = 0;
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


  // first determine direction for motor1
    motorSetSpeed(newmotor1,1);
    delay(25);

  // then determine direction for motor2
    motorSetSpeed(newmotor2,2);
    delay(25);

  // then determine direction for motor3
    motorSetSpeed(newmotor3,3);
    delay(25);

  // then determine direction for motor4
    motorSetSpeed(newmotor4,4);
    delay(25);


  // end of loop

}

//motor1
void set_one_value(String the_string){
  if(the_string.substring(0,1) == "A"){
    char temp[20];
    the_string.substring(1).toCharArray(temp, 19);
    int A_val = atoi(temp);
    newmotor1 = A_val;
  }
}


//motor2
void set_two_value(String the_string){
  if(the_string.substring(0,1) == "B"){
    char temp[20];
    the_string.substring(1).toCharArray(temp, 19);
    int B_val = atoi(temp);
    newmotor2 = B_val;
  }
}

//motor3
void set_three_value(String the_string){
  if(the_string.substring(0,1) == "C"){
    char temp[20];
    the_string.substring(1).toCharArray(temp, 19);
    int C_val = atoi(temp);
    newmotor3 = C_val;
  }
}

//motor4
void set_four_value(String the_string){
  if(the_string.substring(0,1) == "D"){
    char temp[20];
    the_string.substring(1).toCharArray(temp, 19);
    int D_val = atoi(temp);
    newmotor4 = D_val;
  }
}


//Forward
void set_forward_value(String the_string){
  if(the_string.substring(0,1) == "F"){
    char temp[20];
    the_string.substring(1).toCharArray(temp, 19);
    int f_val = atoi(temp);
    newmotor1 = f_val;
    newmotor3 = f_val;
  }
}

//Backward
void set_backward_value(String the_string){
  if(the_string.substring(0,1) == "B"){
    char temp[20];
    the_string.substring(1).toCharArray(temp, 19);
    int b_val = atoi(temp);
    newmotor1 = -b_val;
    newmotor3 = -b_val;
  }
}


//All stop
void set_exit_value(String the_string){
  if(the_string.substring(0,1) == "X"){
    newmotor1 = 0;
    newmotor2 = 0;
    newmotor3 = 0;
    newmotor4 = 0;
  }
}



void handle_command(String readString){

  set_one_value(readString);
  set_two_value(readString);
  set_three_value(readString);
  set_exit_value(readString);
  set_forward_value(readString);
  set_backward_value(readString);
  set_four_value(readString); 

  // Here you can send the values back to your Computer and read them on the Processing terminal.
  // Sending these values over Xbee can take slow the sketch down, so I comment them out after testing. 
  
//   Serial.print("left: ");
//   Serial.print(newLeft);
//   Serial.print("     ");
//   Serial.print("right: ");
//   Serial.print(newRight);
//   Serial.println("     "); 
//   Serial.print("slider: ");
//   Serial.print(newSlider);
//   Serial.println("     ");
//   Serial.print("arm: ");
//   Serial.print(newArm);
//   Serial.println("     ");    

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



