#include <Arduino.h>
#include <Servo.h>

//Project is built on Arduino Nano ATMEGA328 (not the new version)
//Mount Servos
//Wiggle both servos, then home them
//Invesitgate motion trigger mechanisms in lieu of PIR sensors
//Determine if LEDs will be used
//Program a standby state which is either servo homing or lowered head


Servo HServo;
Servo VServo;

const int HServoPin = 9;
const int VServoPin = 10;
const int homePosition = 90;

void setup() {
    HServo.attach(HServoPin);
    VServo.attach(VServoPin);
}

void loop() {
    for (int pos = 0; pos <= 180; pos++) {
        HServo.write(pos);
        VServo.write(pos);
        delay(15); // Adjust the delay to control the speed of the sweep
    }
    for (int pos = 180; pos >= 0; pos--) {
        HServo.write(pos);
        VServo.write(pos);
        delay(15); // Adjust the delay to control the speed of the sweep
    }
}