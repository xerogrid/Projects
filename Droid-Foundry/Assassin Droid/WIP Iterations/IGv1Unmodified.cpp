//////////////////////////////////////////
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
// For servos
#include <Servo.h>                // Get Servo library

// For WS2812B LEDs
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

// Servo limits
const int eyePanRangeMax = 180;     // Servo Max angle
const int eyePanRangeMin = 0;       // Servo min angle
const int eyeTiltRangeMax = 110;    // Servo max angle (RECHECK value every time head is reassembled)
const int eyeTiltRangeMin = 0;      // Servo min angle (RECHECK value every time head is reassembled)

// Neutral position (determined experimentally)
const int chinPanStop = 135;       // Servo stopped (360° servo)
const int eyePanStop = 90;         // Middle of Pan
const int eyeTiltStop = 60;        // Eyes are level (RECHECK value every time head is reassembled)
const int sensorPanStop = 89;      // Servo stopped (360° servo)

// Set servo speed limits
const int chinPanSpeedRange = 5;   // Units unclear. Degrees/(step? ms?)
const int eyePanStepMax = 2;       // Degrees/step 
const int eyePanStepMin = 1;       // Degrees/step
const int eyeTiltStepMax = 2;      // Degrees/step
const int eyeTiltStepMin = 1;      // Degrees/step 
const int sensorPanSpeedRange = 8; // Units unclear. Degrees/(step? ms?)
const int chinPanLeftSpeed = chinPanStop - chinPanSpeedRange;        // servo max speed left (360°)
const int chinPanRightSpeed = chinPanStop + chinPanSpeedRange;       // servo max speed right (360°)
const int sensorPanLeftSpeed = sensorPanStop - sensorPanSpeedRange;  // servo max speed left (360°)
const int sensorPanRightSpeed = sensorPanStop + sensorPanSpeedRange; // servo max speed right (360°)

// Set servo moving time limits (microseconds)
unsigned long  chinPanIntervalMax =    3000000;   // MAX time servo will move (360)
unsigned long  chinPanIntervalMin =    1000000;   // MIN time servo will move (360)
unsigned long  eyePanIntervalMax =    20000;     // Max duration between steps
unsigned long  eyePanIntervalMin =    8000;      // MIN duration between steps
unsigned long  eyeTiltIntervalMax =   10000;     // Max duration between steps
unsigned long  eyeTiltIntervalMin =   1000;      // MIN duration between steps
unsigned long  sensorPanIntervalMax =  4000000;   // MAX time servo will move (360)
unsigned long  sensorPanIntervalMin =  1000000;   // MIN time servo will move (360)
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
const unsigned long  pixelWaitMax =     4000000;       
const unsigned long  pixelWaitMin =     100000;

//========================================
//--------------VARIABLES-----------------
//========================================
// Time keeping (microseconds)
unsigned long currentMicros = 0;                        
unsigned long previousChinPanMicros =   currentMicros;  
unsigned long previousEyePanMicros =    currentMicros;  
unsigned long previousEyeTiltMicros =   currentMicros; 
unsigned long previousSensorPanMicros = currentMicros; 
unsigned long previousPixelMicros =     currentMicros;

int  chinPanSpeed =     chinPanStop;   // servo step size (degrees)
int  eyePanPosition =   eyePanStop;    // current angle of the servo (degrees)
int  eyeTiltPosition =  eyeTiltStop;   // current angle of the servo (degrees)
int  sensorPanSpeed =   sensorPanStop; // servo step size (degrees)

// For 180° or 270° servos
int            eyePanTarget = eyePanStop;             
int            eyePanStep = eyePanStepMin;            
unsigned long  eyePanContact = previousEyePanMicros;  
int            eyeTiltTarget = eyeTiltStop;           
int            eyeTiltStep = eyeTiltStepMin;          
unsigned long  eyeTiltContact = previousEyePanMicros; 

// Set initial wait time (us)
unsigned long  chinPanWait = chinPanWaitMin; 
unsigned long  eyePanWait = eyePanWaitMin;
unsigned long  eyeTiltWait = eyeTiltWaitMin;
unsigned long  sensorPanWait = sensorPanWaitMin;
unsigned long  pixelWait = pixelWaitMin;

// Set initial move time (us)                     
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

//========================================
//----- NEW NIGHTRIDER & SERVO TEST ------
//========================================

// Global variables for the nightrider LED effect
int nightriderIndex = 0;
int nightriderDirection = 1;

// Update the LED strip with a single green LED moving in "nightrider" style
void updateNightriderGreen() {
  pixels.clear();
  pixels.setPixelColor(nightriderIndex, pixels.Color(0, 250, 0));
  pixels.show();
  nightriderIndex += nightriderDirection;
  if(nightriderIndex >= pixelNum) {
    nightriderIndex = pixelNum - 2;
    nightriderDirection = -1;
  } else if(nightriderIndex < 0) {
    nightriderIndex = 1;
    nightriderDirection = 1;
  }
}

