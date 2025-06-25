// All Credit for original code and code format goes to DroidDivision User.
// Thanks for the inspiration.
// All code is running on Arduino Nano ATMega328P

//////////////////////////////////////////
//                                      //
//      IG-SERIES ASSASSIN DROID        //
//                                      //
//            MANUFACTURED BY           //
//           HOLOWAN LABORATORIES       //
//                                      //
//                 UNIT IG-12           //
//                                      //
//////////////////////////////////////////

//========================================
// ---------------LIBRARIES---------------
//========================================
#include <Servo.h>                // Get Servo library
#include <Adafruit_NeoPixel.h>    // Get NeoPixel library
#include "SoftwareSerial.h"       // DFPlayer library
#include "DFRobotDFPlayerMini.h"  // DFPlayer library

//========================================
// ---------------CONSTANTS---------------
//========================================
// Assign arduino pins
const int chinPanPin = 2;         // Chin pan servo is on this pin
const int eyePanPin = 3;          // Mid drive plate servo is on this pin
const int eyeTiltPin = 4;         // Eye tilt servo is on this pin
const int sensorPanPin = 5;       // Top Servo is on this pin
#define pixelPin 6                // Pixels are on this pin
// DFPlayer pins
const int dfPlayerRXPin = 10;     // DFPlayer RX pin
const int dfPlayerTXPin = 11;     // DFPlayer TX pin

// Hardware control pins
// WIRING: Connect one wire from each switch to the analog pin, 
//         and the other wire to GND. No external resistors needed.
//         Internal pull-up resistors are used - switches read LOW when pressed.
const int whiteButtonPin = A1;    // White momentary button - "Force Restart"
const int redButtonPin = A2;      // Red momentary button - "Stop All"
const int muteTogglePin = A3;     // Toggle switch - Mute/Unmute audio
const int homeTogglePin = A4;     // Toggle switch - Home servos/Resume
const int productionTogglePin = A5; // Toggle switch - Production Mode on/off

// Number of LEDs
#define pixelNum 11

// Servo limits
const int chinPanRangeMax = 270;    // Servo Max angle (270 degree servo)
const int chinPanRangeMin = 0;      // Servo min angle
const int eyePanRangeMax = 180;     // Servo Max angle (keeping original 180° range)
const int eyePanRangeMin = 0;       // Servo min angle
const int eyeTiltRangeMax = 90;     // Servo max angle (RECHECK value every time head is reassembled)
const int eyeTiltRangeMin = 0;      // Servo min angle (RECHECK value every time head is reassembled)
const int sensorPanRangeMax = 270;  // Servo Max angle (270 degree servo)
const int sensorPanRangeMin = 0;    // Servo min angle

// Conservative startup ranges (to prevent servo damage during testing)
const int eyeTiltStartupMax = 40;   // Safe startup range (10° above neutral)
const int eyeTiltStartupMin = 20;   // Safe startup range (10° below neutral)

// Neutral position (determined experimentally, recommend checking prior to or during assembly)
const int chinPanStop = 135;       // Middle position for 270 degree servo
const int eyePanStop = 90;         // Middle of Pan
const int eyeTiltStop = 60;        // Eyes are level (RECHECK value every time head is reassembled)
const int sensorPanStop = 135;     // Middle position for 270 degree servo

// Set servo step limits
const int chinPanStepMax = 3;       // Degrees/step (larger steps for more dramatic movement)
const int chinPanStepMin = 1;       // Degrees/step
const int eyePanStepMax = 2;        // Degrees/step 
const int eyePanStepMin = 1;        // Degrees/step
const int eyeTiltStepMax = 2;       // Degrees/step
const int eyeTiltStepMin = 1;       // Degrees/step 
const int sensorPanStepMax = 3;     // Degrees/step (larger steps for more dramatic movement)
const int sensorPanStepMin = 1;     // Degrees/step

// Set servo moving time limits (microseconds)
unsigned long  chinPanIntervalMax =    20000;     // Max duration between steps
unsigned long  chinPanIntervalMin =    8000;      // MIN duration between steps
unsigned long  eyePanIntervalMax =    20000;     // Max duration between steps
unsigned long  eyePanIntervalMin =    8000;      // MIN duration between steps
unsigned long  eyeTiltIntervalMax =   10000;     // Max duration between steps
unsigned long  eyeTiltIntervalMin =   1000;      // MIN duration between steps
unsigned long  sensorPanIntervalMax =  20000;     // Max duration between steps
unsigned long  sensorPanIntervalMin =  8000;      // MIN duration between steps

// LED update intervals in microseconds (set to 1-3 seconds)
unsigned long  pixelIntervalMax =   3000000;   
unsigned long  pixelIntervalMin =   1000000;   

// Set wait time limits (microseconds)
const unsigned long  chinPanWaitMax =   3000000;      
const unsigned long  chinPanWaitMin =   2000000;         
const unsigned long  eyePanWaitMax =    4000000;       
const unsigned long  eyePanWaitMin =    2000000;
const unsigned long  eyeTiltWaitMax =   5000000;       
const unsigned long  eyeTiltWaitMin =   2000000;
const unsigned long  sensorPanWaitMax = 3000000;       
const unsigned long  sensorPanWaitMin = 2000000;       

