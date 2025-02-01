#include <Arduino.h>
#include <Servo.h>
#include <IRremote.h>

// Project is built on Arduino Nano ATMEGA328 (not the new version)

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

// Create IR receiver object
IRrecv irrecv(irReceiverPin);
decode_results results;

void setup() {
    // Attach servos to pins
    verticalServo.attach(verticalPin);
    horizontalServo.attach(horizontalPin);

    // Move servos to home position
    verticalServo.write(verticalHomePosition);
    horizontalServo.write(horizontalHomePosition);

    // Initialize serial communication
    Serial.begin(9600);

    // Enable the IR receiver
    irrecv.enableIRIn();
}

void loop() {
    if (irrecv.decode(&results)) {
        switch (results.value) {
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
                // TODO: YOUR CONTROL
                break;

            case 0xFF52AD:  // Key 9
                Serial.println("9");
                // TODO: YOUR CONTROL
                int randomHorizontal = random(horizontalLeftLimit, horizontalRightLimit + 1);
                int randomVertical = random(0, verticalUpLimit + 1);
                horizontalServo.write(randomHorizontal);
                verticalServo.write(randomVertical);
                break;

            case 0xFF6897:  // Key *
                Serial.println("*");
                // TODO: YOUR CONTROL
                verticalServo.write(verticalHomePosition);
                horizontalServo.write(horizontalHomePosition);
                break;

            case 0xFF9867:  // Key 0
                Serial.println("0");
                // TODO: YOUR CONTROL
                verticalServo.detach();
                horizontalServo.detach();
                break;

            case 0xFFB04F:  // Key #
                Serial.println("#");
                // TODO: YOUR CONTROL
                break;

            case 0xFF629D:  // Key UP
                Serial.println("UP");
                // TODO: YOUR CONTROL
                int currentPosition = verticalServo.read();
                if (currentPosition > 0) {
                  verticalServo.write(currentPosition - 10);  // Move up by 10 degrees
                }
                break;

            case 0xFFA857:  // Key DOWN
                Serial.println("DOWN");
                // TODO: YOUR CONTROL
                int currentPosition = verticalServo.read();
                if (currentPosition < verticalUpLimit) {
                  verticalServo.write(currentPosition + 10);  // Move down by 10 degrees
                }
                break;

            case 0xFF22DD:  // Key LEFT
                Serial.println("LEFT");
                // TODO: YOUR CONTROL
                int currentPosition = horizontalServo.read();
                if (currentPosition > horizontalLeftLimit) {
                  horizontalServo.write(currentPosition - 10);  // Move left by 10 degrees
                }
                break;

            case 0xFFC23D:  // Key RIGHT
                Serial.println("RIGHT");
                // TODO: YOUR CONTROL
                int currentPosition = horizontalServo.read();
                if (currentPosition < horizontalRightLimit) {
                  horizontalServo.write(currentPosition + 10);  // Move right by 10 degrees
                }
                break;

            case 0xFF02FD:  // Key OK
                Serial.println("OK");
                // TODO: YOUR CONTROL
                break;

            default:
                Serial.println("WARNING: undefined key:");
                break;
        }
        irrecv.resume();  // Receive the next value
    }
}