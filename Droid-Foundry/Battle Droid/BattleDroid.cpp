#include <Arduino.h>
#include <Servo.h>
#include <IRremote.h>

// ------------------------------
// Pin Definitions & Constants
// ------------------------------
const int verticalPin      = 9;   // Servo for vertical movement
const int horizontalPin    = 10;  // Servo for horizontal movement
const int irReceiverPin    = 11;  // IR receiver signal pin

// Servo home positions and limits
const int verticalHomePosition   = 90;
const int horizontalHomePosition = 90;
const int horizontalLeftLimit    = 20;
const int horizontalRightLimit   = 160;
const int verticalUpLimit        = 160;  // Maximum vertical angle
const int verticalDownLimit      = 20;   // Minimum vertical angle

// ------------------------------
// Object Declarations
// ------------------------------
Servo verticalServo;
Servo horizontalServo;

// ------------------------------
// Setup
// ------------------------------
void setup() {
  // Attach servos to their respective pins
  verticalServo.attach(verticalPin);
  horizontalServo.attach(horizontalPin);

  // Move servos to their home positions
  verticalServo.write(verticalHomePosition);
  horizontalServo.write(horizontalHomePosition);

  // Initialize Serial communication
  Serial.begin(9600);
  
  // Initialize the IR receiver (IRremote 4.x+)
  IrReceiver.begin(irReceiverPin, ENABLE_LED_FEEDBACK);
  
  Serial.println("BattleDroid template initialized.");
}

// ------------------------------
// Main Loop
// ------------------------------
void loop() {
  // Check if an IR code has been received
  if (IrReceiver.decode()) {
    uint32_t fullCode = IrReceiver.decodedIRData.decodedRawData;
    Serial.print("IR code received: 0x");
    Serial.println(fullCode, HEX);
    
    // * Button to home servos
    if (fullCode == 0xE916FF00) {
      Serial.println("Homing servos");
      verticalServo.write(verticalHomePosition);
      horizontalServo.write(horizontalHomePosition);
    }
    // Up Button to move up (increase vertical angle)
    else if (fullCode == 0xE718FF00) {
      int newPosition = verticalServo.read() + 10;
      if (newPosition <= verticalUpLimit) {
        Serial.print("Moving vertical servo up to ");
        Serial.println(newPosition);
        verticalServo.write(newPosition);
      } else {
        Serial.println("Vertical up limit reached.");
      }
    }
    // Down Button to move down (decrease vertical angle)
    else if (fullCode == 0xAD52FF00) {
      int newPosition = verticalServo.read() - 10;
      if (newPosition >= verticalDownLimit) {
        Serial.print("Moving vertical servo down to ");
        Serial.println(newPosition);
        verticalServo.write(newPosition);
      } else {
        Serial.println("Vertical down limit reached.");
      }
    }
    // Left Button to move left (decrease horizontal angle)
    else if (fullCode == 0xF708FF00) {
      int newPosition = horizontalServo.read() - 10;
      if (newPosition >= horizontalLeftLimit) {
        Serial.print("Moving horizontal servo left to ");
        Serial.println(newPosition);
        horizontalServo.write(newPosition);
      } else {
        Serial.println("Horizontal left limit reached.");
      }
    }
    // Right Button to move right (increase horizontal angle)
    else if (fullCode == 0xA55AFF00) {
      int newPosition = horizontalServo.read() + 10;
      if (newPosition <= horizontalRightLimit) {
        Serial.print("Moving horizontal servo right to ");
        Serial.println(newPosition);
        horizontalServo.write(newPosition);
      } else {
        Serial.println("Horizontal right limit reached.");
      }
    }
    // 0 button to home vertical servo
    else if (fullCode == 0xE619FF00) {
      Serial.println("Homing vertical servo");
      verticalServo.write(verticalHomePosition);
    }
    // 8 button to home horizontal servo
    else if (fullCode == 0xEA15FF00) {
      Serial.println("Homing horizontal servo");
      horizontalServo.write(horizontalHomePosition);
    }
    // 1 button to randomly sweep both servos
    else if (fullCode == 0xBA45FF00) {
      int verticalPosition = random(verticalDownLimit, verticalUpLimit + 1);
      int horizontalPosition = random(horizontalLeftLimit, horizontalRightLimit + 1);
      Serial.print("Random positions: Vertical=");
      Serial.print(verticalPosition);
      Serial.print(", Horizontal=");
      Serial.println(horizontalPosition);
      verticalServo.write(verticalPosition);
      horizontalServo.write(horizontalPosition);
    }
    else {
      Serial.println("Undefined IR command.");
    }
    
    // Resume IR receiver to get ready for the next signal
    IrReceiver.resume();
  }
}