// Set DFPlayer audio file numbers
// 001.mp3 - Boot, 002.mp3 - Greeting, 003.mp3 - Gag, 004.mp3 - Noises

//========================================
//--------------VARIABLES-----------------
//========================================
unsigned long currentMicros = 0;                        
unsigned long previousChinPanMicros =   currentMicros;  
unsigned long previousEyePanMicros =    currentMicros;  
unsigned long previousEyeTiltMicros =   currentMicros; 
unsigned long previousSensorPanMicros = currentMicros; 
unsigned long previousPixelMicros =     currentMicros;

int  chinPanPosition =  chinPanStop;   // current angle of the servo (degrees)
int  eyePanPosition =   eyePanStop;    // current angle of the servo (degrees)
int  eyeTiltPosition =  eyeTiltStop;   // current angle of the servo (degrees)
int  sensorPanPosition = sensorPanStop; // current angle of the servo (degrees)

int            chinPanTarget = chinPanStop;
int            chinPanStep = chinPanStepMin;
unsigned long  chinPanContact = previousChinPanMicros;
int            eyePanTarget = eyePanStop;             
int            eyePanStep = eyePanStepMin;            
unsigned long  eyePanContact = previousEyePanMicros;  
int            eyeTiltTarget = eyeTiltStop;           
int            eyeTiltStep = eyeTiltStepMin;          
unsigned long  eyeTiltContact = previousEyeTiltMicros;
int            sensorPanTarget = sensorPanStop;
int            sensorPanStep = sensorPanStepMin;
unsigned long  sensorPanContact = previousSensorPanMicros; 

unsigned long  chinPanWait = chinPanWaitMin; 
unsigned long  eyePanWait = eyePanWaitMin;
unsigned long  eyeTiltWait = eyeTiltWaitMin;
unsigned long  sensorPanWait = sensorPanWaitMin;

unsigned long  chinPanInterval = chinPanIntervalMin;      
unsigned long  eyePanInterval = eyePanIntervalMin;        
unsigned long  eyeTiltInterval = eyeTiltIntervalMin;      
unsigned long  sensorPanInterval = sensorPanIntervalMin;  
unsigned long  pixelInterval = pixelIntervalMin;

// DFPlayer variables
String line;
char command;
int pause = 0;
int repeat = 0;

// create objects
Servo chinPanServo;  
Servo eyePanServo;
Servo eyeTiltServo;
Servo sensorPanServo;
Adafruit_NeoPixel pixels(pixelNum, pixelPin, NEO_GRB + NEO_KHZ800);
SoftwareSerial mySoftwareSerial(dfPlayerRXPin, dfPlayerTXPin);  // RX, TX
DFRobotDFPlayerMini myDFPlayer;

//========================================
//---------- HARDWARE CONTROLS -----------
//========================================
// Switch states and debouncing
bool whiteButtonPressed = false;
bool redButtonPressed = false;
bool muteToggleState = false;
bool homeToggleState = false;
bool productionToggleState = false;

bool lastWhiteButtonState = false;
bool lastRedButtonState = false;
bool lastMuteToggleState = false;
bool lastHomeToggleState = false;
bool lastProductionToggleState = false;

unsigned long lastWhiteButtonTime = 0;
unsigned long lastRedButtonTime = 0;
unsigned long lastMuteToggleTime = 0;
unsigned long lastHomeToggleTime = 0;
unsigned long lastProductionToggleTime = 0;

const unsigned long debounceDelay = 50; // 50ms debounce

// Control state variables
bool stopAllActive = false;
bool audioMuted = false;
bool servosHomed = false;

//========================================
//---------- PRODUCTION MODE -------------
//========================================
bool productionMode = true;  // Set by switch, true for production behavior

// Production mode states
enum ProductionState {
  PROD_IDLE_SCANNING,  // LEDs + SensorPan only
  PROD_FULL_ACTIVE,    // All servos + LEDs
  PROD_DORMANT         // Complete silence
};

ProductionState currentProductionState = PROD_DORMANT;
unsigned long productionStateStartTime = 0;
unsigned long productionStateDuration = 0;

// Dormant LED sweep variables
unsigned long lastDormantSweepTime = 0;
const unsigned long dormantSweepInterval = 10000; // 10 seconds

// Audio timing variables
unsigned long lastIdleScanningAudioTime = 0;
unsigned long lastFullActiveAudioTime = 0;
const unsigned long idleScanningAudioInterval = 3000; // Check every 3 seconds
const unsigned long fullActiveAudioInterval = 8000; // Check every 8 seconds

// Servo homing variables
unsigned long lastSensorHomeTime = 0;
const unsigned long sensorHomeInterval = 1800000; // 30 minutes in milliseconds

