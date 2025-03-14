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
const int chinPanStop = 89;       // Servo stopped (360° servo)
const int eyePanStop = 90;        // Middle of Pan
const int eyeTiltStop = 60;       // Eyes are level (RECHECK value every time head is reassembled)
const int sensorPanStop = 89;     // Servo stopped (360° servo)

// Set servo speed limits
const int chinPanSpeedRange = 5;   // Units unclear. Degrees/(step? ms?)
const int eyePanStepMax = 2;         // Degrees/step 
const int eyePanStepMin = 1;         // Degrees/step
const int eyeTiltStepMax = 2;        // Degrees/step
const int eyeTiltStepMin = 1;        // Degrees/step 
const int sensorPanSpeedRange = 8; // Units unclear. Degrees/(step? ms?)
const int chinPanLeftSpeed = chinPanStop - chinPanSpeedRange;        // servo max speed left (360°)
const int chinPanRightSpeed = chinPanStop + chinPanSpeedRange;       // servo max speed right (360°)
const int sensorPanLeftSpeed = sensorPanStop - sensorPanSpeedRange;  // servo max speed left (360°)
const int sensorPanRightSpeed = sensorPanStop + sensorPanSpeedRange; // servo max speed right (360°)

// Set servo moving time limits (microseconds)
unsigned long  chinPanIntervalMax =    4000000;   // MAX time servo will move (360)
unsigned long  chinPanIntervalMin =    2000000;   // MIN time servo will move (360)
unsigned long  eyePanIntervalMax =    20000;     // Max duration between steps
unsigned long  eyePanIntervalMin =    8000;      // MIN duration between steps
unsigned long  eyeTiltIntervalMax =   10000;     // Max duration between steps
unsigned long  eyeTiltIntervalMin =   1000;      // MIN duration between steps
unsigned long  sensorPanIntervalMax =  4000000;   // MAX time servo will move (360)
unsigned long  sensorPanIntervalMin =  1000000;   // MIN time servo will move (360)
unsigned long  pixelIntervalMax =   200000;
unsigned long  pixelIntervalMin =   50000; 

// Set wait time limits (microseconds)
const unsigned long  chinPanWaitMax =   3000000;      // Longest time the chin pan servo can wait (micro seconds)
const unsigned long  chinPanWaitMin =   2000000;         // Shortest time the chin pan servo can wait
const unsigned long  eyePanWaitMax =    4000000;       // ...
const unsigned long  eyePanWaitMin =    2000000;
const unsigned long  eyeTiltWaitMax =   5000000;       // ...
const unsigned long  eyeTiltWaitMin =   2000000;
const unsigned long  sensorPanWaitMax = 3000000;       // ...
const unsigned long  sensorPanWaitMin = 2000000;
const unsigned long  pixelWaitMax =     4000000;       // ...
const unsigned long  pixelWaitMin =     100000;

//========================================
//--------------VARIABLES-----------------
//========================================
// Time keeping (microseconds)
unsigned long currentMicros = 0;                        // Current Time
unsigned long previousChinPanMicros =   currentMicros;  // the time when the servo was last moved
unsigned long previousEyePanMicros =    currentMicros;  
unsigned long previousEyeTiltMicros =   currentMicros; 
unsigned long previousSensorPanMicros = currentMicros; 
unsigned long previousPixelMicros =     currentMicros;

int  chinPanSpeed =     chinPanStop;   // servo step size (degrees)
int  eyePanPosition =   eyePanStop;    // current angle of the servo (degrees)
int  eyeTiltPosition =  eyeTiltStop;   // current angle of the servo (degrees)
int  sensorPanSpeed =   sensorPanStop; // servo step size (degrees)

