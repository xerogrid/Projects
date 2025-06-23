// All Credit for original code and code format goes to xyz.
// Thanks for the inspiration.

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

// Number of LEDs
#define pixelNum 11

// Servo limits
const int chinPanRangeMax = 270;    // Servo Max angle (270 degree servo)
const int chinPanRangeMin = 0;      // Servo min angle
const int eyePanRangeMax = 180;     // Servo Max angle (keeping original 180° range)
const int eyePanRangeMin = 0;       // Servo min angle
const int eyeTiltRangeMax = 110;    // Servo max angle (RECHECK value every time head is reassembled)
const int eyeTiltRangeMin = 0;      // Servo min angle (RECHECK value every time head is reassembled)
const int sensorPanRangeMax = 270;  // Servo Max angle (270 degree servo)
const int sensorPanRangeMin = 0;    // Servo min angle

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

// Set DFPlayer audio file numbers and what they are
// 001.mp3 - System Boot and Diagnostic audio
// 002.mp3 - Greeting audio after startup
// 003.mp3 - Missing Limbs Gag Audio
// 004.mp3 - Generic Robot Noises
// 005.mp3 -
// 006.mp3 -
// 007.mp3 -
// 008.mp3 -
// 009.mp3 -


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
//---------- PRODUCTION MODE -------------
//========================================
bool productionMode = true;  // Set to true for production behavior

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
    
    // Check file count
    delay(1000);
    int fileCount = myDFPlayer.readFileCounts();
    Serial.print(F("Audio files detected: "));
    Serial.println(fileCount);
  }
}

void playStartupAudio() {
  Serial.println(F("Playing startup audio..."));
  myDFPlayer.play(1);  // Play 001.mp3
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
      myDFPlayer.play(command);
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
  myDFPlayer.play(2); // Play 002.mp3
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
          productionStateDuration = random(120000, 300000); // 2-5 minutes
          Serial.println("Production Mode: Entering Full Active");
          lastFullActiveAudioTime = now; // Reset audio timer for new state
        } else {
          currentProductionState = PROD_IDLE_SCANNING;
          productionStateDuration = random(5000, 20000); // 5-20 seconds
          Serial.println("Production Mode: Entering Idle Scanning");
          lastIdleScanningAudioTime = now; // Reset audio timer for new state
        }
        break;
        
      case PROD_IDLE_SCANNING:
        // Always return to dormant after idle scanning
        currentProductionState = PROD_DORMANT;
        productionStateDuration = random(120000, 600000); // 2-10 minutes
        Serial.println("Production Mode: Entering Dormant");
        break;
        
      case PROD_FULL_ACTIVE:
        // Always return to dormant after full active
        currentProductionState = PROD_DORMANT;
        productionStateDuration = random(600000, 1800000); // 10-30 minutes
        Serial.println("Production Mode: Entering Dormant");
        break;
    }
  }
}

void handleSensorHoming() {
  unsigned long now = millis();
  
  // Check if 30 minutes have passed since last homing
  if (now - lastSensorHomeTime >= sensorHomeInterval) {
    Serial.println("Production Mode: Performing sensor pan homing");
    
    // Force sensor pan to home position
    sensorPanTarget = sensorPanStop;
    sensorPanPosition = sensorPanStop;
    sensorPanServo.write(sensorPanStop);
    
    // Reset the timer
    lastSensorHomeTime = now;
    
    // Brief delay to ensure servo reaches position
    delay(100);
    
    Serial.println("Sensor pan homed to center position");
  }
}

void handleProductionAudio() {
  unsigned long now = millis();
  
  switch (currentProductionState) {
    case PROD_IDLE_SCANNING:
      // Check for audio every 3 seconds during idle scanning
      if (now - lastIdleScanningAudioTime >= idleScanningAudioInterval) {
        if (random(0, 100) < 15) { // 15% chance every 3 seconds = roughly every 20 seconds on average
          Serial.println("Playing idle scanning audio (003.mp3)");
          myDFPlayer.play(3);
          lastIdleScanningAudioTime = now + 10000; // Prevent audio overlap for 10 seconds
        } else {
          lastIdleScanningAudioTime = now;
        }
      }
      break;
      
    case PROD_FULL_ACTIVE:
      // Check for audio every 8 seconds during full active
      if (now - lastFullActiveAudioTime >= fullActiveAudioInterval) {
        if (random(0, 100) < 20) { // 20% chance every 8 seconds = roughly every 40 seconds on average
          int audioChoice = random(4, 6); // Random between 4 and 5 (004.mp3 or 005.mp3)
          Serial.print("Playing full active audio (00");
          Serial.print(audioChoice);
          Serial.println(".mp3)");
          myDFPlayer.play(audioChoice);
          lastFullActiveAudioTime = now + 15000; // Prevent audio overlap for 15 seconds
        } else {
          lastFullActiveAudioTime = now;
        }
      }
      break;
      
    case PROD_DORMANT:
      // No automatic audio during dormant mode
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
  Serial.begin(115200);  // Changed to 115200 for compatibility with original audio code
  Serial.println("Starting IG-11");

  // Initialize servos
  chinPanServo.attach(chinPanPin);
  eyePanServo.attach(eyePanPin);
  eyeTiltServo.attach(eyeTiltPin);
  sensorPanServo.attach(sensorPanPin);

  // Initialize LEDs
  pixels.begin();
  pixels.show();
  pixels.setBrightness(200);

  // Run the startup sequence (includes audio initialization and startup sound)
  startupSequence();

  // Initialize production mode or normal mode
  if (productionMode) {
    Serial.println("Production Mode Enabled");
    currentProductionState = PROD_DORMANT;
    productionStateStartTime = millis();
    productionStateDuration = random(120000, 600000); // Start dormant for 2-10 minutes
    lastDormantSweepTime = millis(); // Initialize dormant sweep timer
    lastSensorHomeTime = millis(); // Initialize sensor homing timer
  } else {
    Serial.println("Normal Mode - All systems active");
  }
  
  Serial.println("Audio commands: 1-9 (play), s (stop), p (pause), v# (volume), +/- (vol up/down)");
}

//========================================
//---------------- LOOP ------------------
//========================================
void loop() {
  currentMicros = micros();
  
  // Always handle audio commands regardless of production mode
  handleSerialAudioCommands();
  
  if (productionMode) {
    // Handle production mode logic
    updateProductionMode();
    handleProductionAudio(); // Handle automatic audio for production modes
    handleSensorHoming(); // Handle periodic sensor homing every 30 minutes
    
    switch (currentProductionState) {
      case PROD_DORMANT:
        // Check for dormant LED sweep every 60 seconds
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