void setup() {
  verticalServo.attach(9);    // attaches the vertical servo to pin 9
  horizontalServo.attach(10); // attaches the horizontal servo to pin 10

  // Move servos to home position
  moveVerticalServo(0);       // move the vertical servo to the home position
  horizontalServo.write(0);   // move the horizontal servo to the home position
  
  randomSeed(analogRead(0));  // initialize random number generator
}
// Randomly sweep the horizontal servo
randomInterval = random(2000, 5000);   // get a random interval between 2 and 5 seconds
int randomAngle = random(0, 161);       // get a random angle between 0 and 160 degrees

horizontalServo.write(randomAngle);    // sweep the horizontal servo to the random angle
delay(randomInterval);                 // wait for the random interval
void setup() {
  verticalServo.attach(9);    // attaches the vertical servo to pin 9
  horizontalServo.attach(10); // attaches the horizontal servo to pin 10

  // Move servos to home position
  moveVerticalServo(0);       // move the vertical servo to the home position
  horizontalServo.write(0);   // move the horizontal servo to the home position
  
  randomSeed(analogRead(0));  // initialize random number generator
}

void loop() {
  // Randomly move the vertical servo
  int randomInterval = random(1000, 5000);  // get a random interval between 1 and 5 seconds
  int randomDirection = random(0, 2);       // get a random direction (0 or 1)
  
  if (randomDirection == 0) {
    moveVerticalServo(180);  // move the vevoid setup() {
      verticalServo.attach(9);    // attaches the vertical servo to pin 9
      horizontalServo.attach(10); // attaches the horizontal servo to pin 10
    
      // Move servos to home position
      moveVerticalServo(0);       // move the vertical servo to the home position
      horizontalServo.write(0);   // move the horizontal servo to the home position
      
      randomSeed(analogRead(0));  // initialize random number generator
    }
    
    void loop() {
      // Randomly move the vertical servo
      int randomInterval = random(1000, 5000);  // get a random interval between 1 and 5 seconds
      int randomDirection = random(0, 2);       // get a random direction (0 or 1)
      
      if (randomDirection == 0) {
        moveVerticalServo(180);  // move the vertical servo to the up position
      } else {
        moveVerticalServo(0);    // move the vertical servo to the down position
      }
      
      delay(randomInterval);  // wait for the random interval
    
      // Hold the position for 2 seconds
      moveVerticalServo(90);  // move the vertical servo to the middle position
      delay(2000);             // hold the position for 2 seconds
    
      // Return to home position
      moveVerticalServo(0);   // move the vertical servo to the home position
      
      // Randomly sweep the horizontal servo
      randomInterval = random(2000, 5000);   // get a random interval between 2 and 5 seconds
      int randomAngle = random(0, 161);       // get a random angle between 0 and 160 degrees
    
      horizontalServo.write(randomAngle);    // sweep the horizontal servo to the random angle
      delay(randomInterval);                 // wait for the random interval
    }
    initialize_pir_sensor()
    initialize_servos()
    
    while True:
       if pir_sensor_triggered():
          if not motion_sequence_running():
             run_motion_sequence()
             set_motion_sequence_running_flag()
             start_timeout_timer()
          
       if timeout_elapsed():
          reset_motion_sequence_running_flag()
    rtical servo to the up position
  } else {
    moveVerticalServo(0);    // move the vertical servo to the down position
  }
  
  delay(randomInterval);  // wait for the random interval

  // Hold the position for 2 seconds
  moveVerticalServo(90);  // move the vertical servo to the middle position
  delay(2000);             // hold the position for 2 seconds

  // Return to home position
  moveVerticalServo(0);   // move the vertical servo to the home position
  
  // Randomly sweep the horizontal servo
  randomInterval = random(2000, 5000);   // get a random interval between 2 and 5 seconds
  int randomAngle = random(0, 161);       // get a random angle between 0 and 160 degrees

  horizontalServo.write(randomAngle);    // sweep the horizontal servo to the random angle
  delay(randomInterval);                 // wait for the random interval
}