// For 180° or 270° servos
int            eyePanTarget = eyePanStop;             // Initial target (degrees)
int            eyePanStep = eyePanStepMin;            // Initial speed (degrees/step)
unsigned long  eyePanContact = previousEyePanMicros;  // Initial time (microseconds)
int            eyeTiltTarget = eyeTiltStop;           // Initial target (degrees)
int            eyeTiltStep = eyeTiltStepMin;          // Initial speed (degrees/step)
unsigned long  eyeTiltContact = previousEyePanMicros; // Initial time (microseconds)

// Set initial wait time (us)
unsigned long  chinPanWait = chinPanWaitMin; 
unsigned long  eyePanWait = eyePanWaitMin;
unsigned long  eyeTiltWait = eyeTiltWaitMin;
unsigned long  sensorPanWait = sensorPanWaitMin;
unsigned long  pixelWait = pixelWaitMin;

// Set initial move time (us)                     
unsigned long  chinPanInterval = chinPanIntervalMin;      // Duration of servo pan
unsigned long  eyePanInterval = eyePanIntervalMin;        // Time between servo steps
unsigned long  eyeTiltInterval = eyeTiltIntervalMin;      // Time between servo steps
unsigned long  sensorPanInterval = sensorPanIntervalMin;  // Duration of servo pan
unsigned long  pixelInterval = pixelIntervalMin;

// create objects
Servo chinPanServo;  
Servo eyePanServo;
Servo eyeTiltServo;
Servo sensorPanServo;
Adafruit_NeoPixel pixels(pixelNum, pixelPin, NEO_GRB + NEO_KHZ800);

//========================================
//---------------- SETUP -----------------
//========================================
void setup() {
  
// Servo Setup
  Serial.begin(9600);
  Serial.println("Starting IG-11");       // Tell user name of program
  
  chinPanServo.write(chinPanSpeed);       // Set servo to initial position (but doesn't actually move servo?)
  chinPanServo.attach(chinPanPin);        // Tell program which pin the servo is on
  eyePanServo.write(eyePanPosition);
  eyePanServo.attach(eyePanPin);
  eyeTiltServo.write(eyeTiltPosition);
  eyeTiltServo.attach(eyeTiltPin);
  sensorPanServo.write(sensorPanSpeed);
  sensorPanServo.attach(sensorPanPin);

// LED setup
  pixels.begin();            // INITIALIZE NeoPixel pixels object
  pixels.show();             // Turn OFF all pixels
  pixels.setBrightness(200); // Set BRIGHTNESS (max = 255)
}

//========================================
//---------------- LOOP ------------------
//========================================
void loop() {                                // Loop these functions
  currentMicros = micros();                  // Update total run time

//  Serial.println("CHIN PAN")              // Tell user the chinPanServo will be checked
  moveChinPan();                             // Check chinPanServo
//  Serial.println("EYE PAN")               // Tell user the eyePanServo will be checked
  moveEyePan();                              // Check eye pan servo
//  Serial.println("EYE TILT")     
  moveEyeTilt();                             // Check eye tilt servo
//  Serial.println("SENSOR PAN")
  moveSensorPan();                           // Check sensor pan servo
  
// Choose moveLED01 OR moveLED02, comment out the other.
//  Serial.println("LEDs solid red")         // Tell user the eyePanServo will be checked
  moveLED01();                             // Check LEDs
//Serial.println("LEDs random flashing")
//  moveLED02();
}

