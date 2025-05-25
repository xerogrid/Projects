//========================================
//------ NEW LED CONNECTION LOGIC ------
//========================================//////////////////////////////////////////
//                                      //
//      IG-SERIES ASSASSIN DROID        //
//                                      //
//            MANUFACTURED BY           //
//           HOLOWAN LABORATORIES       //
//                                      //
//                 UNIT IG-11           //
//                                      //
//////////////////////////////////////////

//========================================
// ---------------LIBRARIES---------------
//========================================
#include <Servo.h>                // Get Servo library
#include <Adafruit_NeoPixel.h>    // Get NeoPixel library

//========================================
// ---------------CONSTANTS---------------
//========================================
// Assign arduino pins
const int chinPanPin = 2;         // Chin pan servo is on this pin
const int eyePanPin = 3;          // Mid drive plate servo is on this pin
const int eyeTiltPin = 4;         // Eye tilt servo is on this pin
const int sensorPanPin = 5;       // Top Servo is on this pin
#define pixelPin 6                // Pixels are on this pin

// Number of LEDs
#define pixelNum 11

// Servo limits - Updated for 270 degree servos
const int chinPanRangeMax = 270;    // Servo Max angle (270 degree servo)
const int chinPanRangeMin = 0;      // Servo min angle
const int eyePanRangeMax = 180;     // Servo Max angle (keeping original 180° range)
const int eyePanRangeMin = 0;       // Servo min angle
const int eyeTiltRangeMax = 110;    // Servo max angle (RECHECK value every time head is reassembled)
const int eyeTiltRangeMin = 0;      // Servo min angle (RECHECK value every time head is reassembled)
const int sensorPanRangeMax = 270;  // Servo Max angle (270 degree servo)
const int sensorPanRangeMin = 0;    // Servo min angle

// Neutral position (determined experimentally) - Updated for 270 degree servos
const int chinPanStop = 135;       // Middle position for 270 degree servo
const int eyePanStop = 90;         // Middle of Pan
const int eyeTiltStop = 60;        // Eyes are level (RECHECK value every time head is reassembled)
const int sensorPanStop = 135;     // Middle position for 270 degree servo

// Set servo speed limits
const int chinPanStepMax = 3;       // Degrees/step (larger steps for more dramatic movement)
const int chinPanStepMin = 1;       // Degrees/step
const int eyePanStepMax = 2;        // Degrees/step 
const int eyePanStepMin = 1;        // Degrees/step
const int eyeTiltStepMax = 2;       // Degrees/step
const int eyeTiltStepMin = 1;       // Degrees/step 
const int sensorPanStepMax = 3;     // Degrees/step (larger steps for more dramatic movement)
const int sensorPanStepMin = 1;     // Degrees/step

// Set servo moving time limits (microseconds)
unsigned long  chinPanIntervalMax =    20000;     // Max duration between steps (aligned with eye servos)
unsigned long  chinPanIntervalMin =    8000;      // MIN duration between steps (aligned with eye servos)
unsigned long  eyePanIntervalMax =    20000;     // Max duration between steps
unsigned long  eyePanIntervalMin =    8000;      // MIN duration between steps
unsigned long  eyeTiltIntervalMax =   10000;     // Max duration between steps
unsigned long  eyeTiltIntervalMin =   1000;      // MIN duration between steps
unsigned long  sensorPanIntervalMax =  20000;     // Max duration between steps (aligned with eye servos)
unsigned long  sensorPanIntervalMin =  8000;      // MIN duration between steps (aligned with eye servos)
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
const unsigned long  sensorPanWaitMax = 3000000;       // aligned with chinPan
const unsigned long  sensorPanWaitMin = 2000000;       // aligned with chinPan
const unsigned long  pixelWaitMax =     4000000;       
const unsigned long  pixelWaitMin =     100000;

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
unsigned long  eyeTiltContact = previousEyePanMicros;
int            sensorPanTarget = sensorPanStop;
int            sensorPanStep = sensorPanStepMin;
unsigned long  sensorPanContact = previousSensorPanMicros; 

unsigned long  chinPanWait = chinPanWaitMin; 
unsigned long  eyePanWait = eyePanWaitMin;
unsigned long  eyeTiltWait = eyeTiltWaitMin;
unsigned long  sensorPanWait = sensorPanWaitMin;
unsigned long  pixelWait = pixelWaitMin;

