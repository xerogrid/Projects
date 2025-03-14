// Code to home servos for IG Droid before final assembly
// Controller is Arduino Nano ATMega328P (Old Bootloader)

#include <Arduino.h>
#include <Servo.h>
#include <Adafruit_NeoPixel.h>

// Attach Servos to pins and define

const int BottomServoPin = 2 ; // Chin pan servo is on this pin
    // This wire is white
    // This is a 270° servo
const int MidDrivePlatePin = 33 ; // Mid drive plate servo is on this pin
    // This wire is blue
    // This is a 270° servo
const int EyeServoPin = 4 ; // Eye tilt servo is on this pin
    // This wire is yellow
    // This is a 360° servo
const int TopServo = 5 ; // Top Servo is on this pin
    // This wire is green
    // This is a 270° servo
// const int NeoPixelPin = 6 ; // NeoPixel is on this pin
    // This wire is pink


// Run loop which is simply initialize servos and return them to home position

Servo bottomServo;
Servo midDrivePlateServo;
Servo eyeServo;
Servo topServo;

void setup() {
    bottomServo.attach(BottomServoPin);
    midDrivePlateServo.attach(MidDrivePlatePin);
    eyeServo.attach(EyeServoPin);
    topServo.attach(TopServo);

    // Move servos to home position
    bottomServo.write(135); // Adjust the angle as needed for home position (270° servo)
    midDrivePlateServo.write(135); // Adjust the angle as needed for home position (270° servo)
    eyeServo.write(180); // Adjust the angle as needed for home position (360° servo)
    topServo.write(135); // Adjust the angle as needed for home position (270° servo)
}

void loop() {
    // As a test, once the servos are home sweep them fully then return to home
    
    // Sweep servos to maximum position and then return to home
    bottomServo.write(0);
    delay(1000);
    bottomServo.write(270);
    delay(1000);

    midDrivePlateServo.write(0);
    delay(1000);
    midDrivePlateServo.write(270);
    delay(1000);

    eyeServo.write(0);
    delay(1000);
    eyeServo.write(350);
    delay(1000);

    topServo.write(0);
    delay(1000);
    topServo.write(270);
    delay(1000);
}