////DHT
#include <dht.h>

dht DHT;

#define DHT22_PIN 5

struct
{
    uint32_t total;
    uint32_t ok;
    uint32_t crc_error;
    uint32_t time_out;
    uint32_t connect;
    uint32_t ack_l;
    uint32_t ack_h;
    uint32_t unknown;
} stat = { 0,0,0,0,0,0,0,0};
/////////////////////////////////////////////////////////////////////////////////////

////LCD_Display
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

#define I2C_ADDR    0x27  // Define I2C Address where the PCF8574A is
                          // Address can be changed by soldering A0, A1, or A2
                          // Default is 0x27

// map the pin configuration of LCD backpack for the LiquidCristal class
#define BACKLIGHT_PIN 3
#define En_pin  2
#define Rw_pin  1
#define Rs_pin  0
#define D4_pin  4
#define D5_pin  5
#define D6_pin  6
#define D7_pin  7

LiquidCrystal_I2C lcd(I2C_ADDR,
                      En_pin,Rw_pin,Rs_pin,D4_pin,D5_pin,D6_pin,D7_pin,
                      BACKLIGHT_PIN, POSITIVE);
/////////////////////////////////////////////////////////////////////////////////////

// buttons
const int buttonPin1 = 2;     // the number of the pushbutton pin
const int buttonPin2 = 3;     // the number of the pushbutton pin
const int buttonPin3 = 6;     // the number of the pushbutton pin
const int buttonPin4 = 7;     // the number of the pushbutton pin

const int ledPin =  13;      // the number of the LED pin

// variables will change:
int buttonState1 = 0;         // variable for reading the pushbutton status
int buttonState2 = 0;         // variable for reading the pushbutton status
int buttonState3 = 0;         // variable for reading the pushbutton status
int buttonState4 = 0;         // variable for reading the pushbutton status

////////////////////////////////////////////////////////////////////////////////////


// fans
const int fanPin1 = 8;
///////////////////////////////////////////////////////////////////////////////////

void setup()

/////DHT
{
    Serial.begin(115200);
    Serial.println("dht22_test.ino");
    Serial.print("LIBRARY VERSION: ");
    Serial.println(DHT_LIB_VERSION);
    Serial.println();
    Serial.println("Type,\tstatus,\tHumidity (%),\tTemperature (C)\tTime (us)");

//////////////////////////////////////////////////////////////////////////////////////

    lcd.begin(20,4);        // 20 columns by 4 rows on display

    lcd.setBacklight(HIGH); // Turn on backlight, LOW for off

    lcd.setCursor ( 0, 0 );            // go to the top left corner
    lcd.print("Time:    "); // write this string on the top row
    lcd.setCursor ( 0, 1 );            // go to the 2nd row
    lcd.print("Temp:    "); // pad string with spaces for centering
    lcd.setCursor ( 0, 2 );            // go to the third row
    lcd.print("Humidity:"); // pad with spaces for centering
    lcd.setCursor ( 0, 3 );            // go to the fourth row
    lcd.print("Set-");
/////////////////////////////////////////////////////////////////////////////////////


////buttons

    // initialize the LED pin as an output:
    pinMode(ledPin, OUTPUT);
    // initialize the pushbutton pin as an input:
    pinMode(buttonPin1, INPUT);
    pinMode(buttonPin2, INPUT);
    pinMode(buttonPin3, INPUT);
    pinMode(buttonPin4, INPUT);
    
//////////////////////////////////////////////////////////////////////////////////////

// fans
    // initialize the FAN pin as an output:
    pinMode(fanPin1, OUTPUT);

//////////////////////////////////////////////////////////////////////////////////////
}

//clock variables
int n = 1; // a global variable to track number of display refreshes
int h=12; 
int m; 
int s; 
int flag; 
int TIME; 
const int hs=8; 
const int ms=9; 
int state1; 
int state2;
////////////////////////////////////////////////////////////////////////////////////

// temp and humidity control knob settings
int potpin1 = 1;
int potpin2 = 2;

int temp_set = 0;
int humid_set = 0;

int t_min = 5;
int t_max = 99;

int h_min = 0;
int h_max = 99;

int fanSpeed1 = 0;
///////////////////////////////////////////////////////////////////////////////////////////

