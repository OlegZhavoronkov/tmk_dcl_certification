/*
 From "Hello World!" example: 
 
 Demonstrates the use a 16x2 LCD display.  The LiquidCrystal
 library works with all LCD displays that are compatible with the
 Hitachi HD44780 driver. There are many of them out there, and you
 can usually tell them by the 16-pin interface.

  The circuit:
 * LCD RS pin to digital pin 12
 * LCD Enable pin to digital pin 11
 * LCD D4 pin to digital pin 5
 * LCD D5 pin to digital pin 4
 * LCD D6 pin to digital pin 3
 * LCD D7 pin to digital pin 2
 * LCD R/W pin to ground
 * LCD VSS pin to ground
 * LCD VCC pin to 5V
 * 10K resistor:
 * ends to +5V and ground
 * wiper to LCD VO pin (pin 3)

 Library originally added 18 Apr 2008
 by David A. Mellis
 library modified 5 Jul 2009
 by Limor Fried (http://www.ladyada.net)
 example added 9 Jul 2009
 by Tom Igoe
 modified 22 Nov 2010
 by Tom Igoe

 This example code is in the public domain.

 http://www.arduino.cc/en/Tutorial/LiquidCrystal
 */

// include the LiquidCrystal library code:
#include <LiquidCrystal.h>

// initialize the library with the numbers of the interface pins
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

const int ledPin =  LED_BUILTIN; // output bulb (or diode)
int ledState = LOW; // bulb is off
const int incButton = 6; // increase pulse button
const int decButton = 7; // decrease pulse button
int pulseLength = 1000; // current pulse length
const int pulseDelta = 100; // if button pressed, pulseLength should differ
unsigned long previousMillis = 0; // from "Blink without delay"
int needClear = 0; // do we need to clear lcd display?

void setup() {
  // set up the LCD's number of columns and rows:
  lcd.begin(16, 2);
  
  Serial.begin(9600); // log init
  
  pinMode(incButton, INPUT_PULLUP);
  pinMode(decButton, INPUT_PULLUP);
  pinMode(ledPin, OUTPUT);
}

void loop() {
  if (needClear == 1) {
    needClear = 0;
    lcd.clear();
  }
  // print the number of seconds since reset:
  lcd.setCursor(0, 0);
  lcd.print("Time:");
  lcd.print(millis() / 1000);
  // print current pulseLength
  lcd.setCursor(0, 1);
  lcd.print("Fr:");
  lcd.print(pulseLength);
  
  unsigned long currentMillis = millis();
  
  if (currentMillis - previousMillis >= pulseLength) {
    previousMillis = currentMillis;
    
    if (ledState == LOW) {
      ledState = HIGH;
    } else {
      ledState = LOW;
    }
    
    digitalWrite(ledPin, ledState);
  }
  
  int increase = digitalRead(incButton);
  int decrease = digitalRead(decButton);
  
  Serial.println(pulseLength); // log print
  
  if (increase == LOW) {
    pulseLength += pulseDelta;
    needClear = 1;
  }
  if (decrease == LOW) {
    pulseLength -= pulseDelta;
    needClear = 1;
  }
}
 
