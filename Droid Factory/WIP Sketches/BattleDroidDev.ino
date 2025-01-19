#include <Arduino.h>
#include <Servo.h>
#Cloning from WorkingPIRv1 since the servo layout is the same

// Define servo objects
Servo verticalServo;
Servo horizontalServo;

// Define pin numbers
const int verticalPin = 9;
const int horizontalPin = 10;
const int pirPin = 2;
const int redLedPin = 3;
const int blueLedPin = 4;


// Define home positions
const int verticalHomePosition = 90;
const int horizontalHomePosition = 90;

void setup() {
  // Configure pin modes
  pinMode(pirPin, INPUT);
  pinMode(redLedPin, OUTPUT);
  pinMode(blueLedPin, OUTPUT);

  digitalWrite(redLedPin, LOW);
  digitalWrite(blueLedPin, LOW);

  // Attach servos to pins
  verticalServo.attach(verticalPin);
  horizontalServo.attach(horizontalPin);

  // Move servos to home position
  verticalServo.write(verticalHomePosition);
  horizontalServo.write(horizontalHomePosition);

  // Initialize random seed
  randomSeed(analogRead(0));

  // Initialize serial communication
  Serial.begin(9600);
}

void loop() {
  // Read motion sensor pin value
  int motion = digitalRead(pirPin);

  if (motion == HIGH) {
    Serial.println("Motion detected!");

    // Turn on the blue LED when motion is detected
    digitalWrite(blueLedPin, HIGH);
    // Turn off the red LED when motion is detected
    digitalWrite(redLedPin, LOW);

    // Randomly move the vertical servo
    int randomInterval = random(1000, 5000);  // Get a random interval between 1 and 5 seconds
    int randomDirection = random(0, 2);       // Get a random direction (0 or 1)

    if (randomDirection == 0) {
      verticalServo.write(0);   // Move the vertical servo to the down position
    } else {
      verticalServo.write(180); // Move the vertical servo to the up position
    }

    delay(randomInterval);  // Wait for the random interval

    // Hold the position for 2 seconds
    verticalServo.write(verticalHomePosition);  // Move the vertical servo to the middle position
    delay(2000);                                 // Hold the position for 2 seconds

    // Return to home position
    verticalServo.write(verticalHomePosition);  // Move the vertical servo to the home position

    // Randomly sweep the horizontal servo
    randomInterval = random(2000, 5000);  // Get a random interval between 2 and 5 seconds
    int randomAngle = random(0, 161);     // Get a random angle between 0 and 180 degrees

    horizontalServo.write(randomAngle);
    delay(randomInterval);

    } else {
    // Turn on the red LED when no motion is detected and no servo is running
    digitalWrite(redLedPin, HIGH);
    
    // Reset blue LED to off
    digitalWrite(blueLedPin, LOW);
  }
}