//======================================
//------------- FUNCTIONS --------------
//======================================
void moveChinPan() {                                                                                  // Check chinPanServo
//  Serial.print("  Current Time           "); Serial.println(currentMicros);                             // Tell user the total run time
//  Serial.print("  chinPanInterval        "); Serial.println(chinPanInterval);
//  Serial.print("  previousChinPanMicros  "); Serial.println(previousChinPanMicros);
//  Serial.println("  - Is chin move time complete?");
  if (currentMicros >= chinPanInterval + previousChinPanMicros){                                       // Has the move time elapsed?
    chinPanSpeed = chinPanStop;                                                                           // YES
    chinPanServo.write(chinPanSpeed);                                                                       // Stop chin servo
//    Serial.println("      YES, chin move complete. Servo stopped.");                                        // Tell user the servo has stopped                  
//    Serial.println("   - Has the chin servo stopped long enough?");
    if (currentMicros >= chinPanWait + chinPanInterval + previousChinPanMicros) {                           // Has the servo waited long enough?
                                                                                                               // YES
//      Serial.println("        YES, chin stop complete.");                                                        // Tell user the wait is over                                   

//      Serial.print("    OLD previousChinPanMicros  "); Serial.println(previousChinPanMicros);                    // Tell user old time sum
      previousChinPanMicros = previousChinPanMicros + chinPanInterval + chinPanWait;                               // Add wait and move time to time sum
//      Serial.print("    NEW previousChinPanMicros  "); Serial.println(previousChinPanMicros);                    // Tell user new time sum

//      Serial.print("    OLD chinPanInterval  "); Serial.println(chinPanInterval);                                // Tell user old move time
      chinPanInterval = random(chinPanIntervalMin, chinPanIntervalMax);                                            // Choose new move time
//      Serial.print("    NEW chinPanInterval  "); Serial.println(chinPanInterval);                                // Tell user new move time

//      Serial.print("    OLD chinPanWait  "); Serial.println(chinPanWait);                                        // Tell user old wait time
      chinPanWait = random(chinPanWaitMin, chinPanWaitMax);                                                        // Choose new wait time
//      Serial.print("    NEW chinPanWait  "); Serial.println(chinPanWait);                                        // Tell user new wait time

//      Serial.print("    OLD chinPanSpeed  "); Serial.println(chinPanSpeed);                                      // Tell user old move speed
      chinPanSpeed = random(chinPanLeftSpeed, chinPanRightSpeed);                                                  // Choose new move speed
//      Serial.print("    NEW chinPanSpeed  "); Serial.println(chinPanSpeed);                                      // Tell user new move speed
      chinPanServo.write(chinPanSpeed);                                                                            // Start moving servo
    }
  }
} // function

//======================================
void moveEyePan() {                                                     // check eye pan servo
//  Serial.print("  eyePanWait is "); Serial.println(eyePanWait);         // tell user
//  Serial.print("  eyePanPosition "); Serial.println(eyePanPosition);
//  Serial.print("  eyePanTarget "); Serial.println(eyePanTarget);
  if (currentMicros >= eyePanInterval + previousEyePanMicros){            // if its time to move
    if (eyePanPosition < eyePanTarget) {                                     // if position is left of target, 
//       Serial.print("    Eyes pan right "); Serial.println(eyePanStep);     // tell user
       eyePanPosition += eyePanStep;                                          // set position one step right
       previousEyePanMicros = currentMicros;                                  // record time
         if (eyePanPosition >= eyePanTarget) {                                  // If position reached or passed target
          eyePanPosition = eyePanTarget;                                         // Set position to target
           eyePanContact = currentMicros;                                        // log current time
         }
       eyePanServo.write(eyePanPosition);                                  // Move servo
    }
    if (eyePanPosition > eyePanTarget) {                                    // if postion is right of target,
//    Serial.print("    Eyes pan left "); Serial.println(eyePanStep);       // tell user
      eyePanPosition -= eyePanStep;                                           // setposition one step left
      previousEyePanMicros = currentMicros;                                   // record time
      if (eyePanPosition < 0) eyePanPosition = 0;                               // If position is negative, set positon to 0
      if (eyePanPosition <= eyePanTarget) {                                     // If position reached or passed target
        eyePanPosition = eyePanTarget;                                          // Set position to target
        eyePanContact = currentMicros;                                          // log current time
      }
      eyePanServo.write(eyePanPosition);                                       // Move servo
    }
    if (eyePanPosition == eyePanTarget) {                                     // if position is at target
//    Serial.print("  reached target at "); Serial.println(eyePanContact);    // tell user
//    Serial.print("  stop for "); Serial.println(eyePanWait);
//    Serial.print("  current time "); Serial.println(currentMicros);
//    Serial.print("  waiting for "); Serial.println(eyePanWait + eyePanContact);
      if (eyePanWait + eyePanContact <= currentMicros) {                      // if wait time is complete
//       Serial.println(" wait complete");                                    // tell user
        eyePanWait = random(eyePanWaitMin, eyePanWaitMax);                      // select new wait time
        eyePanTarget = random(eyePanRangeMin, eyePanRangeMax);                  // select new target
        eyePanStep = random(eyePanStepMin, eyePanStepMax);                      // select new step size
        eyePanInterval = random(eyePanIntervalMin, eyePanIntervalMax);          // select new interval
//       Serial.print("   new target at "); Serial.println(eyePanTarget);       // tell user new target
    }
  }
  }
}
  //======================================
