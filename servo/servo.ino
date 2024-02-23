#include <Servo.h>

const int buttonPin = 2;  

// Variables will change:     
int buttonState;            
int lastButtonState = LOW;  


unsigned long lastDebounceTime = 0;  
unsigned long debounceDelay = 50;    

Servo myservo;  

void setup() {
  Serial.begin(9600);
  // pinMode(2, INPUT_PULLUP);
  pinMode(buttonPin, INPUT);
}

void servo720Rotation(){
  myservo.attach(6);
  myservo.write(0);
  delay(1000);
  myservo.write(90);
  // delay(5000);
  myservo.detach(); 
}

void loop() {
  // Set rotation speed and direction
  // 0.5 ms to 2.5 ms corresponds to full speed in one direction to full speed in the other direction
  int reading = digitalRead(buttonPin);

  // check to see if you just pressed the button
  // (i.e. the input went from LOW to HIGH), and you've waited long enough
  // since the last press to ignore any noise:

  // If the switch changed, due to noise or pressing:
  if (reading != lastButtonState) {
    // reset the debouncing timer
    lastDebounceTime = millis();
  }

  if ((millis() - lastDebounceTime) > debounceDelay) {
    // whatever the reading is at, it's been there for longer than the debounce
    // delay, so take it as the actual current state:

    // if the button state has changed:
    if (reading != buttonState) {
      buttonState = reading;

      // only toggle the LED if the new button state is HIGH
      if (buttonState == HIGH) {
        servo720Rotation();
      }
    }
  }

  // set the LED:

  // save the reading. Next time through the loop, it'll be the lastButtonState:
  lastButtonState = reading;
}