//========================================
//---------- HARDWARE FUNCTIONS ----------
//========================================
void readHardwareControls() {
  unsigned long now = millis();
  
  // Read digital values - switches read LOW when pressed/closed
  bool currentWhiteButton = !digitalRead(whiteButtonPin);
  bool currentRedButton = !digitalRead(redButtonPin);
  bool currentMuteToggle = !digitalRead(muteTogglePin);
  bool currentHomeToggle = !digitalRead(homeTogglePin);
  bool currentProductionToggle = !digitalRead(productionTogglePin);
  
  // Debounce white button (momentary)
  if (currentWhiteButton != lastWhiteButtonState) {
    lastWhiteButtonTime = now;
  }
  if ((now - lastWhiteButtonTime) > debounceDelay) {
    if (currentWhiteButton && !whiteButtonPressed) {
      whiteButtonPressed = true;
      handleWhiteButtonPress();
    } else if (!currentWhiteButton) {
      whiteButtonPressed = false;
    }
  }
  lastWhiteButtonState = currentWhiteButton;
  
  // Debounce red button (momentary)
  if (currentRedButton != lastRedButtonState) {
    lastRedButtonTime = now;
  }
  if ((now - lastRedButtonTime) > debounceDelay) {
    if (currentRedButton && !redButtonPressed) {
      redButtonPressed = true;
      handleRedButtonPress();
    } else if (!currentRedButton) {
      redButtonPressed = false;
    }
  }
  lastRedButtonState = currentRedButton;
  
  // Debounce mute toggle
  if (currentMuteToggle != lastMuteToggleState) {
    lastMuteToggleTime = now;
  }
  if ((now - lastMuteToggleTime) > debounceDelay) {
    if (currentMuteToggle != muteToggleState) {
      muteToggleState = currentMuteToggle;
      handleMuteToggle();
    }
  }
  lastMuteToggleState = currentMuteToggle;
  
  // Debounce home toggle
  if (currentHomeToggle != lastHomeToggleState) {
    lastHomeToggleTime = now;
  }
  if ((now - lastHomeToggleTime) > debounceDelay) {
    if (currentHomeToggle != homeToggleState) {
      homeToggleState = currentHomeToggle;
      handleHomeToggle();
    }
  }
  lastHomeToggleState = currentHomeToggle;
  
  // Debounce production toggle
  if (currentProductionToggle != lastProductionToggleState) {
    lastProductionToggleTime = now;
  }
  if ((now - lastProductionToggleTime) > debounceDelay) {
    if (currentProductionToggle != productionToggleState) {
      productionToggleState = currentProductionToggle;
      handleProductionToggle();
    }
  }
  lastProductionToggleState = currentProductionToggle;
}

void handleWhiteButtonPress() {
  Serial.println(F("WHITE: Force Restart"));
  
  if (productionMode) {
    // Force immediate production mode state change
    Serial.println(F("Forcing production state change"));
    
    // Randomly choose next state (avoid staying in dormant)
    if (random(0, 100) < 70) {
      currentProductionState = PROD_FULL_ACTIVE;
      productionStateDuration = random(60000, 180000); // 1-3 minutes
      Serial.println(F("Restart: Full Active"));
      
      // Play restart audio
      // THIS IS THE BUG WHEN PUSHING WHITE BUTTON
      if (!audioMuted) {
        playAudioFile(2); // Greeting audio
      }
    } else {
      currentProductionState = PROD_IDLE_SCANNING;
      productionStateDuration = random(10000, 30000); // 10-30 seconds
      Serial.println(F("Restart: Idle Scanning"));
      
      // Play scanning audio
      if (!audioMuted) {
        playAudioFile(3); // Scanning audio
      }
    }
    
    productionStateStartTime = millis();
    
  } else {
    // Demo mode (production false) - trigger reboot sequence
    Serial.println(F("Demo mode restart sequence"));
    
    // Brief reboot LED sequence
    for (int i = 0; i < 3; i++) {
      pixels.fill(pixels.Color(255, 255, 255)); // White
      pixels.show();
      delay(150);
      pixels.clear();
      pixels.show();
      delay(150);
    }
    
    // Play reboot audio
    if (!audioMuted) {
      playAudioFile(1); // Boot audio
    }
    
    // Briefly home all servos then resume
    homeAllServos();
    delay(500);
    
    // Set new random targets for immediate movement
    chinPanTarget = random(chinPanRangeMin, chinPanRangeMax);
    eyePanTarget = random(eyePanRangeMin, eyePanRangeMax);
    eyeTiltTarget = random(eyeTiltRangeMin, eyeTiltRangeMax);
    sensorPanTarget = random(sensorPanRangeMin, sensorPanRangeMax);
  }
  
  // Visual feedback - quick blue flash to indicate restart
  pixels.fill(pixels.Color(0, 100, 255)); // Blue
  pixels.show();
  delay(200);
  pixels.clear();
  pixels.show();
}

void handleRedButtonPress() {
  Serial.println(F("RED: STOP ALL"));
  stopAllActive = true;
  homeAllServos();
  if (!audioMuted) myDFPlayer.stop();
  blinkOrangeRapid();
}

