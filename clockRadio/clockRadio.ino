#include <Wire.h>
#include <LiquidCrystal_PCF8574.h>

LiquidCrystal_PCF8574 lcd(0x27);  // set the LCD address to 0x27 for a 16 chars and 2 line display

int show;

void setup()
{
  int error;

  Serial.begin(115200);
  Serial.println("LCD...");

  while (! Serial);

  Serial.println("Dose: check for LCD");

  // See http://playground.arduino.cc/Main/I2cScanner
  Wire.begin();
  Wire.beginTransmission(0x27);
  error = Wire.endTransmission();
  Serial.print("Error: ");
  Serial.print(error);

  if (error == 0) {
    Serial.println(": LCD found.");

  } else {
    Serial.println(": LCD not found.");
  } // if

  lcd.begin(20, 4); // initialize the lcd
  show = 0;


} // setup()

void loop()
{

//turn display on  
//lcd.setBacklight(255);

//get serial data
if (Serial.available()) {

  //lcd.clear();

  while (Serial.available() >0) {
    String incomingData = Serial.readString();
    //Serial.println(incomingData);
    String data1 = incomingData.substring(0,19);
    String data2 = incomingData.substring(20,39);
    String data3 = incomingData.substring(40,59);
    String data4 = incomingData.substring(60,79);

    if (data1.length() < 9){
      lcd.clear();
      lcd.noDisplay();
      lcd.setBacklight(0);
    }
    else{
      lcd.display();
      lcd.setBacklight(255);
    }

    lcd.setCursor(0, 0);
    lcd.print(data1);
    lcd.setCursor(0, 1);
    lcd.print(data2);
    lcd.setCursor(0, 2);
    lcd.print(data3);
    lcd.setCursor(0, 4);
    lcd.print(data4);

  }
}  
} // loop()

//COMMANDS
//lcd.home();
//lcd.setBacklight(0);
//lcd.setBacklight(255);
//lcd.blink();
//lcd.noBlink();
//lcd.cursor();
//lcd.noCursor();
//lcd.display();
//lcd.noDisplay();
//lcd.clear();
//lcd.setCursor(0, 1);
//lcd.print("*** second line.");
//lcd.autoscroll();
//lcd.noAutoscroll();
//lcd.scrollDisplayLeft();
//lcd.scrollDisplayRight();
//
