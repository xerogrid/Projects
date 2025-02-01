#include <Arduino.h>
#include <Servo.h>

// Project is built on Arduino Nano ATMEGA328 (not the new version)

// Servo setup
Servo servo1;
Servo servo2;

const int servo1Pin = 9;
const int servo2Pin = 10;
const int homePosition = 90;

// PIR Sensor setup
const int pirSensorPin = 2;  // Pin for PIR sensor
bool motionDetected = false;

// LED setup
const int led1Pin = 3;  // Pin for LED 1
const int led2Pin = 4;  // Pin for LED 2

void setup() {
    // Servo initialization
    servo1.attach(servo1Pin);
    servo2.attach(servo2Pin);
    servo1.write(homePosition);  // Move to home position
    servo2.write(homePosition);  // Move to home position

    // PIR Sensor initialization
    pinMode(pirSensorPin, INPUT);

    // LED initialization
    pinMode(led1Pin, OUTPUT);
    pinMode(led2Pin, OUTPUT);

    // Start with LEDs off
    digitalWrite(led1Pin, LOW);
    digitalWrite(led2Pin, LOW);
}

void loop() {
    // Check PIR sensor for motion
    motionDetected = digitalRead(pirSensorPin);

    if (motionDetected) {
        // If motion is detected, perform servo movement and LED blinking
        for (int pos = 0; pos <= 180; pos += 10) {
            servo1.write(pos);
            servo2.write(180 - pos);  // Inverted movement for servo 2
            digitalWrite(led1Pin, HIGH);  // LED 1 on
            digitalWrite(led2Pin, LOW);   // LED 2 off
            delay(50);  // Delay for motion
        }

        for (int pos = 180; pos >= 0; pos -= 10) {
            servo1.write(pos);
            servo2.write(180 - pos);  // Inverted movement for servo 2
            digitalWrite(led1Pin, LOW);   // LED 1 off
            digitalWrite(led2Pin, HIGH);  // LED 2 on
            delay(50);  // Delay for motion
        }
    } else {
        // If no motion, return servos to home position and turn off LEDs
        servo1.write(homePosition);
        servo2.write(homePosition);
        digitalWrite(led1Pin, LOW);
        digitalWrite(led2Pin, LOW);
    }

    // Small delay to prevent excessive polling
    delay(100);
}