void handleMuteToggle() {
  audioMuted = muteToggleState;
  if (audioMuted) {
    Serial.println(F("MUTE: ON"));
    myDFPlayer.stop();
  } else {
    Serial.println(F("MUTE: OFF"));
  }
}

void handleHomeToggle() {
  servosHomed = homeToggleState;
  if (servosHomed) {
    Serial.println(F("HOME: ON"));
    homeAllServos();
  } else {
    Serial.println(F("HOME: OFF"));
  }
}

void handleProductionToggle() {
  productionMode = !productionToggleState; // Inverted logic: switch ON = production OFF
  Serial.print(F("PRODUCTION MODE: "));
  Serial.println(productionMode ? F("ON - State Cycling") : F("OFF - Continuous Demo"));
  
  if (!productionMode) {
    Serial.println(F("Demo: All systems active"));
  } else {
    // Reset production mode state when switching back on
    currentProductionState = PROD_DORMANT;
    productionStateStartTime = millis();
    productionStateDuration = random(120000, 600000);
    Serial.println(F("Production: Reset to dormant"));
  }
}

void homeAllServos() {
  chinPanServo.write(chinPanStop);
  eyePanServo.write(eyePanStop);
  eyeTiltServo.write(eyeTiltStop);
  sensorPanServo.write(sensorPanStop);
  
  // Update position variables
  chinPanPosition = chinPanStop;
  eyePanPosition = eyePanStop;
  eyeTiltPosition = eyeTiltStop;
  sensorPanPosition = sensorPanStop;
  
  // Update target variables
  chinPanTarget = chinPanStop;
  eyePanTarget = eyePanStop;
  eyeTiltTarget = eyeTiltStop;
  sensorPanTarget = sensorPanStop;
}

void blinkOrangeRapid() {
  // Rapid orange blinking to indicate STOP ALL
  for (int i = 0; i < 10; i++) {
    pixels.fill(pixels.Color(255, 165, 0)); // Orange
    pixels.show();
    delay(100);
    pixels.clear();
    pixels.show();
    delay(100);
  }
}

void printHardwareStatus() {
  static unsigned long lastStatusPrint = 0;
  unsigned long now = millis();
  
  if (now - lastStatusPrint >= 30000) {
    Serial.println(F("=== Status ==="));
    Serial.print(F("Muted: ")); Serial.println(audioMuted ? F("YES") : F("NO"));
    Serial.print(F("Homed: ")); Serial.println(servosHomed ? F("YES") : F("NO"));
    Serial.print(F("Production: ")); Serial.println(productionMode ? F("ON") : F("OFF"));
    Serial.print(F("Stop: ")); Serial.println(stopAllActive ? F("ON") : F("OFF"));
    Serial.println(F("============="));
    lastStatusPrint = now;
  }
}

//========================================
//---------- LED FUNCTIONS ---------------
//========================================
void blinkTwiceRed() {
  pixels.fill(pixels.Color(250, 0, 0));
  pixels.show();
  delay(250);
  pixels.clear();
  pixels.show();
  delay(250);
  pixels.fill(pixels.Color(250, 0, 0));
  pixels.show();
  delay(250);
  pixels.clear();
  pixels.show();
  delay(250);
}

void blinkHalfRedNoFade() {
  pixels.setBrightness(100);  // set to half brightness
  pixels.fill(pixels.Color(250, 0, 0));
  pixels.show();
  delay(250);
  pixels.clear();
  pixels.show();
  delay(250);
  pixels.fill(pixels.Color(250, 0, 0));
  pixels.show();
  delay(250);
  pixels.clear();
  pixels.show();
  delay(250);
  pixels.setBrightness(200);  // restore normal brightness
}

void updateLEDs() {
  if (currentMicros >= pixelInterval + previousPixelMicros) {
    int chance = random(0, 100); // 0 to 99
    if (chance < 10) { // 10% chance for blink twice red at full brightness
      blinkTwiceRed();
    } else if (chance < 20) { // Next 10% chance for blink twice red at half brightness (no fade)
      blinkHalfRedNoFade();
    } else {
      uint32_t red = pixels.Color(250, 0, 0);
      pixels.fill(red, 0, pixelNum);
      // Additionally, randomly turn 1-3 LEDs white
      int whiteCount = random(1, 4); // 1 to 3
      for (int i = 0; i < whiteCount; i++) {
        int idx = random(0, pixelNum);
        pixels.setPixelColor(idx, pixels.Color(255, 255, 255));
      }
      pixels.show();
    }
    previousPixelMicros = currentMicros;
    pixelInterval = random(pixelIntervalMin, pixelIntervalMax);
  }
}

void dormantLEDSweep() {
  pixels.clear();
  
  // Sweep red LEDs around the ring
  for (int i = 0; i < pixelNum; i++) {
    pixels.clear();
    pixels.setPixelColor(i, pixels.Color(150, 0, 0)); // Dimmer red for dormant mode
    pixels.show();
    delay(100);
  }
  
  // Brief pause then clear
  delay(200);
  pixels.clear();
  pixels.show();
}