// A non-blocking delay replacement for test mode that updates the LED effect
void testDelay(unsigned long ms) {
  unsigned long start = millis();
  while(millis() - start < ms) {
    updateNightriderGreen();
    delay(200);
  }
}

// Gradually sweep a servo from startAngle to endAngle, updating the LED effect as it moves
void sweepServo(Servo &servo, int startAngle, int endAngle, int stepDelay) {
  int step = (endAngle > startAngle) ? 1 : -1;
  for (int angle = startAngle; angle != endAngle; angle += step) {
    servo.write(angle);
    updateNightriderGreen();
    delay(stepDelay);
  }
  servo.write(endAngle);
  updateNightriderGreen();
  delay(stepDelay);
}

// Test a given servo by sweeping from home to home+offset then home, then home to home-offset then home
void testServo(Servo &servo, int home, int offset, int stepDelay) {
  servo.write(home);
  delay(200);
  sweepServo(servo, home, home + offset, stepDelay);
  sweepServo(servo, home + offset, home, stepDelay);
  sweepServo(servo, home, home - offset, stepDelay);
  sweepServo(servo, home - offset, home, stepDelay);
}

// Run tests for all servos sequentially
void servoTest() {
  // Test chin pan servo (sweep 20° each direction)
  testServo(chinPanServo, chinPanStop, 20, 20);
  // Test eye pan servo (sweep 20° each direction)
  testServo(eyePanServo, eyePanStop, 20, 20);
  // Test eye tilt servo (sweep 10° each direction)
  testServo(eyeTiltServo, eyeTiltStop, 10, 20);
  // Test sensor pan servo (sweep 20° each direction)
  testServo(sensorPanServo, sensorPanStop, 20, 20);
}

//========================================
//------ NEW LED CONNECTION LOGIC ------
//========================================

// Stub function to check LED connection status.
// In a real implementation, this should check a hardware signal.
bool checkLEDConnection() {
  return true; // Assume connected by default.
}

// Global variable to store last known LED connection state.
bool ledsConnected = true;

// Flash white twice with 250ms intervals
void flashWhiteTwice() {
  // First flash
  pixels.fill(pixels.Color(255, 255, 255));
  pixels.show();
  delay(250);
  // Off for 250ms
  pixels.clear();
  pixels.show();
  delay(250);
  // Second flash
  pixels.fill(pixels.Color(255, 255, 255));
  pixels.show();
  delay(250);
  // Off for 250ms
  pixels.clear();
  pixels.show();
  delay(250);
}

// NEW LED PIZZAZZ FUNCTIONS

// Blink all LEDs twice as red then resume normal state.
void blinkTwiceRed() {
  // First red flash at full brightness
  pixels.fill(pixels.Color(250, 0, 0));
  pixels.show();
  delay(250);
  // Off
  pixels.clear();
  pixels.show();
  delay(250);
  // Second red flash
  pixels.fill(pixels.Color(250, 0, 0));
  pixels.show();
  delay(250);
  // Off
  pixels.clear();
  pixels.show();
  delay(250);
}

// Blink all LEDs twice as red at half brightness (no fade).
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
//---------------- SETUP -----------------
//========================================
void setup() {
  Serial.begin(9600);
  Serial.println("Starting IG-11");

  chinPanServo.write(chinPanSpeed);
  chinPanServo.attach(chinPanPin);
  eyePanServo.write(eyePanPosition);
  eyePanServo.attach(eyePanPin);
  eyeTiltServo.write(eyeTiltPosition);
  eyeTiltServo.attach(eyeTiltPin);
  sensorPanServo.write(sensorPanSpeed);
  sensorPanServo.attach(sensorPanPin);

  pixels.begin();
  pixels.show();
  pixels.setBrightness(200);

  // Send all servos to their home positions
  chinPanServo.write(chinPanStop);
  eyePanServo.write(eyePanStop);
  eyeTiltServo.write(eyeTiltStop);
  sensorPanServo.write(sensorPanStop);
  delay(500); // Allow servos to reach home

  // Run servo tests with nightrider green LED effect
  servoTest();

  // (Optional) Pause briefly after tests before entering main loop
  delay(1000);
}

