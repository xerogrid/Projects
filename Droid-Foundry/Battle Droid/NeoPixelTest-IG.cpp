#include <Adafruit_NeoPixel.h>

#define PIN 5 // Pin where the NeoPixels are connected
#define NUMPIXELS 16 // Number of NeoPixels

Adafruit_NeoPixel pixels(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

void setup() {
    pixels.begin(); // Initialize the NeoPixel library
    randomSeed(analogRead(0)); // Seed the random number generator

    // Test pattern: Blink each NeoPixel twice in green
    for (int i = 0; i < NUMPIXELS; i++) {
        for (int j = 0; j < 2; j++) {
            pixels.setPixelColor(i, pixels.Color(0, 255, 0)); // Green color
            pixels.show();
            delay(250);
            pixels.setPixelColor(i, pixels.Color(0, 0, 0)); // Off
            pixels.show();
            delay(250);
        }
    }
}

void loop() {
    for(int i=0; i<NUMPIXELS; i++) {
        int state = random(10); // Randomly choose a number between 0 and 9
        if (state < 2) {
            pixels.setPixelColor(i, pixels.Color(255, 0, 0)); // Red color
        } else if (state < 4) {
            for (int j = 0; j <= 255; j += 5) { // Fade in
                pixels.setPixelColor(i, pixels.Color(j, j, j));
                pixels.show();
                delay(10);
            }
            for (int j = 255; j >= 0; j -= 5) { // Fade out
                pixels.setPixelColor(i, pixels.Color(j, j, j));
                pixels.show();
                delay(10);
            }
        } else {
            pixels.setPixelColor(i, pixels.Color(0, 0, 0)); // Off
        }
        delay(50); // Small delay to ensure proper timing
    }
    pixels.show(); // Update the strip
    delay(500); // Wait for 500 milliseconds
}