//========================================
//---------- DFPLAYER FUNCTIONS ----------
//========================================
void initializeDFPlayer() {
  Serial.println(F("Initializing DFPlayer Mini..."));
  mySoftwareSerial.begin(9600);
  
  if (!myDFPlayer.begin(mySoftwareSerial)) {
    Serial.println(F("DFPlayer initialization failed!"));
    Serial.println(F("Check connections and SD card"));
    // Continue without audio rather than halt
  } else {
    Serial.println(F("DFPlayer Mini initialized successfully"));
    myDFPlayer.setTimeOut(500);
    myDFPlayer.volume(30);  // Set to maximum volume
    myDFPlayer.EQ(0);       // Normal EQ
    
    // Only check file count if not muted (avoids delays during muted startup)
    if (!audioMuted) {
      delay(1000);
      int fileCount = myDFPlayer.readFileCounts();
      Serial.print(F("Audio files detected: "));
      Serial.println(fileCount);
    } else {
      Serial.println(F("Skipping file count check - audio muted"));
    }
  }
}

void playStartupAudio() {
  if (!audioMuted) {
    Serial.println(F("Playing startup audio..."));
    myDFPlayer.play(1);  // Play 001.mp3
  } else {
    Serial.println(F("Startup audio muted by switch"));
  }
}

void playAudioFile(int fileNumber) {
  if (!audioMuted) {
    myDFPlayer.play(fileNumber);
  }
}

void handleSerialAudioCommands() {
  // Check for DFPlayer feedback
  if (myDFPlayer.available()) {
    printDetail(myDFPlayer.readType(), myDFPlayer.read());
  }
  
  // Handle serial commands for audio control
  while (Serial.available() > 0) {
    command = Serial.peek();
    line = Serial.readStringUntil('\n');

    // Play from first 9 files
    if ((command >= '1') && (command <= '9')) {
      Serial.print(F("Playing audio file "));
      Serial.println(command);
      command = command - 48;
      playAudioFile(command);
    }

    // Stop audio
    if (command == 's') {
      myDFPlayer.stop();
      Serial.println(F("Audio stopped"));
    }

    // Pause/Continue audio
    if (command == 'p') {
      pause = !pause;
      if (pause == 0) {
        Serial.println(F("Audio continue..."));
        myDFPlayer.start();
      }
      if (pause == 1) {
        Serial.println(F("Audio paused"));
        myDFPlayer.pause();
      }
    }

    // Volume control
    if (command == 'v') {
      int myVolume = line.substring(1).toInt();
      if (myVolume >= 0 && myVolume <= 30) {
        myDFPlayer.volume(myVolume);
        Serial.print(F("Volume set to: "));
        Serial.println(myVolume);
      }
    }

    // Volume up/down
    if (command == '+') {
      myDFPlayer.volumeUp();
      Serial.println(F("Volume up"));
    }
    if (command == '-') {
      myDFPlayer.volumeDown();
      Serial.println(F("Volume down"));
    }

    // Previous/Next track
    if (command == '<') {
      myDFPlayer.previous();
      Serial.println(F("Previous track"));
    }
    if (command == '>') {
      myDFPlayer.next();
      Serial.println(F("Next track"));
    }
  }
}

void printDetail(uint8_t type, int value) {
  switch (type) {
    case TimeOut:
      Serial.println(F("Audio timeout"));
      break;
    case DFPlayerPlayFinished:
      Serial.print(F("Track "));
      Serial.print(value);
      Serial.println(F(" finished"));
      break;
    case DFPlayerError:
      Serial.print(F("Audio error: "));
      switch (value) {
        case Busy:
          Serial.println(F("Card not found"));
          break;
        case FileIndexOut:
          Serial.println(F("File not found"));
          break;
        case FileMismatch:
          Serial.println(F("Cannot find file"));
          break;
        default:
          Serial.println(F("Unknown error"));
          break;
      }
      break;
  }
}

//========================================
//------ STARTUP SEQUENCE ---------------
//========================================
void tripleFlashLED(int ledIndex) {
  // Red flash
  pixels.clear();
  pixels.setPixelColor(ledIndex, pixels.Color(255, 0, 0));
  pixels.show();
  delay(200);
  pixels.clear();
  pixels.show();
  delay(100);
  
  // White flash
  pixels.setPixelColor(ledIndex, pixels.Color(255, 255, 255));
  pixels.show();
  delay(200);
  pixels.clear();
  pixels.show();
  delay(100);
  
  // Green flash
  pixels.setPixelColor(ledIndex, pixels.Color(0, 255, 0));
  pixels.show();
  delay(200);
  pixels.clear();
  pixels.show();
  delay(100);
}

void startupLEDSequence() {
  for (int i = 0; i < pixelNum; i++) {
    tripleFlashLED(i);
  }
}

