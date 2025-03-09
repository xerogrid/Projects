#include <Arduino.h>
#include <ServoSmooth.h> // Include the ServoSmooth library

// Define servo objects
ServoSmooth verticalServo;
ServoSmooth horizontalServo;

// Define pin numbers
const int verticalPin = 9;
const int horizontalPin = 10;
const int pirPin = 2;

// Define home positions
const int verticalHomePosition = 90;
const int horizontalHomePosition = 90;

// Define vertical servo movement range
const int verticalUpLimit = 110;
const int verticalDownLimit = 30;

// Define horizontal servo movement range
const int horizontalLeftLimit = 20;
const int horizontalRightLimit = 160;

// Function prototypes
void initialVerticalServoSequence();
void initialHorizontalServoSequence();
void detachServos();

void setup() {
  pinMode(pirPin, INPUT);
  Serial.begin(9600);

  // Attach servos to pins
  verticalServo.attach(verticalPin);
  horizontalServo.attach(horizontalPin);

  // Set speed and acceleration (more conservative)
  verticalServo.setSpeed(2); // Degrees per second (slower)
  verticalServo.setAccel(0.05); // Lower acceleration
  horizontalServo.setSpeed(2);
  horizontalServo.setAccel(0.05);

  // Perform startup sequence
  initialVerticalServoSequence();
  initialHorizontalServoSequence();

  // Home both servos
  verticalServo.write(verticalHomePosition);
  horizontalServo.write(horizontalHomePosition);

  // Detach servos to prevent ticking
  detachServos();

  randomSeed(analogRead(0));
}

void initialVerticalServoSequence() {
  Serial.println("Starting vertical servo sequence");
  verticalServo.write(verticalUpLimit);
  while (!verticalServo.tick()) delay(40); // Increase tick interval
  verticalServo.write(verticalDownLimit);
  while (!verticalServo.tick()) delay(40);
  verticalServo.write(verticalHomePosition);
  while (!verticalServo.tick()) delay(40);
}

void initialHorizontalServoSequence() {
  Serial.println("Starting horizontal servo sequence");
  horizontalServo.write(horizontalLeftLimit);
  while (!horizontalServo.tick()) delay(40); // Increase tick interval
  horizontalServo.write(horizontalRightLimit);
  while (!horizontalServo.tick()) delay(40);
  horizontalServo.write(horizontalHomePosition);
  while (!horizontalServo.tick()) delay(40);
}

void loop() {
  int motion = digitalRead(pirPin);

  if (motion == HIGH) {
    Serial.println("Motion detected!");

    verticalServo.attach(verticalPin);
    horizontalServo.attach(horizontalPin);

    int randomInterval = random(1000, 5000);
    int randomVerticalPosition = random(verticalDownLimit, verticalUpLimit + 1);

    verticalServo.write(randomVerticalPosition);
    unsigned long startTime = millis();
    while (millis() - startTime < randomInterval) {
      verticalServo.tick();
      delay(40); // Increase tick interval
    }

    verticalServo.write(verticalHomePosition);
    startTime = millis();
    while (millis() - startTime < 2000) {
      verticalServo.tick();
      delay(40);
    }

    randomInterval = random(2000, 5000);
    int randomHorizontalPosition = random(horizontalLeftLimit, horizontalRightLimit + 1);

    horizontalServo.write(randomHorizontalPosition);
    startTime = millis();
    while (millis() - startTime < randomInterval) {
      horizontalServo.tick();
      delay(40);
    }

    detachServos();
  }
}

void detachServos() {
  verticalServo.detach();
  horizontalServo.detach();
}