void moveEyeTilt() {                                                      // check eye tilt servo
//  Serial.print("  eyeTiltWait is "); Serial.println(eyeTiltWait);         // tell user
//  Serial.print("  eyeTiltPosition "); Serial.println(eyeTiltPosition);
//  Serial.print("  eyeTiltTarget "); Serial.println(eyeTiltTarget);
  if (eyeTiltPosition < eyeTiltTarget) {                                    // if position is below target, 
//     Serial.print("    Eyes Tilt up "); Serial.println(eyeTiltStep);        // tell user
     eyeTiltPosition += eyeTiltStep;                                          // set position one step up

       if (eyeTiltPosition >= eyeTiltTarget) {                                // If position reached or passed target
         eyeTiltPosition = eyeTiltTarget;                                       // Set position to target
         eyeTiltContact = currentMicros;                                        // log current time
       }
       eyeTiltServo.write(eyeTiltPosition);                                   // move servo
  }
  if (eyeTiltPosition > eyeTiltTarget) {                                   // if postion is above of target,
//    Serial.print("    Eyes Tilt down "); Serial.println(eyeTiltStep);      // tell user
    eyeTiltPosition -= eyeTiltStep;                                          // setposition one step down
    if (eyeTiltPosition < 0) eyeTiltPosition = 0;

    if (eyeTiltPosition <= eyeTiltTarget) {                                   // If position reached or passed target
      eyeTiltPosition = eyeTiltTarget;                                          // Set position to target
      eyeTiltContact = currentMicros;                                           // log current time
    }
    eyeTiltServo.write(eyeTiltPosition);                                      // Move servo
  }
  if (eyeTiltPosition == eyeTiltTarget) {                                   // if servo is at target
//    Serial.print("  reached target at "); Serial.println(eyeTiltContact);   // tell user
//    Serial.print("  stop for "); Serial.println(eyeTiltWait);
//    Serial.print("  current time "); Serial.println(currentMicros);
//    Serial.print("  waiting for "); Serial.println(eyeTiltWait + eyeTiltContact);
    if (eyeTiltWait + eyeTiltContact <= currentMicros) {                      // if wait time is complete,
//       Serial.println(" wait complete");                                      // tell user
       eyeTiltWait = random(eyeTiltWaitMin, eyeTiltWaitMax);                    // select new wait time
       eyeTiltTarget = random(eyeTiltRangeMin, eyeTiltRangeMax);                // select new target
       eyeTiltStep = random(eyeTiltStepMin, eyeTiltStepMax);                    // select new step size
//       Serial.print("   new target at "); Serial.println(eyeTiltTarget);      // tell user new target
    }
  }
}
  