void sweepServoStartup(Servo &servo, int minAngle, int maxAngle, int homeAngle) {
  Serial.print("Moving to minimum: ");
  Serial.println(minAngle);
  servo.write(minAngle);
  delay(1000);
  
  Serial.print("Sweeping from ");
  Serial.print(minAngle);
  Serial.print(" to ");
  Serial.println(maxAngle);
  
  // Sweep to maximum
  if (maxAngle > minAngle) {
    for (int angle = minAngle; angle <= maxAngle; angle += 2) {
      servo.write(angle);
      delay(20);
    }
  } else {
    for (int angle = minAngle; angle >= maxAngle; angle -= 2) {
      servo.write(angle);
      delay(20);
    }
  }
  servo.write(maxAngle);
  delay(1000);
  
  Serial.print("Returning to home: ");
  Serial.println(homeAngle);
  
  // Return to home
  if (homeAngle > maxAngle) {
    for (int angle = maxAngle; angle <= homeAngle; angle += 2) {
      servo.write(angle);
      delay(20);
    }
  } else {
    for (int angle = maxAngle; angle >= homeAngle; angle -= 2) {
      servo.write(angle);
      delay(20);
    }
  }
  servo.write(homeAngle);
  delay(500);
  Serial.println("Servo test complete");
}

void blinkAllGreenTwice() {
  // First blink
  pixels.fill(pixels.Color(0, 255, 0));
  pixels.show();
  delay(300);
  pixels.clear();
  pixels.show();
  delay(300);
  
  // Second blink
  pixels.fill(pixels.Color(0, 255, 0));
  pixels.show();
  delay(300);
  pixels.clear();
  pixels.show();
  delay(300);
}

void startupSequence() {
  Serial.println("Starting IG-11 Startup Sequence");
  
  // Step 1: Initialize audio first
  initializeDFPlayer();
  
  // Step 2: Play startup audio
  playStartupAudio();
  
  // Step 3: Triple flash each LED sequentially
  Serial.println("LED Startup Sequence");
  startupLEDSequence();
  Serial.println("LED sequence complete");
  
  // Step 4: Chin servo full rotation (270 degree servo)
  Serial.println("Testing Chin Pan Servo (270 degree)");
  sweepServoStartup(chinPanServo, chinPanRangeMin, chinPanRangeMax, chinPanStop);
  
  // Step 5: Eye pan servo full rotation (180 degree servo)
  Serial.println("Testing Eye Pan Servo (180 degree)");
  sweepServoStartup(eyePanServo, eyePanRangeMin, eyePanRangeMax, eyePanStop);
  
  // Step 6: Eye tilt servo min/max
  Serial.println("Testing Eye Tilt Servo (SG90/FS90R)");
  sweepServoStartup(eyeTiltServo, eyeTiltRangeMin, eyeTiltRangeMax, eyeTiltStop);
  
  // Step 7: Sensor pan servo full rotation (270 degree servo)
  Serial.println("Testing Sensor Pan Servo (270 degree)");
  sweepServoStartup(sensorPanServo, sensorPanRangeMin, sensorPanRangeMax, sensorPanStop);
  
  // Step 8: Blink all LEDs green twice
  Serial.println("Startup Complete - Ready");
  blinkAllGreenTwice();
  
  // Step 9: Play post-startup audio
  delay(1000); // Brief pause after startup
  Serial.println("Playing post-startup audio...");
  playAudioFile(2); // Play 002.mp3
}

//========================================
//---------- PRODUCTION MODE LOGIC ------
//========================================
void updateProductionMode() {
  unsigned long now = millis();
  
  // Check if it's time to change states
  if (now - productionStateStartTime >= productionStateDuration) {
    productionStateStartTime = now;
    
    switch (currentProductionState) {
      case PROD_DORMANT:
        // Random chance for full active mode (25% chance) or idle scanning (75% chance)
        if (random(0, 100) < 25) {
          currentProductionState = PROD_FULL_ACTIVE;
          productionStateDuration = random(120000, 300000);
          Serial.println(F("Prod: Full Active"));
          lastFullActiveAudioTime = now;
        } else {
          currentProductionState = PROD_IDLE_SCANNING;
          productionStateDuration = random(5000, 20000);
          Serial.println(F("Prod: Idle Scan"));
          lastIdleScanningAudioTime = now;
        }
        break;
        
      case PROD_IDLE_SCANNING:
        currentProductionState = PROD_DORMANT;
        productionStateDuration = random(120000, 600000);
        Serial.println(F("Prod: Dormant"));
        break;
        
      case PROD_FULL_ACTIVE:
        currentProductionState = PROD_DORMANT;
        productionStateDuration = random(600000, 1800000);
        Serial.println(F("Prod: Dormant"));
        break;
    }
  }
}

void handleSensorHoming() {
  unsigned long now = millis();
  
  if (now - lastSensorHomeTime >= sensorHomeInterval) {
    Serial.println(F("Sensor home"));
    sensorPanTarget = sensorPanStop;
    sensorPanPosition = sensorPanStop;
    sensorPanServo.write(sensorPanStop);
    lastSensorHomeTime = now;
    delay(100);
  }
}

