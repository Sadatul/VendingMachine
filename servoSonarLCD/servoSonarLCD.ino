#include <Servo.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

#define FALL_DISTANCE_CM_THRESHOLD 25
#define FALL_DISTANCE_CM_THRESHOLD_LOW 2

const int buttonPin = 2;
const int servoPwmPin = 5;  

// Variables will change:     
int buttonState;            
int lastButtonState = LOW;  

// FOR LCD
LiquidCrystal_I2C lcd(0x27, 16, 2); // set the LCD address to 0x27 for a 16 chars and 2 line display

unsigned long lastDebounceTime = 0;  
unsigned long debounceDelay = 50;   
// Sonar Pins
const int pingPin = 7; // Trigger Pin of Ultrasonic Sensor
const int echoPin = 6; // Echo Pin of Ultrasonic Sensor

Servo myservo;

//sonar outputs
long cm;

void setup() {
  // Serial.begin(9600);
  // pinMode(2, INPUT_PULLUP);
  pinMode(buttonPin, INPUT);
  lcd.init();         // initialize the lcd
  lcd.backlight();    // Turn on the LCD screen backlight
}

void servo720Rotation(){
  myservo.attach(servoPwmPin);
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
  // Serial.print(cm);
  // Serial.print("cm");
  // Serial.println();
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
        if (cm > FALL_DISTANCE_CM_THRESHOLD || cm < FALL_DISTANCE_CM_THRESHOLD_LOW) servo720Rotation();
        else {
          // Serial.println("PLEASE REMOVE THE PRODUCT FROM THE SHELF FIRST");
          // lcd.setCursor(0, 0);
          // lcd.clear();
          // lcd.print("Grab your snack!");
          // // Serial.println("Blocked");
          // delay(3000);
          // lcd.clear();
          msgToLCD("Grab your snack!");
        }
      }
    }
  }

  // set the LED:

  // save the reading. Next time through the loop, it'll be the lastButtonState:
  lastButtonState = reading;


  sonarUpdate();
}

void msgToLCD(char *s){
  char s1[17] = "                ";
  char s2[17] = "                ";
  int n = strlen(s);
  int s1Size = n < 16 ? n : 16;
  int s2Start = n > 16 ? 16 : -1;

  for (int i = 0; i < s1Size; i++){
    s1[i] = s[i];
  }
  if(s2Start != -1 ){
    for (int i = 16; i < n; i++){
      s2[i - 16] = s[i];
    }
  }
  

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(s1);
  if(s2Start != -1){
    lcd.setCursor(0, 1);
    lcd.print(s2);
  }
  delay(3000);
  lcd.clear();
}

long microsecondsToCentimeters(long microseconds) {
   return microseconds / 29 / 2;
}