/*************************************************************************
* File Name          : Makeblock_for_Scratch.ino
* Author             : Ander
* Updated            : Ander
* Version            : V1.0.1
* Date               : 04/15/2014
* Description        : Makeblock Electronic for  Scratch's Communication
* License            : CC-BY-SA 3.0
* Copyright (C) 2014 Maker Works Technology Co., Ltd. All right reserved.
* http://www.makeblock.cc/
**************************************************************************/
#include "Makeblock.h"
#include <SoftwareSerial.h>
#include <Wire.h>
MeSerial serial;
MeDCMotor motor;
MeGyro gyro;
//MeServo servo;
MeUltrasonicSensor ultrasonic;
//MeLineFinder linefinder;
//MeLimitSwitch limitswitch;
MeJoystick joystick;
//MeSoundSensor soundsensor;
MeLightSensor lightsensor;
//Me4Button button;
MeTemperature temperature;
MeRGBLed led;
MePotentiometer potentiometer;
char *device;
char *method;
int port;
char *pin;
long value;
int slot;
int ultrasonic_port=-1;
int switch_port=-1;
int switch_slot=1;
long servoTime = 0;
bool isServoRun = false;
unsigned long gyroTime[3]={0,0,0};
typedef struct{
  int type;
  int mode;
  int slot[2];
}Module;
Module ports[10];
Module pins[18];
int analogPins[6] = {A0,A1,A2,A3,A4,A5};
void addDevice(){
  if(port>0){
    int index = port-1;
    if(strcmp(device,"motor")==0){
      ports[index].type=1;
    }else if(strcmp(device,"servo")==0){
      ports[index].type=2;
      ports[index].slot[slot-1]=1;
    }else if(strcmp(device,"Ultrasonic")==0){
      ports[index].type=3;
    }else if(strcmp(device,"Line%20Finder")==0){
      ports[index].type=4;
    }else if(strcmp(device,"LimitSwitch")==0){
      ports[index].type=5;
      ports[index].slot[slot-1]=1;
    }else if(strcmp(device,"Light%20Sensor")==0){
      ports[index].type=6;
    }else if(strcmp(device,"Sound%20Sensor")==0){
      ports[index].type=7;
    }else if(strcmp(device,"Temperature")==0){
      ports[index].type=8;
      ports[index].slot[slot-1]=1;
    }else if(strcmp(device,"Joystick")==0){
      ports[index].type=9;
    }else if(strcmp(device,"Button")==0){
      ports[index].type=10;
    }else if(strcmp(device,"Potentiometer")==0){
      ports[index].type=11;
    }
  }
}
void runDevice(){
  if(port>0){
    int index = port-1;
    Module portModule = ports[index];
    if(strcmp(device,"motor")==0){
      motor.reset(port);
      motor.run(value);
    }else if(strcmp(device,"servo")==0){
//      servo.reset(port,slot);
//      servo.begin();
//      servo.write(value);
      isServoRun = true;
      servoTime = millis();
    }else if(strcmp(device,"led")==0){
      led.reset(port);
      if(slot==0){
       for(int i=0;i<led.getNumber();i++){
        led.setColorAt(i,value);
       } 
      }else{
        led.setColorAt(slot-1,value);
      }
      led.show();
    }
  }
}
void checkDevice(){
  int i,portNum;
  for(i=0;i<10;i++){
    portNum = i+1;
    if(ports[i].type>0){
      switch(ports[i].type){
        case 3:{
          ultrasonic.reset(portNum);
          int dist = ultrasonic.distanceCm();
          sendCommand("Ultrasonic/Port",portNum,-1,dist);
        }
        break;
        case 4:{
//          linefinder.reset(portNum);
//          int state = linefinder.readSensors();
//          sendCommand("LineFinder/Port",portNum,-1,state);
        }
        break;
        case 5:{
//          limitswitch.reset(portNum,ports[i].slot[0]==1?1:2);
//          int touched = limitswitch.touched();
//          sendCommand("LimitSwitch/Port",portNum,ports[i].slot[0]==1?1:2,touched==0,true);
        }
        break;
        case 6:{
          lightsensor.reset(portNum);
          int light = lightsensor.strength();
          sendCommand("LightSensor/Port",portNum,-1,light);
        }
        break;
        case 7:{
//          soundsensor.reset(portNum);
//          int sound = soundsensor.strength();
//          sendCommand("SoundSensor/Port",portNum,-1,sound);
        }
        break;
        case 8:{
          temperature.reset(portNum,ports[i].slot[0]==1?1:2);
          float t = temperature.temperature();
          if(t==-100){
            t = temperature.temperature();
          }
          if(t>-100){
            sendCommand("Temperature/Port",portNum,ports[i].slot[0]==1?1:2,t);
          }
        }
        break;
        case 9:{
          joystick.reset(portNum);
          int dist = joystick.readX();
          serial.print("Joystick/Port");
          serial.print(portNum);
          serial.print("/X-Axis");
          serial.print(" ");
          serial.println(dist);
          delay(60);
          dist = joystick.readY();
          serial.print("Joystick/Port");
          serial.print(portNum);
          serial.print("/Y-Axis");
          serial.print(" ");
          serial.println(dist);
        }
        break;
        case 10:{
//          button.reset(portNum);
//          int state = button.pressed();
//          sendCommand("Button/Port",portNum,-1,state);
        }
        break;
        case 11:{
          potentiometer.reset(portNum);
          sendCommand("Potentiometer/Port",portNum,-1,potentiometer.read());
        }
        break;
      }
    }
  }
  for(i=0;i<18;i++){
    if(pins[i].type==1){
      sendCommand("digital/D",i+2,-1,digitalRead(i+2));
    }else if(pins[i].type==2){
      sendCommand("analog/A",i-12,-1,analogRead(analogPins[i-12]));
    }
  }
  if(isServoRun&&millis()-servoTime>500){
     isServoRun = false;
//     servo.detach(); 
  }
  if(millis()-gyroTime[0]>150){
    gyro.update();
    double ax = gyro.angleX();
    if(ax!=0){
      serial.print("Gyro/Port0/X-Axis");
      serial.print(" ");
      serial.println(gyro.angleX());
      gyroTime[0]=millis();
    }
  }else if(millis()-gyroTime[1]>120){
    double ay = gyro.angleY();
    if(ay!=0){
        serial.print("Gyro/Port0/Y-Axis");
        serial.print(" ");
        serial.println(gyro.angleY());
        gyroTime[1]=millis();
      }
  }else if(millis()-gyroTime[2]>100){
    double az = gyro.angleZ();
    if(az!=0){
        serial.print("Gyro/Port0/Z-Axis");
        serial.print(" ");
        serial.println(gyro.angleZ());
        gyroTime[2]=millis();
      }
  }
}
void sendCommand(const char*cmd,int cPort,int cSlot,double v){
  sendCommand(cmd,cPort,cSlot,v,false);
}
void sendCommand(const char*cmd,int cPort,int cSlot,double v,bool isBool){
  serial.print(cmd);
  serial.print(cPort);
  if(cSlot>-1){
    serial.print("/Slot");
    serial.print(cSlot);
  }
  serial.print(" ");
  if(isBool){
    serial.println(v!=0?"true":"false");
  }else{
    serial.println(v);
  }
}
void setPinMode(){
  if(value==0){
    return;
  }
  if(pin[0]=='D'){
    pin[0]='0';
    int p = atoi(pin);
    pinMode(p,value==2?OUTPUT:INPUT);
    if(value==1){
      pins[p-2].type = 1;
    }else{
      pins[p-2].type = 0;
    }
  }else if(pin[0]=='A'){
    pin[0]='0';
    int p = atoi(pin);
    pinMode(analogPins[p],value==2?OUTPUT:INPUT);
    if(value==1){
      pins[p+12].type = 2;
    }else{
      pins[p+12].type = 0;
    }
  }
}
void pinWrite(){
  if(pin[0]=='D'){
    pin[0]='0';
    digitalWrite(atoi(pin),value==2?HIGH:LOW);
  }else if(pin[0]=='A'){
    pin[0]='0';
    digitalWrite(analogPins[atoi(pin)],value==2?HIGH:LOW);
  }
}
void callResponse(){
  Serial.println("v1.0.0421"); 
}
long currentTime = 0;
long sampleTime = 100;
long baudrate = 115200;
byte responseTime=0;
void setup() {
  serial.begin(baudrate);
  serial.println("Application Start");
  buzzerOn();
  delay(100);
  buzzerOff();
  gyro.begin();
}
void loop() {
  long time = millis()-currentTime;
  
    if(time>sampleTime||time<0){
      currentTime = millis();
      checkDevice();
    }
  if(serial.paramAvailable()){
    device = serial.getParamCode("device");
    port = serial.getParamValue("port");
    pin = serial.getParamCode("pin");
    value = serial.getParamValue("value");
    slot = serial.getParamValue("slot");
    method = serial.getParamCode("method");
    if(strcmp(device,"reset")==0){
      int i;
      for(i=0;i<10;i++){
         ports[i].type = 0;
         ports[i].slot[0] = 0;
         ports[i].slot[1] = 0;
      }
      for(i=0;i<18;i++){
         pins[i].type = 0;
         pins[i].slot[0] = 0;
         pins[i].slot[1] = 0;
      }
      motor.reset(M1);
      motor.stop();
      motor.reset(M2);
      motor.stop();
      motor.reset(PORT_1);
      motor.stop();
      motor.reset(PORT_2);
      motor.stop();
      return;
    }
    if(strcmp(device,"ver")==0){
      callResponse();
      return;
    }
    if(strcmp(method,"add")==0){
     addDevice(); 
     return;
    }else if(strcmp(method,"run")==0){
     runDevice(); 
     return;
    }else if(strcmp(method,"mode")==0){
     setPinMode(); 
     return;
    }else if(strcmp(method,"write")==0){
     pinWrite(); 
     return;
    }
  }else{
  }
}
