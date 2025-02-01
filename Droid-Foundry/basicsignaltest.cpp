#include <Arduino.h>

const int irReceiverPin = 11;  // Your IR receiver signal pin

void setup() {
    Serial.begin(9600);
    pinMode(irReceiverPin, INPUT);
}

void loop() {
    int signal = digitalRead(irReceiverPin);  // Read HIGH or LOW
    Serial.println(signal);
    delay(100);
}