unsigned long  chinPanInterval = chinPanIntervalMin;      
unsigned long  eyePanInterval = eyePanIntervalMin;        
unsigned long  eyeTiltInterval = eyeTiltIntervalMin;      
unsigned long  sensorPanInterval = sensorPanIntervalMin;  
unsigned long  pixelInterval = pixelIntervalMin;

// create objects
Servo chinPanServo;  
Servo eyePanServo;
Servo eyeTiltServo;
Servo sensorPanServo;
Adafruit_NeoPixel pixels(pixelNum, pixelPin, NEO_GRB + NEO_KHZ800);

//------------------ NEW MODE SWITCHING ------------------
// We'll run the main loop (servos and LED pizzazz) only during an "active" period of 5-10 minutes.
// Otherwise, the system stays idle (i.e. does nothing) for a period of 15-30 minutes.
unsigned long modeSwitchTime = 0;    // time when the current mode started (using millis())
unsigned long modeDuration = 0;      // duration of current mode (in ms)
bool activeMode = false;             // false = idle; true = active

//------------------ SERVO TEST FLAGS ------------------
// Set to true to enable continuous full range sweep for testing
bool chinPanContinuousSweep = false;
bool eyePanContinuousSweep = false;
bool eyeTiltContinuousSweep = false;
bool sensorPanContinuousSweep = false;

// Sweep state tracking
int chinPanSweepPos = 0;
int chinPanSweepDirection = 1;
int eyePanSweepPos = 0;
int eyePanSweepDirection = 1;
int eyeTiltSweepPos = 0;
int eyeTiltSweepDirection = 1;
int sensorPanSweepPos = 0;
int sensorPanSweepDirection = 1;

unsigned long lastChinPanSweepTime = 0;
unsigned long lastEyePanSweepTime = 0;
unsigned long lastEyeTiltSweepTime = 0;
unsigned long lastSensorPanSweepTime = 0;

//========================================
//------ CONTINUOUS SWEEP FUNCTIONS -----
//========================================

void continuousSweepChinPan() {
  if (currentMicros - lastChinPanSweepTime >= 50000) { // 50ms between steps
    chinPanSweepPos += chinPanSweepDirection * 2;
    if (chinPanSweepPos >= chinPanRangeMax) {
      chinPanSweepPos = chinPanRangeMax;
      chinPanSweepDirection = -1;
    } else if (chinPanSweepPos <= chinPanRangeMin) {
      chinPanSweepPos = chinPanRangeMin;
      chinPanSweepDirection = 1;
    }
    chinPanServo.write(chinPanSweepPos);
    lastChinPanSweepTime = currentMicros;
  }
}

void continuousSweepEyePan() {
  if (currentMicros - lastEyePanSweepTime >= 50000) { // 50ms between steps
    eyePanSweepPos += eyePanSweepDirection * 2;
    if (eyePanSweepPos >= eyePanRangeMax) {
      eyePanSweepPos = eyePanRangeMax;
      eyePanSweepDirection = -1;
    } else if (eyePanSweepPos <= eyePanRangeMin) {
      eyePanSweepPos = eyePanRangeMin;
      eyePanSweepDirection = 1;
    }
    eyePanServo.write(eyePanSweepPos);
    lastEyePanSweepTime = currentMicros;
  }
}

void continuousSweepEyeTilt() {
  if (currentMicros - lastEyeTiltSweepTime >= 50000) { // 50ms between steps
    eyeTiltSweepPos += eyeTiltSweepDirection * 2;
    if (eyeTiltSweepPos >= eyeTiltRangeMax) {
      eyeTiltSweepPos = eyeTiltRangeMax;
      eyeTiltSweepDirection = -1;
    } else if (eyeTiltSweepPos <= eyeTiltRangeMin) {
      eyeTiltSweepPos = eyeTiltRangeMin;
      eyeTiltSweepDirection = 1;
    }
    eyeTiltServo.write(eyeTiltSweepPos);
    lastEyeTiltSweepTime = currentMicros;
  }
}

void continuousSweepSensorPan() {
  if (currentMicros - lastSensorPanSweepTime >= 50000) { // 50ms between steps
    sensorPanSweepPos += sensorPanSweepDirection * 2;
    if (sensorPanSweepPos >= sensorPanRangeMax) {
      sensorPanSweepPos = sensorPanRangeMax;
      sensorPanSweepDirection = -1;
    } else if (sensorPanSweepPos <= sensorPanRangeMin) {
      sensorPanSweepPos = sensorPanRangeMin;
      sensorPanSweepDirection = 1;
    }
    sensorPanServo.write(sensorPanSweepPos);
    lastSensorPanSweepTime = currentMicros;
  }
}
bool checkLEDConnection() {
  return true; // Stub: assume connected by default.
}