//======================================
void moveSensorPan() {                                                                             // Check SensorPanServo
    //  Serial.print("  Current Time           "); Serial.println(currentMicros);                        // Tell user the total run time
    //  Serial.print("  SensorPanInterval        "); Serial.println(sensorPanInterval);
    //  Serial.print("  previousServoPanMicros  "); Serial.println(previousSensorPanMicros);
    //  Serial.println("  - Is sensor move time complete?");
      if (currentMicros >= sensorPanInterval + previousSensorPanMicros){                                 // Has the move time elapsed?
        sensorPanSpeed = sensorPanStop;                                                                     // YES
        sensorPanServo.write(sensorPanSpeed);                                                                  // Stop sensor servo
    //   Serial.println("      YES, sensor move complete. Servo stopped.");                                    // Tell user the servo has stopped                  
    //   Serial.println("   - Has the sensor servo stopped long enough?");
        if (currentMicros >= sensorPanWait + sensorPanInterval + previousSensorPanMicros) {                  // Has the servo waited long enough?
                                                                                                                // YES
    //      Serial.println("        YES, sensor stop complete.");                                               // Tell user the wait is over                                   
    
    //      Serial.print("    OLD previousSensorPanMicros  "); Serial.println(previousSensorPanMicros);         // Tell user old time sum
          previousSensorPanMicros = previousSensorPanMicros + sensorPanInterval + sensorPanWait;                // Add wait and move time to time sum
    //      Serial.print("    NEW previousSensorPanMicros  "); Serial.println(previousSensorPanMicros);         // Tell user new time sum
    
    //      Serial.print("    OLD sensorPanInterval  "); Serial.println(sensorPanInterval);                     // Tell user old move time
          sensorPanInterval = random(sensorPanIntervalMin, sensorPanIntervalMax);                               // Choose new move time
    //      Serial.print("    NEW sensorPanInterval  "); Serial.println(sensorPanInterval);                     // Tell user new move time
    
    //      Serial.print("    OLD sensorPanWait  "); Serial.println(sensorPanWait);                             // Tell user old wait time
          sensorPanWait = random(sensorPanWaitMin, sensorPanWaitMax);                                           // Choose new wait time
    //      Serial.print("    NEW sensorPanWait  "); Serial.println(sensorPanWait);                             // Tell user new wait time
    
    //      Serial.print("    OLD sensorPanSpeed  "); Serial.println(sensorPanSpeed);                           // Tell user old move speed
          sensorPanSpeed = random(sensorPanLeftSpeed, sensorPanRightSpeed);                                     // Choose new move speed
    //      Serial.print("    NEW sensorPanSpeed  "); Serial.println(sensorPanSpeed);                           // Tell user new move speed
          sensorPanServo.write(sensorPanSpeed);                                                                 // Start moving servo
        }
      } // second else
    } // function
    
    //======================================
    void moveLED01() { 
      // solid red lights
      // even though the LEDs don't change, the function must be in the loop. If slip ring breaks connection, function is needed to turn LEDs back on.
      // Probably over complicated, but a simpler function causes severe servo chatter.
      
      if(currentMicros >= pixelInterval + previousPixelMicros) {            // if interval has passed
        uint32_t red = pixels.Color(250, 0, 0);                             // create color "red"
        pixels.fill(red, 0, pixelNum);                                      // Change all LED values to red
        pixels.show();                                                      // Change LEDs
        previousPixelMicros = currentMicros;                                // record time
        pixelInterval = random(pixelIntervalMin, pixelIntervalMax);         // choose new inteval
      }
    }
    
    //======================================
    void moveLED02() { 
    // flash random colors
     
      if(currentMicros >= pixelInterval + previousPixelMicros){                      // if interval has passed
        uint32_t rainbow = pixels.Color(random(0,250),random(0,250),random(0,250));  // create random color "rainbow"
        pixels.fill(rainbow, 0, pixelNum);                                           // change all LED values to rainbow
        pixels.show();                                                               // change LEDs
        previousPixelMicros = currentMicros;                                         // record time
        pixelInterval = random(pixelIntervalMin, pixelIntervalMax);                  // choose new interval
      }
    }
     
    //========================================
    //--------------- THE END ----------------
    //========================================
    