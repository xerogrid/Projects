#include <Arduino.h>
#include <IRremote.h>

const int irReceiverPin = 11;  // Your IR signal pin

void setup() {
    Serial.begin(9600);
    IrReceiver.begin(irReceiverPin, ENABLE_LED_FEEDBACK); // Correct initialization for IRremote v3+
}

void loop() {
    if (IrReceiver.decode()) {
        Serial.print("IR Received: 0x");
        Serial.println(IrReceiver.decodedIRData.decodedRawData, HEX);
        IrReceiver.resume();  // Ready for next signal
    }
}