void handleProductionAudio() {
  unsigned long now = millis();
  
  switch (currentProductionState) {
    case PROD_IDLE_SCANNING:
      if (now - lastIdleScanningAudioTime >= idleScanningAudioInterval) {
        if (random(0, 100) < 15) {
          Serial.println(F("Audio: 003"));
          playAudioFile(3);
          lastIdleScanningAudioTime = now + 10000;
        } else {
          lastIdleScanningAudioTime = now;
        }
      }
      break;
      
    case PROD_FULL_ACTIVE:
      if (now - lastFullActiveAudioTime >= fullActiveAudioInterval) {
        if (random(0, 100) < 20) {
          int audioChoice = random(4, 6);
          Serial.print(F("Audio: 00"));
          Serial.println(audioChoice);
          playAudioFile(audioChoice);
          lastFullActiveAudioTime = now + 15000;
        } else {
          lastFullActiveAudioTime = now;
        }
      }
      break;
      
    case PROD_DORMANT:
      break;
  }
}

bool isServoMoving() {
  return (sensorPanPosition != sensorPanTarget);
}

//========================================
//---------------- SETUP -----------------
//========================================
void setup() {
  Serial.begin(115200);
  Serial.println(F("Starting IG-12"));

  // Initialize hardware control pins FIRST
  pinMode(whiteButtonPin, INPUT_PULLUP);
  pinMode(redButtonPin, INPUT_PULLUP);
  pinMode(muteTogglePin, INPUT_PULLUP);
  pinMode(homeTogglePin, INPUT_PULLUP);
  pinMode(productionTogglePin, INPUT_PULLUP);
  
  // Read mute switch state BEFORE any audio initialization
  audioMuted = !digitalRead(muteTogglePin);
  
  if (audioMuted) {
    Serial.println(F("STARTUP: Audio muted by switch"));
  } else {
    Serial.println(F("STARTUP: Audio enabled"));
  }

  // Initialize servos
  chinPanServo.attach(chinPanPin);
  eyePanServo.attach(eyePanPin);
  eyeTiltServo.attach(eyeTiltPin);
  sensorPanServo.attach(sensorPanPin);

  // Initialize LEDs
  pixels.begin();
  pixels.show();
  pixels.setBrightness(200);

  // Run startup sequence (now respects mute state)
  startupSequence();

  // Read production mode switch after startup
  productionMode = !digitalRead(productionTogglePin);
  
  // Initialize production mode
  if (productionMode) {
    Serial.println(F("Production Mode: ON"));
    currentProductionState = PROD_DORMANT;
    productionStateStartTime = millis();
    productionStateDuration = random(120000, 600000);
    lastDormantSweepTime = millis();
    lastSensorHomeTime = millis();
  } else {
    Serial.println(F("Production Mode: OFF - Continuous Demo"));
  }
  
  Serial.println(F("Commands: 1-9,s,p,v#,+/-"));
  Serial.println(F("White(A1):Restart Red(A2):Stop"));
  Serial.println(F("A3:Mute A4:Home A5:Production"));
  Serial.println(F("Wire switches to pin+GND"));
}

//========================================
//---------------- LOOP ------------------
//========================================
void loop() {
  currentMicros = micros();
  
  // Always read hardware controls first
  readHardwareControls();
  
  // Always handle audio commands regardless of other states
  handleSerialAudioCommands();
  
  // Print hardware status periodically
  printHardwareStatus();
  
  // If STOP ALL is active, do nothing else
  if (stopAllActive) {
    return;
  }
  
  // If servos are homed, keep them in position and do nothing else
  if (servosHomed) {
    homeAllServos();
    return;
  }
  
  if (productionMode) {
    // Handle production mode logic
    updateProductionMode();
    handleProductionAudio(); // Handle automatic audio for production modes
    handleSensorHoming(); // Handle periodic sensor homing every 30 minutes
    
    switch (currentProductionState) {
      case PROD_DORMANT:
        // Check for dormant LED sweep every 10 seconds
        if (millis() - lastDormantSweepTime >= dormantSweepInterval) {
          dormantLEDSweep();
          lastDormantSweepTime = millis();
        }
        return;
        
      case PROD_IDLE_SCANNING:
        // Only run LEDs when servo is not moving
        if (!isServoMoving()) {
          updateLEDs();
        }
        // Always run sensor pan
        moveSensorPan();
        break;
        
      case PROD_FULL_ACTIVE:
        // Run everything like normal mode
        updateLEDs();
        moveChinPan();
        moveEyePan();
        moveEyeTilt();
        moveSensorPan();
        break;
    }
  } else {
    // Normal mode - run everything
    updateLEDs();
    moveChinPan();
    moveEyePan();
    moveEyeTilt();
    moveSensorPan();
  }
}

