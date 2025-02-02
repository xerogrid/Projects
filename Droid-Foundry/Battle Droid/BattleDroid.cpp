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
const int verticalUpLimit        = 160;  // Adjust as needed

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
  
  // Optional: Print a startup message for debugging
  Serial.println("BattleDroid template initialized.");
}

// ------------------------------
// Main Loop
// ------------------------------
void loop() {
  // Check if an IR code has been received
  if (IrReceiver.decode()) {
    
    // * Button to home servos
    if (IrReceiver.decodedIRData.command == 0x00FF30CF) {
      verticalServo.write(verticalHomePosition);
      horizontalServo.write(horizontalHomePosition);
    }
    // Up Button to move up
    else if (IrReceiver.decodedIRData.command == 0x00FF18E7) {
      int newPosition = verticalServo.read() - 10;
      if (newPosition > verticalUpLimit) {
        verticalServo.write(newPosition);
      }
    }
    // Down Button to move down
    else if (IrReceiver.decodedIRData.command == 0x00FF4AB5) {
      int newPosition = verticalServo.read() + 10;
      if (newPosition < verticalHomePosition) {
        verticalServo.write(newPosition);
      }
    }
    // Left Button to move left
    else if (IrReceiver.decodedIRData.command == 0x00FF10EF) {
      int newPosition = horizontalServo.read() - 10;
      if (newPosition > horizontalLeftLimit) {
        horizontalServo.write(newPosition);
      }
    }
    // Right Button to move right
    else if (IrReceiver.decodedIRData.command == 0x00FF5AA5) {
      int newPosition = horizontalServo.read() + 10;
      if (newPosition < horizontalRightLimit) {
        horizontalServo.write(newPosition);
      }
    }
    // 0 button to home vertical servo
    else if (IrReceiver.decodedIRData.command == 0x00FF38C7) {
      verticalServo.write(verticalHomePosition);
    }
    // 8 button to home horizontal servo
    else if (IrReceiver.decodedIRData.command == 0x00FF42BD) {
      horizontalServo.write(horizontalHomePosition);
    }
    // 1 button to randomly sweep both servos
    else if (IrReceiver.decodedIRData.command == 0x00FF52AD) {
      int verticalPosition = random(verticalHomePosition, verticalUpLimit);
      int horizontalPosition = random(horizontalLeftLimit, horizontalRightLimit);
      verticalServo.write(verticalPosition);
      horizontalServo.write(horizontalPosition);
    }

    // Resume IR receiver to get ready for the next signal
    IrReceiver.resume();
  }

  // TODO: Optionally add additional code here
}