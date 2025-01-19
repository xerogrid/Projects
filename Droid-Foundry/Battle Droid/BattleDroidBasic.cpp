#include <Arduino.h>
#include <Servo.h>

//Project is built on Arduino Nano ATMEGA328 (not the new version)

Servo servo1;
Servo servo2;

const int servo1Pin = 9;
const int servo2Pin = 10;
const int homePosition = 90;
void setup() {
    servo1.attach(servo1Pin);
    servo2.attach(servo2Pin);
}

void loop() {
    for (int pos = 0; pos <= 180; pos++) {
        servo1.write(pos);
        servo2.write(pos);
        delay(15); // Adjust the delay to control the speed of the sweep
    }
    for (int pos = 180; pos >= 0; pos--) {
        servo1.write(pos);
        servo2.write(pos);
        delay(15); // Adjust the delay to control the speed of the sweep
    }
}