void loop() {
  // put your main code here, to run repeatedly:

    s=s+1;

    // READ DATA
    Serial.print("DHT22, \t");

    uint32_t start = micros();
    int chk = DHT.read22(DHT22_PIN);
    uint32_t stop = micros();

    stat.total++;
    switch (chk)
    {
    case DHTLIB_OK:
        stat.ok++;
        Serial.print("OK,\t");
        break;
    case DHTLIB_ERROR_CHECKSUM:
        stat.crc_error++;
        Serial.print("Checksum error,\t");
        break;
    case DHTLIB_ERROR_TIMEOUT:
        stat.time_out++;
        Serial.print("Time out error,\t");
        break;
    case DHTLIB_ERROR_CONNECT:
        stat.connect++;
        Serial.print("Connect error,\t");
        break;
    case DHTLIB_ERROR_ACK_L:
        stat.ack_l++;
        Serial.print("Ack Low error,\t");
        break;
    case DHTLIB_ERROR_ACK_H:
        stat.ack_h++;
        Serial.print("Ack High error,\t");
        break;
    default:
        stat.unknown++;
        Serial.print("Unknown error,\t");
        break;
    }
    // DISPLAY DATA
    Serial.print(DHT.humidity, 1);
    Serial.print(",\t");
    Serial.print(DHT.temperature, 1);
    Serial.print(",\t");
    Serial.print(stop - start);
    Serial.println();

    


   lcd.setCursor (7,0);
   if (h < 10) lcd.print(" ");                     
   lcd.print(h); 
   lcd.print(":");
   if (m < 10) lcd.print(0); 
   lcd.print(m);
   lcd.print(":");
   if (s < 10) lcd.print(0); 
   lcd.print(s); 
   if(flag<12)lcd.print(" AM"); 
   if(flag==12)lcd.print(" PM"); 
   if(flag>12)lcd.print(" PM"); 
   if(flag==24)flag=0;
                                 
   lcd.setCursor (10,1);         
   lcd.print(DHT.temperature,1);
   lcd.print(" C");                                 

   lcd.setCursor (10,2);         
   lcd.print(DHT.humidity,1);
   lcd.print(" %");

   lcd.setCursor (4,4);         
   lcd.print("Temp: ");
   temp_set = analogRead(potpin1);
   temp_set = map(temp_set, 0, 1023, t_min , t_max);
   if (temp_set<10) lcd.print(0); 
   lcd.print(temp_set); 

   lcd.setCursor (13,4);
   lcd.print("Hum: ");
   humid_set = analogRead(potpin2);
   humid_set = map(humid_set, 0, 1023, h_min , h_max);
   if (humid_set<10) lcd.print(0); 
   lcd.print(humid_set); 
              
 

    if (stat.total % 20 == 0)
    {
        Serial.println("\nTOT\tOK\tCRC\tTO\tCON\tACK_L\tACK_H\tUNK");
        Serial.print(stat.total);
        Serial.print("\t");
        Serial.print(stat.ok);
        Serial.print("\t");
        Serial.print(stat.crc_error);
        Serial.print("\t");
        Serial.print(stat.time_out);
        Serial.print("\t");
        Serial.print(stat.connect);
        Serial.print("\t");
        Serial.print(stat.ack_l);
        Serial.print("\t");
        Serial.print(stat.ack_h);
        Serial.print("\t");
        Serial.print(stat.unknown);
        Serial.println("\n");
    }


/////buttons
   // read the state of the pushbutton value:
    buttonState1 = digitalRead(buttonPin1);

   // check if the pushbutton is pressed.
   // if it is, the buttonState is HIGH:
    if (buttonState1 == HIGH) {
     // turn LED on:
     digitalWrite(ledPin, HIGH);
     if(h>1)h=h-1;
   } else {
     // turn LED off:
     digitalWrite(ledPin, LOW);
    }

    buttonState2 = digitalRead(buttonPin2);

   // check if the pushbutton is pressed.
   // if it is, the buttonState is HIGH:
    if (buttonState2 == HIGH) {
     // turn LED on:
     digitalWrite(ledPin, HIGH);
     if(h<12)h=h+1;
   } else {
     // turn LED off:
     digitalWrite(ledPin, LOW);
    }

    buttonState3 = digitalRead(buttonPin3);

   // check if the pushbutton is pressed.
   // if it is, the buttonState is HIGH:
    if (buttonState3 == HIGH) {
     // turn LED on:
     digitalWrite(ledPin, HIGH);
     if(m>1)m=m-1;
   } else {
     // turn LED off:
     digitalWrite(ledPin, LOW);
    }

    buttonState4 = digitalRead(buttonPin4);

   // check if the pushbutton is pressed.
   // if it is, the buttonState is HIGH:
    if (buttonState4 == HIGH) {
     // turn LED on:
     digitalWrite(ledPin, HIGH);
     if(m<60)m=m+1;
   } 
    else {
     // turn LED off:
     digitalWrite(ledPin, LOW);
    }


//fan control

  fanSpeed1 = 200;
  //check if temp reading higher than set_temp
  if (DHT.temperature > temp_set) {
    analogWrite(fanPin1, fanSpeed1);
  }
  else {
    analogWrite(fanPin1, 0);
  }
    
////////////////////////////////////////////

    
    delay(1000);

    if(s==60)
    { 
    s=0; 
    m=m+1; 
    } 

    if(m==60) 
    { 
    m=0; 
    h=h+1; 
    flag=flag+1; 
    }
     
    if(h==13) 
    { 
    h=1; 
    }
}
