#include <Servo.h>

#define FALL_DISTANCE_CM_THRESHOLD 25
#define FALL_DISTANCE_CM_THRESHOLD_LOW 2

const int buttonPin = 2;  

// Variables will change:     
int buttonState;            
int lastButtonState = LOW;  


unsigned long lastDebounceTime = 0;  
unsigned long debounceDelay = 50;   
// Sonar Pins
const int pingPin = 7; // Trigger Pin of Ultrasonic Sensor
const int echoPin = 6; // Echo Pin of Ultrasonic Sensor

Servo myservo;

//sonar outputs
long cm;

void setup() {
  Serial.begin(9600);
  // pinMode(2, INPUT_PULLUP);
  pinMode(buttonPin, INPUT);
}

void servo720Rotation(){
  myservo.attach(9);
  myservo.write(0);
  while (cm > FALL_DISTANCE_CM_THRESHOLD || cm < FALL_DISTANCE_CM_THRESHOLD_LOW){
    sonarUpdate();
  }

  myservo.write(90);
  // // delay(5000);
  myservo.detach(); 
}

void sonarUpdate(){
  pinMode(pingPin, OUTPUT);
  digitalWrite(pingPin, LOW);
  delayMicroseconds(2);
  digitalWrite(pingPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(pingPin, LOW);
  pinMode(echoPin, INPUT);
  long duration = pulseIn(echoPin, HIGH);
  cm = microsecondsToCentimeters(duration);
  Serial.print(cm);
  Serial.print("cm");
  Serial.println();
  delay(100);
}

void loop() {
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


  sonarUpdate();
}



long microsecondsToCentimeters(long microseconds) {
   return microseconds / 29 / 2;
}