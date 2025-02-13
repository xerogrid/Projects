#include <Arduino.h>
#include <Wire.h>
#include <PWMServo.h>
#include <IRremote.h>

// ----------------------------------------------------
// Servo Configuration (angles in degrees)
// ----------------------------------------------------
const int horizontalLeftLimit   = 20;
const int horizontalRightLimit  = 160;
const int verticalDownLimit     = 20;
const int verticalUpLimit       = 160;
const int horizontalHomePosition = 90;
const int verticalHomePosition   = 90;

// Create servo objects using PWMServo (channels will correspond to PCA9685 outputs)
PWMServo horizontalServo;
PWMServo verticalServo;

// Variables to hold the current servo angles
int currentHorizontalAngle = horizontalHomePosition;
int currentVerticalAngle   = verticalHomePosition;

// ----------------------------------------------------
// IR Receiver Configuration
// ----------------------------------------------------
const int irReceiverPin = 2;  // Digital pin where IR receiver is connected

// ----------------------------------------------------
// Setup
// ----------------------------------------------------
void setup() {
  Serial.begin(9600);
  while (!Serial) { }  // Wait for Serial (optional for Nano)

  // Initialize I2C for the PCA9685 board
  Wire.begin();

  // Attach servos to PCA9685 channels:
  // (Make sure your PCA9685 breakout board is powered properly.)
  horizontalServo.attach(0);  // Attach horizontal servo to channel 0
  verticalServo.attach(1);    // Attach vertical servo to channel 1

  // Move servos to their home positions
  horizontalServo.write(currentHorizontalAngle);
  verticalServo.write(currentVerticalAngle);

  // Initialize the IR receiver (IRremote 4.4.1)
  IrReceiver.begin(irReceiverPin, ENABLE_LED_FEEDBACK);

  Serial.println("Setup complete. Servos are at home position.");
}

// ----------------------------------------------------
// Main Loop
// ----------------------------------------------------
void loop() {
  // If an IR code has been received, process it.
  if (IrReceiver.decode()) {
    // Retrieve the decoded IR data as an unsigned long
    unsigned long code = IrReceiver.decodedIRData.decodedRawData;
    Serial.print("IR Code Received: 0x");
    Serial.println(code, HEX);

    // Use a switch-case to handle various remote buttons.
    // (Adjust the IR codes below to match your remote’s codes.)
    switch (code) {

      // Turn horizontal servo LEFT (decrease angle)
      case 0xFF22DD:
        Serial.println("LEFT");
        if (currentHorizontalAngle > horizontalLeftLimit) {
          currentHorizontalAngle -= 5;
          horizontalServo.write(currentHorizontalAngle);
        }
        break;

      // Turn horizontal servo RIGHT (increase angle)
      case 0xFFC23D:
        Serial.println("RIGHT");
        if (currentHorizontalAngle < horizontalRightLimit) {
          currentHorizontalAngle += 5;
          horizontalServo.write(currentHorizontalAngle);
        }
        break;

      // Move vertical servo UP (increase angle)
      case 0xFF629D:
        Serial.println("UP");
        if (currentVerticalAngle < verticalUpLimit) {
          currentVerticalAngle += 5;
          verticalServo.write(currentVerticalAngle);
        }
        break;

      // Move vertical servo DOWN (decrease angle)
      case 0xFFA857:
        Serial.println("DOWN");
        if (currentVerticalAngle > verticalDownLimit) {
          currentVerticalAngle -= 5;
          verticalServo.write(currentVerticalAngle);
        }
        break;

      // Reset both servos to their home positions
      case 0xFF6897:
        Serial.println("HOME");
        currentHorizontalAngle = horizontalHomePosition;
        currentVerticalAngle   = verticalHomePosition;
        horizontalServo.write(currentHorizontalAngle);
        verticalServo.write(currentVerticalAngle);
        break;

      // Handle any undefined IR codes
      default:
        Serial.print("Undefined IR code: 0x");
        Serial.println(code, HEX);
        break;
    }

    // Prepare to receive the next IR code
    IrReceiver.resume();
  }
}