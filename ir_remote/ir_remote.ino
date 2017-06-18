#include <AFMotor.h>

//declaring the pins for the IN pins on the L298N
const int rightPin = 4;
const int leftPin = 5;
const int BackwardPin = 2;
const int ForwardPin = 3;

int driveTime = 1000;
int turnTime = 600;
int turnPower = 150;

#include <IRremote.h> // Include the IRremote library

/* Setup constants for SparkFun's IR Remote: */
#define NUM_BUTTONS 9 // The remote has 9 buttons
/* Define the IR remote button codes. We're only using the
   least signinficant two bytes of these codes. Each one 
   should actually have 0x10EF in front of it. Find these codes
   by running the IRrecvDump example sketch included with
   the IRremote library.*/
const uint16_t BUTTON_POWER = 0xD827; // i.e. 0x10EFD827
const uint16_t BUTTON_A = 0xF807;
const uint16_t BUTTON_B = 0x7887;
const uint16_t BUTTON_C = 0x58A7;
const uint16_t BUTTON_UP = 0xA05F;
const uint16_t BUTTON_DOWN = 0x00FF;
const uint16_t BUTTON_LEFT = 0x10EF;
const uint16_t BUTTON_RIGHT = 0x807F;
const uint16_t BUTTON_CIRCLE = 0x20DF;

/* Connect the output of the IR receiver diode to pin 11. */
int RECV_PIN = 11;
/* Initialize the irrecv part of the IRremote  library */
IRrecv irrecv(RECV_PIN);
decode_results results; // This will store our IR received codes
uint16_t lastCode = 0; // This keeps track of the last code RX'd


const int ledPin = 9;
byte ledValue = 55; // This keeps track of channel brightness
byte activeChannel = ledPin; // Start with RED as the active channel
boolean ledEnable = 1; // Start with the LED on.

void setup()
{
  Serial.begin(9600); // Use serial to debug. 
  irrecv.enableIRIn(); // Start the receiver

  /* Set up the LED pin: */

   pinMode(ledPin, OUTPUT);
   analogWrite(ledPin, ledValue);
  
  /*Set up the Motor pins:*/
  
  //Stating that the pins are OUTPUT
  pinMode(rightPin, OUTPUT);
  pinMode(leftPin, OUTPUT);
  pinMode(ForwardPin, OUTPUT);
  pinMode(BackwardPin, OUTPUT);
}

// loop() constantly checks for any received IR codes. At the
// end it updates the RGB LED.
void loop() 
{
  if (irrecv.decode(&results)) 
  {
    /* read the RX'd IR into a 16-bit variable: */
    uint16_t resultCode = (results.value & 0xFFFF);

    /* The remote will continue to spit out 0xFFFFFFFF if a 
     button is held down. If we get 0xFFFFFFF, let's just
     assume the previously pressed button is being held down */
    if (resultCode == 0xFFFF)
      resultCode = lastCode;
    else
      lastCode = resultCode;

    // This switch statement checks the received IR code against
    // all of the known codes. Each button press produces a 
    // serial output, and has an effect on the LED output.
    switch (resultCode)
    {  
      case BUTTON_POWER:
        Serial.println("Power");
        if (ledEnable) ledEnable = 0;
        else ledEnable = 1; // Flip ledEnable
        break;
      case BUTTON_A:
        Serial.println("A");
        //activeChannel = RED;
        break;
      case BUTTON_B:
        Serial.println("B");
        //activeChannel = GREEN;
        break;
      case BUTTON_C:
        Serial.println("C");
        //activeChannel = BLUE;
        break;
      case BUTTON_UP:
        Serial.println("Up - forward");
        ledValue++; // Increment brightness
        digitalWrite(ForwardPin, HIGH);
        digitalWrite(BackwardPin, LOW);
        delay(driveTime);
        break;
      case BUTTON_DOWN:
        Serial.println("Down - back");
        ledValue--; // Decrement brightness
        digitalWrite(ForwardPin, LOW);
        digitalWrite(BackwardPin, HIGH);
        delay(driveTime);
        break;
      case BUTTON_LEFT:
        Serial.println("Left");
        ledValue = 0; // Min brightness (off)
        analogWrite(leftPin, turnPower);
        analogWrite(rightPin, 0);
        delay(turnTime);
        break;
      case BUTTON_RIGHT:
        Serial.println("Right");
        ledValue =  255; // Max brightness
        analogWrite(leftPin, 0);
        analogWrite(rightPin, turnPower);
        delay(turnTime);
        break;
      case BUTTON_CIRCLE:
        Serial.println("Circle - stop");
        ledValue = 127; // Medium brightness
        analogWrite(rightPin, 0);
        analogWrite(leftPin, 0);
        digitalWrite(ForwardPin, LOW);
        digitalWrite(BackwardPin, LOW);
        break;
      default:
        Serial.print("Unrecognized code received: 0x");
        Serial.println(results.value, HEX);
        break;        
        
    } 
  

  irrecv.resume(); // Receive the next value

  // Every time through the loop, update the RGB LEDs:
  if (ledEnable)
     analogWrite(ledPin, ledValue);   
  else
      analogWrite(ledPin, 0);
    }
}
