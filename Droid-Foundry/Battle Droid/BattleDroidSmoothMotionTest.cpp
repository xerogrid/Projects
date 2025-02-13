#include <Arduino.h>
#include <Servo.h>
#include <IRremote.h>

// ------------------------------
// Pin Definitions & Constants
// ------------------------------
const int verticalPin      = 9;
const int horizontalPin    = 10;
const int irReceiverPin    = 11;

// Servo positions and limits
const int verticalHomePosition   = 90;
const int horizontalHomePosition = 90;
const int horizontalLeftLimit    = 20;
const int horizontalRightLimit   = 160;
const int verticalUpLimit        = 160;
const int verticalDownLimit      = 20;

// Movement increments and update interval (ms)
const int servoStep = 2;          // How many degrees to move at a time
const unsigned long updateInterval = 50;  // Update every 50 ms

// ------------------------------
// Object Declarations
// ------------------------------
Servo verticalServo;
Servo horizontalServo;

// ------------------------------
// Movement Tracking
// ------------------------------
enum Direction { NONE, UP, DOWN, LEFT, RIGHT, HOME };
Direction currentDirection = NONE;
unsigned long lastUpdateTime = 0;

// Store the last non-repeat command code
uint32_t lastCommand = 0;

// ------------------------------
// Setup
// ------------------------------
void setup() {
  verticalServo.attach(verticalPin);
  horizontalServo.attach(horizontalPin);

  verticalServo.write(verticalHomePosition);
  horizontalServo.write(horizontalHomePosition);

  Serial.begin(9600);
  IrReceiver.begin(irReceiverPin, ENABLE_LED_FEEDBACK);
  Serial.println("BattleDroid with smoothing initialized.");
}

// ------------------------------
// Helper: Update Servo Positions Smoothly
// ------------------------------
void updateServos() {
  unsigned long now = millis();
  if (now - lastUpdateTime >= updateInterval) {
    lastUpdateTime = now;
    
    // Vertical servo
    int currentV = verticalServo.read();
    if (currentDirection == UP && currentV < verticalUpLimit) {
      verticalServo.write(min(currentV + servoStep, verticalUpLimit));
    } else if (currentDirection == DOWN && currentV > verticalDownLimit) {
      verticalServo.write(max(currentV - servoStep, verticalDownLimit));
    }
    
    // Horizontal servo
    int currentH = horizontalServo.read();
    if (currentDirection == RIGHT && currentH < horizontalRightLimit) {
      horizontalServo.write(min(currentH + servoStep, horizontalRightLimit));
    } else if (currentDirection == LEFT && currentH > horizontalLeftLimit) {
      horizontalServo.write(max(currentH - servoStep, horizontalLeftLimit));
    }
  }
}

// ------------------------------
// Main Loop
// ------------------------------
void loop() {
  // Check for an IR code
  if (IrReceiver.decode()) {
    uint32_t code = IrReceiver.decodedIRData.decodedRawData;
    Serial.print("IR code received: 0x");
    Serial.println(code, HEX);
    
    // Check if the code is a repeat code (0xFFFFFFFF)
    if (code == 0xFFFFFFFF) {
      // Use the last command (if any) for a held-button effect
      code = lastCommand;
    } else {
      // Save the new command for repeat processing
      lastCommand = code;
    }
    
    // Process command
    if (code == 0xE916FF00) {  // Home all
      Serial.println("Homing servos");
      verticalServo.write(verticalHomePosition);
      horizontalServo.write(horizontalHomePosition);
      currentDirection = HOME;
    }
    else if (code == 0xE718FF00) {  // Up
      Serial.println("Moving UP");
      currentDirection = UP;
    }
    else if (code == 0xAD52FF00) {  // Down
      Serial.println("Moving DOWN");
      currentDirection = DOWN;
    }
    else if (code == 0xF708FF00) {  // Left
      Serial.println("Moving LEFT");
      currentDirection = LEFT;
    }
    else if (code == 0xA55AFF00) {  // Right
      Serial.println("Moving RIGHT");
      currentDirection = RIGHT;
    }
    else {
      Serial.println("Undefined IR command.");
      currentDirection = NONE;
    }
    
    IrReceiver.resume();
  }
  
  // Smoothly update servos based on currentDirection.
  // If no direction is active, do nothing.
  if (currentDirection != HOME && currentDirection != NONE) {
    updateServos();
  }
}