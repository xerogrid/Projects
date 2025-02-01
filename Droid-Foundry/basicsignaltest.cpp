#include <Arduino.h>

void setup() {
    Serial.begin(9600);
    while (!Serial) {}  // Wait for Serial to initialize (for Leonardo/Micro)
    Serial.println("Serial is working!");
}

void loop() {
    Serial.println("Loop running...");
    delay(1000);
}