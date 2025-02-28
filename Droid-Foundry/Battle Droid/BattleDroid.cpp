#include <Arduino.h>
#include <Servo.h>
#include <IRremote.h>

// ------------------------------
// Pin Definitions & Constants
// ------------------------------
const int pirPin           = 8;   // PIR motion sensor pin
const int verticalPin      = 9;   // Servo for vertical movement
const int horizontalPin    = 10;  // Servo for horizontal movement
const int irReceiverPin    = 11;  // IR receiver signal pin

// Servo home positions and limits
const int verticalHomePosition   = 90;
const int horizontalHomePosition = 90;
const int horizontalLeftLimit    = 20;
const int horizontalRightLimit   = 160;
const int verticalUpLimit        = 100;  // Maximum vertical angle 
const int verticalDownLimit      = 30;   // Minimum vertical angle

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

  // Attach motion sensor to pin 8
  pinMode(pirPin, INPUT);
  digitalWrite(pirPin, LOW);

  // Detach servos to prevent ticking
  detachServos();

  // Initialize Serial communication
  Serial.begin(9600);
  
  // Initialize the IR receiver (IRremote 4.x+)
  IrReceiver.begin(irReceiverPin, ENABLE_LED_FEEDBACK);
  
  Serial.println("BattleDroid template initialized.");
}

void detachServos() {
  verticalServo.detach();
  horizontalServo.detach();
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
    
    // 0 button to home vertical servo
    if (fullCode == 0xE619FF00) {
      Serial.println("Homing vertical servo");
      verticalServo.write(verticalHomePosition);
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
    // 2 button to nod vertically as if to indicate "yes" twice
    else if (fullCode == 0xB24DFF00) {
      Serial.println("Nodding vertically");
      verticalServo.write(verticalUpLimit);
      delay(500);
      verticalServo.write(verticalHomePosition);
      delay(500);
      verticalServo.write(verticalUpLimit);
      delay(500);
      verticalServo.write(verticalHomePosition);
    }
    // 5 button to shake horizontally as if to indicate "no" twice
    else if (fullCode == 0xBF40FF00) {
      Serial.println("Shaking horizontally");
      horizontalServo.write(horizontalLeftLimit);
      delay(500);
      horizontalServo.write(horizontalHomePosition);
      delay(500);
      horizontalServo.write(horizontalLeftLimit);
      delay(500);
      horizontalServo.write(horizontalHomePosition);
    }
    // 8 button to home horizontal servo
    else if (fullCode == 0xEA15FF00) {
      Serial.println("Homing horizontal servo");
      horizontalServo.write(horizontalHomePosition);
    }
    // 6 button to switch to motion sensor mode
    else if (fullCode == 0xBC43FF00) {
      Serial.println("Switching to motion sensor mode");
      unsigned long startTime = millis();
      unsigned long interval = random(3000, 10001); // Random interval between 3 and 10 seconds

      while (millis() - startTime < interval) {
      if (digitalRead(pirPin) == HIGH) {
        int verticalPosition = random(verticalDownLimit, verticalUpLimit + 1);
        int horizontalPosition = random(horizontalLeftLimit, horizontalRightLimit + 1);
        Serial.print("Motion detected. Random positions: Vertical=");
        Serial.print(verticalPosition);
        Serial.print(", Horizontal=");
        Serial.println(horizontalPosition);
        verticalServo.write(verticalPosition);
        horizontalServo.write(horizontalPosition);
        delay(500); // Small delay to allow servos to move
      }
      }

      // Return to home position after the interval
      Serial.println("Returning to home position");
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
    // * Button to home servos
    else if (fullCode == 0xE916FF00) {
      Serial.println("Homing servos");
      verticalServo.write(verticalHomePosition);
      horizontalServo.write(horizontalHomePosition);
    }
    else {
      Serial.println("Undefined IR command.");
    }
    
    // Resume IR receiver to get ready for the next signal
    IrReceiver.resume();
  }
}

/*
  IR Remote Control Button Mapping:
  ---------------------------------
  Button Code       | Action
  ------------------|---------------------------------
  0xE619FF00 (0)    | Home vertical servo
  0xBA45FF00 (1)    | Randomly sweep both servos
  0xB946FF00 (2)    | Nod vertically (yes) twice
  0xBF40FF00 (5)    | Shake horizontally (no) twice
  0xEA15FF00 (8)    | Home horizontal servo
  0xEA15FF00 (6)    | Switch to motion sensor mode
  0xE718FF00 (Up)   | Move vertical servo up
  0xAD52FF00 (Down) | Move vertical servo down
  0xF708FF00 (Left) | Move horizontal servo left
  0xA55AFF00 (Right)| Move horizontal servo right
  0xE916FF00 (*)    | Home both servos
*/