//========================================
//------------- SERVO FUNCTIONS ----------
//========================================
void moveChinPan() {
  if (currentMicros >= chinPanInterval + previousChinPanMicros) {
    if (chinPanPosition < chinPanTarget) {
      chinPanPosition += chinPanStep;
      previousChinPanMicros = currentMicros;
      if (chinPanPosition >= chinPanTarget) {
        chinPanPosition = chinPanTarget;
        chinPanContact = currentMicros;
      }
      chinPanServo.write(chinPanPosition);
    }
    if (chinPanPosition > chinPanTarget) {
      chinPanPosition -= chinPanStep;
      previousChinPanMicros = currentMicros;
      if (chinPanPosition < 0) chinPanPosition = 0;
      if (chinPanPosition <= chinPanTarget) {
        chinPanPosition = chinPanTarget;
        chinPanContact = currentMicros;
      }
      chinPanServo.write(chinPanPosition);
    }
    if (chinPanPosition == chinPanTarget) {
      if (chinPanWait + chinPanContact <= currentMicros) {
        chinPanWait = random(chinPanWaitMin, chinPanWaitMax);
        chinPanTarget = random(chinPanRangeMin, chinPanRangeMax);
        chinPanStep = random(chinPanStepMin, chinPanStepMax);
        chinPanInterval = random(chinPanIntervalMin, chinPanIntervalMax);
      }
    }
  }
}

void moveEyePan() {
  if (currentMicros >= eyePanInterval + previousEyePanMicros) {
    if (eyePanPosition < eyePanTarget) {
      eyePanPosition += eyePanStep;
      previousEyePanMicros = currentMicros;
      if (eyePanPosition >= eyePanTarget) {
        eyePanPosition = eyePanTarget;
        eyePanContact = currentMicros;
      }
      eyePanServo.write(eyePanPosition);
    }
    if (eyePanPosition > eyePanTarget) {
      eyePanPosition -= eyePanStep;
      previousEyePanMicros = currentMicros;
      if (eyePanPosition < 0) eyePanPosition = 0;
      if (eyePanPosition <= eyePanTarget) {
        eyePanPosition = eyePanTarget;
        eyePanContact = currentMicros;
      }
      eyePanServo.write(eyePanPosition);
    }
    if (eyePanPosition == eyePanTarget) {
      if (eyePanWait + eyePanContact <= currentMicros) {
        eyePanWait = random(eyePanWaitMin, eyePanWaitMax);
        eyePanTarget = random(eyePanRangeMin, eyePanRangeMax);
        eyePanStep = random(eyePanStepMin, eyePanStepMax);
        eyePanInterval = random(eyePanIntervalMin, eyePanIntervalMax);
      }
    }
  }
}

void moveEyeTilt() {
  if (currentMicros >= eyeTiltInterval + previousEyeTiltMicros) {
    if (eyeTiltPosition < eyeTiltTarget) {
      eyeTiltPosition += eyeTiltStep;
      previousEyeTiltMicros = currentMicros;
      if (eyeTiltPosition >= eyeTiltTarget) {
        eyeTiltPosition = eyeTiltTarget;
        eyeTiltContact = currentMicros;
      }
      eyeTiltServo.write(eyeTiltPosition);
    }
    if (eyeTiltPosition > eyeTiltTarget) {
      eyeTiltPosition -= eyeTiltStep;
      previousEyeTiltMicros = currentMicros;
      if (eyeTiltPosition < 0) eyeTiltPosition = 0;
      if (eyeTiltPosition <= eyeTiltTarget) {
        eyeTiltPosition = eyeTiltTarget;
        eyeTiltContact = currentMicros;
      }
      eyeTiltServo.write(eyeTiltPosition);
    }
    if (eyeTiltPosition == eyeTiltTarget) {
      if (eyeTiltWait + eyeTiltContact <= currentMicros) {
        eyeTiltWait = random(eyeTiltWaitMin, eyeTiltWaitMax);
        eyeTiltTarget = random(eyeTiltRangeMin, eyeTiltRangeMax);
        eyeTiltStep = random(eyeTiltStepMin, eyeTiltStepMax);
        eyeTiltInterval = random(eyeTiltIntervalMin, eyeTiltIntervalMax);
      }
    }
  }
}

void moveSensorPan() {
  if (currentMicros >= sensorPanInterval + previousSensorPanMicros) {
    if (sensorPanPosition < sensorPanTarget) {
      sensorPanPosition += sensorPanStep;
      previousSensorPanMicros = currentMicros;
      if (sensorPanPosition >= sensorPanTarget) {
        sensorPanPosition = sensorPanTarget;
        sensorPanContact = currentMicros;
      }
      sensorPanServo.write(sensorPanPosition);
    }
    if (sensorPanPosition > sensorPanTarget) {
      sensorPanPosition -= sensorPanStep;
      previousSensorPanMicros = currentMicros;
      if (sensorPanPosition < 0) sensorPanPosition = 0;
      if (sensorPanPosition <= sensorPanTarget) {
        sensorPanPosition = sensorPanTarget;
        sensorPanContact = currentMicros;
      }
      sensorPanServo.write(sensorPanPosition);
    }
    if (sensorPanPosition == sensorPanTarget) {
      if (sensorPanWait + sensorPanContact <= currentMicros) {
        sensorPanWait = random(sensorPanWaitMin, sensorPanWaitMax);
        sensorPanTarget = random(sensorPanRangeMin, sensorPanRangeMax);
        sensorPanStep = random(sensorPanStepMin, sensorPanStepMax);
        sensorPanInterval = random(sensorPanIntervalMin, sensorPanIntervalMax);
      }
    }
  }
}