bool ledsConnected = true;

void flashWhiteTwice() {
  pixels.fill(pixels.Color(255, 255, 255));
  pixels.show();
  delay(250);
  pixels.clear();
  pixels.show();
  delay(250);
  pixels.fill(pixels.Color(255, 255, 255));
  pixels.show();
  delay(250);
  pixels.clear();
  pixels.show();
  delay(250);
}

// NEW LED PIZZAZZ FUNCTIONS
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

//========================================
//------ NEW STARTUP SEQUENCE -----------
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
  
  // Sweep to maximum - use simpler approach
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
  
  // Return to home - use simpler approach
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
  
  // Step 1: Triple flash each LED sequentially
  Serial.println("LED Startup Sequence");
  startupLEDSequence();
  Serial.println("LED sequence complete");
  
  // Step 2: Chin servo full rotation (270 degree servo)
  Serial.println("Testing Chin Pan Servo (270 degree)");
  sweepServoStartup(chinPanServo, chinPanRangeMin, chinPanRangeMax, chinPanStop);
  
  // Step 3: Eye pan servo full rotation (180 degree servo - keeping original range)
  Serial.println("Testing Eye Pan Servo (180 degree)");
  sweepServoStartup(eyePanServo, eyePanRangeMin, eyePanRangeMax, eyePanStop);
  
  // Step 4: Eye tilt servo min/max (180 degree servo)
  Serial.println("Testing Eye Tilt Servo (SG90/FS90R)");
  sweepServoStartup(eyeTiltServo, eyeTiltRangeMin, eyeTiltRangeMax, eyeTiltStop);
  
  // Step 5: Sensor pan servo full rotation (270 degree servo)
  Serial.println("Testing Sensor Pan Servo (270 degree)");
  sweepServoStartup(sensorPanServo, sensorPanRangeMin, sensorPanRangeMax, sensorPanStop);
  
  // Step 6: Blink all LEDs green twice
  Serial.println("Startup Complete - Ready");
  blinkAllGreenTwice();
}

//========================================
//---------------- SETUP -----------------
//========================================
void setup() {
  Serial.begin(9600);
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

  // Run the new startup sequence
  startupSequence();

  // Initialize mode-switching:
  // Start in ACTIVE mode after startup sequence, then switch to idle later
  activeMode = true;  // Changed from false to true
  modeSwitchTime = millis();
  modeDuration = random(300000, 600000); // Start with 5-10 minutes of active time
}

//========================================
//---------------- LOOP ------------------
//========================================
void loop() {
  unsigned long now = millis();
  
  // Check if it's time to toggle mode:
  if (now - modeSwitchTime >= modeDuration) {
    activeMode = !activeMode;      // Toggle mode
    modeSwitchTime = now;          // Reset mode start time
    if (activeMode) {
      modeDuration = random(300000, 600000);   // Active duration: 5-10 minutes
    } else {
      modeDuration = random(900000, 1800000);    // Idle duration: 15-30 minutes
    }
  }
  
  if (!activeMode) {
    // In idle mode, do nothing (or you can add a simple LED indicator here).
    return;
  }
  
  // Active mode: run normal operations
  
  // LED connection check block
  bool currentLEDStatus = checkLEDConnection();
  if (!currentLEDStatus && ledsConnected) {
    ledsConnected = false;
  } else if (currentLEDStatus && !ledsConnected) {
    flashWhiteTwice();
    ledsConnected = true;
  }
  
  // LED pizzazz logic:
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
  
  currentMicros = micros();
  
  // Check for continuous sweep modes (overrides normal movement)
  if (chinPanContinuousSweep) {
    continuousSweepChinPan();
  } else {
    moveChinPan();
  }
  
  if (eyePanContinuousSweep) {
    continuousSweepEyePan();
  } else {
    moveEyePan();
  }
  
  if (eyeTiltContinuousSweep) {
    continuousSweepEyeTilt();
  } else {
    moveEyeTilt();
  }
  
  if (sensorPanContinuousSweep) {
    continuousSweepSensorPan();
  } else {
    moveSensorPan();
  }
}

//========================================
//------------- FUNCTIONS --------------
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