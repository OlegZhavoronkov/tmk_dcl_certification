const int ledPin =  LED_BUILTIN; // output bulb (or diode)
int ledState = LOW; // bulb is off
int incButton = 2; // increase pulse button
int decButton = 3; // decrease pulse button
int pulseLength = 1000; // current pulse length
const int pulseDelta = 1; // if button pressed, pulseLength should differ
unsigned long previousMillis = 0; // from "Blink without delay"

void setup() {
  Serial.begin(9600); // log init
  
  pinMode(incButton, INPUT_PULLUP);
  pinMode(decButton, INPUT_PULLUP);
  pinMode(ledPin, OUTPUT);
}

void loop() {
  
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
  
  Serial.println(pulseLength); // log write
  
  if (increase == LOW) {
    pulseLength += pulseDelta;
  }
  if (decrease == LOW) {
    pulseLength -= pulseDelta;
  }
}
