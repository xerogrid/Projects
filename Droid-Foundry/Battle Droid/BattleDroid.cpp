#include <Arduino.h>
#include <Servo.h>
#include <IRremote.h>

// Define servo objects
Servo verticalServo;
Servo horizontalServo;

// Define pin numbers
const int verticalPin = 9;
const int horizontalPin = 10;

// Define home positions
const int verticalHomePosition = 90;
const int horizontalHomePosition = 90;

// Define servo limits
const int horizontalLeftLimit = 20;
const int horizontalRightLimit = 160;
const int verticalUpLimit = 160;

// Define IR receiver pin
const int irReceiverPin = 11;

void setup() {
    // Attach servos to pins
    verticalServo.attach(verticalPin);
    horizontalServo.attach(horizontalPin);

    // Move servos to home position
    verticalServo.write(verticalHomePosition);
    horizontalServo.write(horizontalHomePosition);

    // Initialize serial communication
    Serial.begin(9600);

    // Enable the IR receiver (IRremote 4.x+)
    IrReceiver.begin(irReceiverPin, ENABLE_LED_FEEDBACK);
}

void loop() {
    if (IrReceiver.decode()) {
        Serial.print("Received IR code: 0x");
        Serial.println(IrReceiver.decodedIRData.decodedRawData, HEX);

        switch (IrReceiver.decodedIRData.decodedRawData) {
            case 0xFF30CF:  // Key 1
                Serial.println("1");
                // TODO: YOUR CONTROL
                break;

            case 0xFF18E7:  // Key 2
                Serial.println("2");
                // TODO: YOUR CONTROL
                break;

            case 0xFF7A85:  // Key 3
                Serial.println("3");
                // TODO: YOUR CONTROL
                break;

            case 0xFF10EF:  // Key 4
                Serial.println("4");
                // TODO: YOUR CONTROL
                break;

            case 0xFF38C7:  // Key 5
                Serial.println("5");
                // TODO: YOUR CONTROL
                break;

            case 0xFF5AA5:  // Key 6
                Serial.println("6");
                // TODO: YOUR CONTROL
                break;

            case 0xFF42BD:  // Key 7
                Serial.println("7");
                // TODO: YOUR CONTROL
                break;

            case 0xFF4AB5:  // Key 8
                Serial.println("8");
                // Horizontal servo sweep
                for (int i = horizontalHomePosition; i <= horizontalRightLimit; i++) {
                    horizontalServo.write(i);
                    delay(10);
                }
                break;

            case 0xFF52AD:  // Key 9
                Serial.println("9");
                // TODO: YOUR CONTROL
                break;

            case 0xFF6897:  // Key *
                Serial.println("*");
                // Home the servos
                verticalServo.write(verticalHomePosition);
                horizontalServo.write(horizontalHomePosition);
                break;

            case 0xFF9867:  // Key 0
                Serial.println("0");
                // Vertical servo sweep
                for (int i = verticalHomePosition; i <= verticalUpLimit; i++) {
                    verticalServo.write(i);
                    delay(10);
                }
                break;

            case 0xFFB04F:  // Key #
                Serial.println("#");
                // TODO: YOUR CONTROL
                break;

            case 0xFF629D:  // Key UP
                Serial.println("UP");
                // Move Vertical Servo up
                if (verticalServo.read() > verticalUpLimit) {
                    verticalServo.write(verticalServo.read() + 1);
                }
                break;

            case 0xFFA857:  // Key DOWN
                Serial.println("DOWN");
                // Move Vertical Servo down
                if (verticalServo.read() < verticalUpLimit) {
                    verticalServo.write(verticalServo.read() - 1);
                }
                break;

            case 0xFF22DD:  // Key LEFT
                Serial.println("LEFT");
                // Move Horitontal Servo to the left
                if (horizontalServo.read() > horizontalLeftLimit) {
                    horizontalServo.write(horizontalServo.read() - 1);
                }

                break;

            case 0xFFC23D:  // Key RIGHT
                Serial.println("RIGHT");
                // Move Horitontal Servo to the right
                if (horizontalServo.read() < horizontalRightLimit) {
                    horizontalServo.write(horizontalServo.read() + 1);
                }
                break;

            case 0xFF02FD:  // Key OK
                Serial.println("OK");
                // TODO: YOUR CONTROL
                break;

            default:
                Serial.print("WARNING: Undefined key received - 0x");
                Serial.println(IrReceiver.decodedIRData.decodedRawData, HEX);
                break;
        }
        IrReceiver.resume();  // Ready for next signal
    }
}