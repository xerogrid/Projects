#include <Arduino.h>
#include <Servo.h>

// Project is built on Arduino Nano ATMEGA328 (not the new version)

// Servo setup
Servo HServo;  // Horizontal Servo
Servo VServo;  // Vertical Servo

const int HServoPin = 10;
const int VServoPin = 9;
const int homePosition = 90;

void setup() {
    // Servo initialization
    HServo.attach(HServoPin);
    VServo.attach(VServoPin);
    HServo.write(homePosition);  // Move to home position
    VServo.write(homePosition);  // Move to home position

    delay(5000);  // Wait for 5 seconds
}

void loop() {
    // Sweep left to right for 5 seconds
    unsigned long startTime = millis();
    while (millis() - startTime < 5000) {
        for (int pos = 0; pos <= 180; pos += 1) {  // Sweep from 0 to 180 degrees
            HServo.write(pos);
            delay(15);  // Adjust delay for speed of sweep
        }
        for (int pos = 180; pos >= 0; pos -= 1) {  // Sweep from 180 to 0 degrees
            HServo.write(pos);
            delay(15);  // Adjust delay for speed of sweep
        }
    }

    delay(5000);  // Wait for 5 seconds before next sweep
}