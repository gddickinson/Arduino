/*************************************************************************
* File Name          : DualMode_Starter.ino
* Author             : Jasen
* Updated            : Xiaoyu
* Version            : V1.0.0
* Date               : 3/3/2014
* Description        : Demo code for Makeblock Starter Robot kit,two motors
                       connect on the M1 and M2 port of baseshield or baseboard, The IR receiver module
                       connect on port 6 and the Ultrasonic sensor connect on port 3.
                       The four black button on the IR controller is used to control the direction 
                       of robot, the number button on the IR controller is for changing the speed of the robot.
                       button 1 is for setting the speed to the slowest,button 9 is for setting the speed to fastest.
                       We can press the button TEST to switch operating mode between an ultrasonic obstacle-avoiding robot and an IR contrtol robot.
* License            : CC-BY-SA 3.0
* Copyright (C) 2013 Maker Works Technology Co., Ltd. All right reserved.
* http://www.makeblock.cc/
**************************************************************************/
#include <Makeblock.h>
#include <Arduino.h>
#include <SoftwareSerial.h>
#include <Wire.h>

MeServo servoDriver1(PORT_1,DEV1);//can ONLY be PORT_1,PORT_2
MeServo servoDriver2(PORT_1,DEV2);

//servo positions
int camVertical = 145; //up - down
int camHorizontal = 90; //left - right

//Number of beeps
int beepNumber = 2;
int beepLength = 100;
int beepDelay = 100;

boolean sonar = true;

MeDCMotor MotorL(M1);  
MeDCMotor MotorR(M2);
MeInfraredReceiver infraredReceiverDecode(PORT_8);
MeUltrasonicSensor UltrasonicSensor(PORT_3);
int moveSpeed = 190;
int turnSpeed = 200;
int distance = 0;
int randnum = 0;
boolean leftflag,rightflag;
int minSpeed = 48;
int factor = 23;

uint8_t mode = 0;


void setup()
{
    infraredReceiverDecode.begin();
    leftflag=false;
    rightflag=false;
    randomSeed(analogRead(0));
    servoDriver1.begin();
    servoDriver2.begin();
    servoDriver1.write(camVertical);//up
    servoDriver2.write(camHorizontal);//front
    
    beep(beepNumber, beepLength, beepDelay);
    
    Serial.begin(9600);
}

void loop()
{
  
  
 if(infraredReceiverDecode.buttonState() == 1)
    {
      
        switch(infraredReceiverDecode.read())
        {
          case IR_BUTTON_PLUS: 
               Forward();
               break;
          case IR_BUTTON_MINUS:
               Backward();
               break;
          case IR_BUTTON_NEXT:
               TurnRight();
               break;
          case IR_BUTTON_PREVIOUS:
               TurnLeft();
               break;
          case IR_BUTTON_9:
               ChangeSpeed(factor*9+minSpeed);
               break;
          case IR_BUTTON_8:
               ChangeSpeed(factor*8+minSpeed);
               break;
          case IR_BUTTON_7:
               ChangeSpeed(factor*7+minSpeed);
               break;
          case IR_BUTTON_6:
               ChangeSpeed(factor*6+minSpeed);
               break;
          case IR_BUTTON_5:
               ChangeSpeed(factor*5+minSpeed);
               break;
          case IR_BUTTON_4:
               ChangeSpeed(factor*4+minSpeed);
               break;
         case IR_BUTTON_3:
               ChangeSpeed(factor*3+minSpeed);
               break;
         case IR_BUTTON_2:
               ChangeSpeed(factor*2+minSpeed);
               break;
         case IR_BUTTON_1:
               ChangeSpeed(factor*1+minSpeed);
               break;
         case IR_BUTTON_TEST:
               LookLeft();
               break;
         case IR_BUTTON_RETURN:
               LookRight();
               break;
         case IR_BUTTON_0:
               LookFront();
               break;      
         case IR_BUTTON_POWER:
               LookUp();
               break;      
         case IR_BUTTON_MENU:
               LookDown();
               break;              
         case IR_BUTTON_PLAY:
               if(++mode==2) mode=0;
               delay(100);
               break;
           default:
       //    Stop();
           break;
        }
    }
    else
    {
      if(mode==0)Stop();
      else if(millis()%50 == 0)
      {
        ultrCarProcess();
      }
   
      
    }
}

  

void Forward()
{
  MotorL.run(-moveSpeed);
  MotorR.run(-moveSpeed);
}
void Backward()
{
  MotorL.run(moveSpeed);
  MotorR.run(moveSpeed);
}

void BackwardAndTurnLeft()
{
  MotorL.run(moveSpeed/2);
  MotorR.run(moveSpeed);
}
void BackwardAndTurnRight()
{
  MotorL.run(moveSpeed);
  MotorR.run(moveSpeed/2);
}
void TurnLeft()
{
  MotorL.run(-moveSpeed);
  MotorR.run(moveSpeed);
}
void TurnRight()
{
  MotorL.run(moveSpeed);
  MotorR.run(-moveSpeed);
}
void Stop()
{
  MotorL.run(0);
  MotorR.run(0);
}
void ChangeSpeed(int spd)
{
  moveSpeed = spd;
}

void LookRight()
{
  if (camHorizontal >2){ 
  camHorizontal = camHorizontal -8;
  servoDriver2.write(camHorizontal);
  Serial.println(camHorizontal);
}
}

void LookLeft()
{
  if (camHorizontal <180){
  camHorizontal = camHorizontal +8;
  servoDriver2.write(camHorizontal);
  Serial.println(camHorizontal);
}
}

void LookFront()
{
  servoDriver1.write(145);
  servoDriver2.write(90);
}

void LookUp()
{
  if (camVertical >25){
  camVertical = camVertical -8;
  servoDriver1.write(camVertical);
  Serial.println(camVertical);
}
}

void LookDown()
{
  if (camVertical <200){
  camVertical = camVertical +8;
  servoDriver1.write(camVertical);
  Serial.println(camVertical);
}
}


void beep(int beeps, int length, int pause)
{
  for (int i = 0; i < beeps; i +=1){
  buzzerOn();
  delay(length);
  buzzerOff();
  delay(pause);  
}
}

void sonarBeep()
{
  distance = UltrasonicSensor.distanceCm();
  Serial.println(distance);
  if(sonar == true && distance<75)
  {
    for (int j = 0; j < distance; j +=1){
    beep(1, 100, distance*2);
    }
  }
}

void ultrCarProcess()
{
  distance = UltrasonicSensor.distanceCm();
  Serial.println(distance);
  randomSeed(analogRead(A4));
  if(distance>20&&distance<40)
  {
    randnum=random(300);
    if(randnum > 190 && !rightflag)
    {
      leftflag=true;
      TurnLeft();   
    }
    else
    {
      rightflag=true;
      TurnRight();  
    }
  }
  else if(distance<20)
  {
     randnum=random(300);
    if(randnum > 190)BackwardAndTurnLeft();
    else BackwardAndTurnRight();
  }
  else
  {
      leftflag=false;
      rightflag=false;
      Forward();
  }
}
