#include <Adafruit_NeoPixel.h>

#define PIN 5 

Adafruit_NeoPixel ring = Adafruit_NeoPixel(24, PIN, NEO_GRB + NEO_KHZ800); // Initialize the Neopixel ring with 24 LEDs connected to the specified pin

int motionPin = 2; 

void setup() {
  pinMode(motionPin, INPUT); // Set the motion sensor pin as input
  ring.begin(); // Start the Neopixel ring
  ring.setBrightness(50); // Set the brightness level (0-255)
  ring.show(); // Update the ring with the current LED colors
}

void loop() {
  if (digitalRead(motionPin) == HIGH) { // If motion is detected
    activateLEDs(); // Activate the Neopixel LEDs
  } else { // If motion is not detected
    disableLEDs(); // Disable the Neopixel LEDs
  }
}

void activateLEDs() {
  for (int i = 0; i < ring.numPixels(); i++) {
    ring.setPixelColor(i, ring.Color(0, 255, 0)); // Set the color of each LED to red (you can change the color as per your preference)
  }
  ring.show(); // Update the ring with the new LED colors
}

void disableLEDs() {
  for (int i = 0; i < ring.numPixels(); i++) {
    ring.setPixelColor(i, ring.Color(0, 0, 0)); // Set the color of each LED to off (black)
  }
  ring.show(); // Update the ring with the new LED colors
}