//========================================
//---------------- LOOP ------------------
//========================================
void loop() {
  // LED connection check block
  bool currentLEDStatus = checkLEDConnection();
  if (!currentLEDStatus && ledsConnected) {
    // LED disconnected event detected
    ledsConnected = false;
  } else if (currentLEDStatus && !ledsConnected) {
    // LED reconnected event detected - flash white twice
    flashWhiteTwice();
    ledsConnected = true;
  }
  
  // NEW LED PIZZAZZ LOGIC:
  // While running in the main loop (normal solid red mode),
  // occasionally (e.g., 10% chance) trigger one of two special patterns.
  if(currentMicros >= pixelInterval + previousPixelMicros) {
    int chance = random(0, 100); // 0 to 99
    if (chance < 10) { // 10% chance for blink twice red at full brightness
      blinkTwiceRed();
    } else if (chance < 20) { // Next 10% chance for blink twice red at half brightness (no fade)
      blinkHalfRedNoFade();
    } else {
      uint32_t red = pixels.Color(250, 0, 0);
      pixels.fill(red, 0, pixelNum);
      // Additionally, randomly turn 1 to 3 LEDs white
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

  moveChinPan();                             
  moveEyePan();                              
  moveEyeTilt();                             
  moveSensorPan();                          
}

//======================================
//------------- FUNCTIONS --------------
//======================================
void moveChinPan() {
    if (currentMicros >= chinPanInterval + previousChinPanMicros) {
      chinPanSpeed = chinPanStop;
      chinPanServo.write(chinPanSpeed);
      if (currentMicros >= chinPanWait + chinPanInterval + previousChinPanMicros) {
        previousChinPanMicros = previousChinPanMicros + chinPanInterval + chinPanWait;
        
        // 50% chance to use sensor pan parameters for chin pan update
        if (random(0, 2) == 0) {
            chinPanInterval = random(sensorPanIntervalMin, sensorPanIntervalMax);
            chinPanWait = random(sensorPanWaitMin, sensorPanWaitMax);
            chinPanSpeed = random(sensorPanLeftSpeed, sensorPanRightSpeed);
        } else {
            chinPanInterval = random(chinPanIntervalMin, chinPanIntervalMax);
            chinPanWait = random(chinPanWaitMin, chinPanWaitMax);
            chinPanSpeed = random(chinPanLeftSpeed, chinPanRightSpeed);
        }
        chinPanServo.write(chinPanSpeed);
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
  if (eyeTiltPosition < eyeTiltTarget) {                                    
     eyeTiltPosition += eyeTiltStep;                                          
       if (eyeTiltPosition >= eyeTiltTarget) {                                
         eyeTiltPosition = eyeTiltTarget;                                       
         eyeTiltContact = currentMicros;                                        
       }
       eyeTiltServo.write(eyeTiltPosition);                                   
  }
  if (eyeTiltPosition > eyeTiltTarget) {                                   
    eyeTiltPosition -= eyeTiltStep;                                          
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
    }
  }
}

void moveSensorPan() {                                                                             
      if (currentMicros >= sensorPanInterval + previousSensorPanMicros) {                                
        sensorPanSpeed = sensorPanStop;                                                                     
        sensorPanServo.write(sensorPanSpeed);                                                                  
          if (currentMicros >= sensorPanWait + sensorPanInterval + previousSensorPanMicros) {                  
          previousSensorPanMicros = previousSensorPanMicros + sensorPanInterval + sensorPanWait;                
          sensorPanInterval = random(sensorPanIntervalMin, sensorPanIntervalMax);                               
          sensorPanWait = random(sensorPanWaitMin, sensorPanWaitMax);                                           
          sensorPanSpeed = random(sensorPanLeftSpeed, sensorPanRightSpeed);                                     
          sensorPanServo.write(sensorPanSpeed);                                                                 
        }
      }
}

void moveLED01() { 
      // This function now randomly triggers one of the two special LED effects,
      // or defaults to the normal solid red state with a few random white LEDs.
      if(currentMicros >= pixelInterval + previousPixelMicros) {
        int chance = random(0, 100); // 0 to 99
        if (chance < 10) { // 10% chance for blink twice red at full brightness
          blinkTwiceRed();
        } else if (chance < 20) { // Next 10% chance for blink twice red at half brightness (no fade)
          blinkHalfRedNoFade();
        } else {
          uint32_t red = pixels.Color(250, 0, 0);
          pixels.fill(red, 0, pixelNum);
          // Randomly turn 1 to 3 LEDs white
          int whiteCount = random(1, 4); // 1 to 3 LEDs
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
    
void moveLED02() { 
      if(currentMicros >= pixelInterval + previousPixelMicros){                      
        uint32_t rainbow = pixels.Color(random(0,250),random(0,250),random(0,250));  
        pixels.fill(rainbow, 0, pixelNum);                                           
        pixels.show();                                                               
        previousPixelMicros = currentMicros;                                         
        pixelInterval = random(pixelIntervalMin, pixelIntervalMax);                  
      }
}

//========================================
//--------------- THE END ----------------
//========================================