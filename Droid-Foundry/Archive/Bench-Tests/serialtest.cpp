#include <Arduino.h>

void setup() {
    Serial.begin(9600);
    while (!Serial) { }  // Wait for serial connection (not necessary for Nano but safe to include)
    Serial.println("Serial monitor is working!");
}

void loop() {
    Serial.println("Loop running...");
    delay(1000);
}