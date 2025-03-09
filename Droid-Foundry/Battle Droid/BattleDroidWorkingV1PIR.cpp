#include <Arduino.h>
#include <Servo.h>

// Define servo objects (180-degree servos)
Servo verticalServo;
Servo horizontalServo;

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

// Sleep configuration
#define TEST_MODE false  // Set to false for normal operation

#if TEST_MODE
const unsigned long sleepInterval = 7000;  // 7 seconds for testing
#else
const unsigned long sleepInterval = 10 * 60 * 1000;  // 10 minutes for normal operation
#endif

bool sleeping = false;
unsigned long lastMotionTime = 0;

// Function prototypes
void initialVerticalServoSequence();
void initialHorizontalServoSequence();
void moveServoIfNeeded(Servo &servo, int position);
void moveServoWithOscillationDamping(Servo &servo, int targetPosition);
void detachServos();
void enterSleepMode();
void wakeAttempt();

void setup() {
  pinMode(pirPin, INPUT);
  Serial.begin(9600);

  // Attach servos to pins
  verticalServo.attach(verticalPin);
  horizontalServo.attach(horizontalPin);

  // Perform startup sequence
  initialVerticalServoSequence();
  initialHorizontalServoSequence();

  // Home both servos
  moveServoIfNeeded(verticalServo, verticalHomePosition);
  moveServoIfNeeded(horizontalServo, horizontalHomePosition);

  // Detach servos to prevent ticking
  detachServos();

  lastMotionTime = millis(); // Initialize last motion time
  randomSeed(analogRead(0));
}

void initialVerticalServoSequence() {
  Serial.println("Starting vertical servo sequence");
  moveServoWithOscillationDamping(verticalServo, verticalUpLimit);
  delay(700);
  moveServoWithOscillationDamping(verticalServo, verticalDownLimit);
  delay(700);
  moveServoWithOscillationDamping(verticalServo, verticalHomePosition);
  delay(700);
}

void initialHorizontalServoSequence() {
  Serial.println("Starting horizontal servo sequence");
  moveServoWithOscillationDamping(horizontalServo, horizontalLeftLimit);
  delay(700);
  moveServoWithOscillationDamping(horizontalServo, horizontalRightLimit);
  delay(700);
  moveServoWithOscillationDamping(horizontalServo, horizontalHomePosition);
  delay(700);
}

void loop() {
  int motion = digitalRead(pirPin);

  if (motion == HIGH) {
    if (sleeping) {
      wakeAttempt(); // Try to wake up if sleeping
    } else {
      lastMotionTime = millis(); // Reset sleep timer
      Serial.println("Motion detected!");

      // Attach servos before moving
      verticalServo.attach(verticalPin);
      horizontalServo.attach(horizontalPin);

      // Action 1: Normal motion-detection behavior
      int verticalInterval = random(1000, 5000);  // 1 to 5 seconds
      int randomVerticalPosition = random(verticalDownLimit, verticalUpLimit + 1);

      moveServoIfNeeded(verticalServo, randomVerticalPosition);
      delay(verticalInterval);

      // Hold the position for 2 seconds
      moveServoIfNeeded(verticalServo, verticalHomePosition);
      delay(3000);

      // More frequent horizontal motion
      int horizontalInterval = random(1500, 4000); // 1.5 to 4 seconds
      int randomHorizontalPosition = random(horizontalLeftLimit, horizontalRightLimit + 1);

      moveServoWithOscillationDamping(horizontalServo, randomHorizontalPosition);
      delay(horizontalInterval);

      // Return to home position before sleep check
      moveServoWithOscillationDamping(horizontalServo, horizontalHomePosition);
      moveServoIfNeeded(verticalServo, verticalHomePosition); // Ensure head is at home when awake

      // Detach servos to prevent ticking
      detachServos();
    }
  }

  // Action 2: Go to sleep if idle for X time (7 sec or 10 min)
  if (!sleeping && millis() - lastMotionTime > sleepInterval) {
    enterSleepMode();
  }
}

void enterSleepMode() {
  Serial.println("Entering sleep mode...");

  // Ensure horizontal servo is at home position before sleeping
  horizontalServo.attach(horizontalPin);
  moveServoWithOscillationDamping(horizontalServo, horizontalHomePosition);

  // Ensure vertical servo is attached before trying to move it
  verticalServo.attach(verticalPin);

  // Optional: Slow head bow before sleeping
  moveServoWithOscillationDamping(verticalServo, verticalDownLimit);
  delay(1000);
  moveServoWithOscillationDamping(verticalServo, verticalDownLimit); // Reinforce head staying down

  // Set sleep state AFTER the head movement finishes
  sleeping = true;

  // Detach horizontal, keep vertical attached to hold the head position during sleep
  horizontalServo.detach(); 
}

void wakeAttempt() {
  int wakeChance = random(1, 4); // 1 out of 3 chance to wake up

  if (wakeChance == 1) {
    Serial.println("Waking up...");
    sleeping = false;
    lastMotionTime = millis();

    // Return head to home position when waking up
    verticalServo.attach(verticalPin);
    moveServoIfNeeded(verticalServo, verticalHomePosition);
  } else {
    Serial.println("Ignoring motion while sleeping...");

    // Only raise head with 1 out of 5 chance while sleeping
    if (random(1, 6) == 1) { // 1 out of 5 chance
      Serial.println("Raising head while ignoring motion...");
      verticalServo.attach(verticalPin);
      moveServoWithOscillationDamping(verticalServo, verticalUpLimit);
      delay(1000);
      moveServoWithOscillationDamping(verticalServo, verticalDownLimit);
    }
  }
}

void moveServoIfNeeded(Servo &servo, int position) {
  if (servo.read() != position) {
    servo.write(position);
  }
}

void moveServoWithOscillationDamping(Servo &servo, int targetPosition) {
  int currentPosition = servo.read();
  int step = (targetPosition > currentPosition) ? 1 : -1;

  while (currentPosition != targetPosition) {
    currentPosition += step;
    servo.write(currentPosition);
    if (abs(targetPosition - currentPosition) < 10) {
      delay(50);  // Slow down as the servo approaches the target position
    } else if (abs(targetPosition - currentPosition) < 20) {
      delay(30);
    } else {
      delay(15);
    }
  }
}

void detachServos() {
  verticalServo.detach();
  horizontalServo.detach();
}