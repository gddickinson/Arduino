#include <Makeblock.h>
#include <Arduino.h>
#include <SoftwareSerial.h>
#include <Wire.h>


MePort output(PORT_7); 

int Rightspeed = 0;
boolean Rwheelstate = false;
boolean NewRwheelstate = false;
int y = output.Aread1();



void setup(){
   Serial.begin(9600);

   OCR0A = 0xAF;            // use the same timer as the millis() function
   TIMSK0 |= _BV(OCIE0A);
   
   if (y<500){
    Rwheelstate = false;
   }
   else
    Rwheelstate = true;
}
   

void loop(){
   // do something amazing
}

int ticks = 0;
ISR(TIMER0_COMPA_vect){
   ticks++;
   
   if(ticks >= 100){
      ticks = 0;
      int x = output.Aread1();
      if(x<500){
        NewRwheelstate = false;
      }
      else
         NewRwheelstate = true;
      }
      if (NewRwheelstate == Rwheelstate){
        Rightspeed -= 1;
        if (Rightspeed <0){
          Rightspeed = 0;
        }
        Serial.println(Rightspeed);
      }
        else
        Rightspeed += 100;
        NewRwheelstate = Rwheelstate;
        Serial.println(Rightspeed);
      }
         
