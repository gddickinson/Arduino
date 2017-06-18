#include <Makeblock.h>
#include <Arduino.h>
#include <SoftwareSerial.h>
#include <Wire.h>


MePort output(PORT_7); 

int wheelRadius =  375; //millimeters
int wheelTeeth = 40;
int distanceTooth = ((2*PI)*wheelRadius)/wheelTeeth;
int timeInterval = 200;
int halfState = 500;
int Rstate = output.Aread1();
int NewRstate = 0;
boolean Rposition = false;
boolean NewRposition = false;


void setup(){
   Serial.begin(9600);

   OCR0A = 0xAF;            // use the same timer as the millis() function
   TIMSK0 |= _BV(OCIE0A);
   
   if (Rstate> halfState){
   Rposition = true;
   }
   else{
   Rposition = false;
}
}

void loop(){
   // do something amazing
}

int ticks = 0;
int count = 0;
ISR(TIMER0_COMPA_vect){
   ticks++;
   int Rightspeed = (count*1000)/timeInterval;
   int Rightspeed2 = Rightspeed * distanceTooth;
   int Rightspeed3 = (Rightspeed2/10)*60;
   Serial.println(Rightspeed);
   Serial.println("teeth/sec");
   Serial.println(Rightspeed2);
   Serial.println("mm/sec");
   Serial.println(Rightspeed3);
   Serial.println("cm/min");
  
   //Serial.println(count);
   //Serial.println(ticks);
  
   if(ticks >= timeInterval){
      ticks = 0;
      count = 0;
   }
   
   
   else{
     NewRstate = output.Aread1();
       if (NewRstate> halfState){
           NewRposition = true;
     }
     
         else{
         NewRposition = false;
     }
     
       if (NewRposition != Rposition){
         count ++;
         Rposition = NewRposition;
         }
           else{
             //Serial.println("noCount");
}
}
}

