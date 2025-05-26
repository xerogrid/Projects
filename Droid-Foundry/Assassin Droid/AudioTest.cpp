#include "SoftwareSerial.h"
#include "DFRobotDFPlayerMini.h"

SoftwareSerial mySoftwareSerial(10, 11);  // RX, TX
DFRobotDFPlayerMini myDFPlayer;
String line;
char command;
int pause = 0;
int repeat = 0;

void setup() {
  mySoftwareSerial.begin(9600);
  Serial.begin(115200);
  
  Serial.println();
  Serial.println(F("DFRobot DFPlayer Mini"));
  Serial.println(F("Initializing DFPlayer module ... Wait!"));

  if (!myDFPlayer.begin(mySoftwareSerial)) {
    Serial.println(F("Not initialized:"));
    Serial.println(F("1. Check the DFPlayer Mini connections"));
    Serial.println(F("2. Insert an SD card"));
    while (true);
  }
  
  Serial.println();
  Serial.println(F("DFPlayer Mini module initialized!"));
  
  // Initial settings
  myDFPlayer.setTimeOut(500);
  myDFPlayer.volume(5);
  myDFPlayer.EQ(0);
  
  // Debug: Check how many files are detected
  delay(1000);  // Give time for SD card to be read
  int fileCount = myDFPlayer.readFileCounts();
  Serial.print(F("Total files detected: "));
  Serial.println(fileCount);
  
  menu_options();
}

void loop() {
  // Check for DFPlayer feedback
  if (myDFPlayer.available()) {
    printDetail(myDFPlayer.readType(), myDFPlayer.read());
  }
  
  // Waits for data entry via serial
  while (Serial.available() > 0) {
    command = Serial.peek();
    line = Serial.readStringUntil('\n');

    // Play from first 9 files
    if ((command >= '1') && (command <= '9')) {
      Serial.print(F("Attempting to play file "));
      Serial.println(command);
      command = command - 48;
      
      // Debug: Show what file number we're trying to play
      Serial.print(F("Converted to file number: "));
      Serial.println(command);
      
      myDFPlayer.play(command);
      
      // Wait a moment and check status
      delay(100);
      Serial.print(F("Current file playing: "));
      Serial.println(myDFPlayer.readCurrentFileNumber());
      
      menu_options();
    }

    // Play from specific folder
    if (command == 'f') {
      int indexF = line.indexOf('f');
      int indexS = line.indexOf('s');
      if (indexF != -1 && indexS != -1 && indexF < indexS) {
        int folder = line.substring(indexF + 1, indexS).toInt();
        int song = line.substring(indexS + 1).toInt();
        Serial.print(F("From folder: "));
        Serial.print(folder);
        Serial.print(F(", playing song: "));
        Serial.println(song);
        myDFPlayer.playFolder(folder, song);
      } else {
        Serial.println(F("Incomplete 'f' command. Specify both folder and song numbers."));
      }
      menu_options();
    }

    // Stop
    if (command == 's') {
      myDFPlayer.stop();
      Serial.println(F("Music Stopped!"));
      menu_options();
    }

    // Pause/Continue the music
    if (command == 'p') {
      pause = !pause;
      if (pause == 0) {
        Serial.println(F("Continue..."));
        myDFPlayer.start();
      }
      if (pause == 1) {
        Serial.println(F("Music Paused!"));
        myDFPlayer.pause();
      }
      menu_options();
    }

    // Test command to check file existence
    if (command == 't') {
      Serial.println(F("Testing all files 1-5:"));
      for (int i = 1; i <= 5; i++) {
        Serial.print(F("Testing file "));
        Serial.print(i);
        Serial.print(F("... "));
        myDFPlayer.play(i);
        delay(2000);  // Play for 2 seconds
        myDFPlayer.pause();
        delay(500);
        Serial.println(F("OK"));
      }
      myDFPlayer.stop();
      menu_options();
    }

    // Toggle repeat mode
    if (command == 'r') {
      repeat = !repeat;
      if (repeat == 1) {
        myDFPlayer.enableLoop();
        Serial.println(F("Repeat mode enabled."));
      } else {
        myDFPlayer.disableLoop();
        Serial.println(F("Repeat mode disabled."));
      }
      menu_options();
    }

    // Set volume
    if (command == 'v') {
      int myVolume = line.substring(1).toInt();
      if (myVolume >= 0 && myVolume <= 30) {
        myDFPlayer.volume(myVolume);
        Serial.print(F("Current Volume:"));
        Serial.println(myDFPlayer.readVolume());
      } else {
        Serial.println(F("Invalid volume level, choose a number between 0-30."));
      }
      menu_options();
    }

    // Increases volume
    if (command == '+') {
      myDFPlayer.volumeUp();
      Serial.print(F("Current Volume:"));
      Serial.println(myDFPlayer.readVolume());
      menu_options();
    }
    // Decreases volume
    if (command == '-') {
      myDFPlayer.volumeDown();
      Serial.print(F("Current Volume:"));
      Serial.println(myDFPlayer.readVolume());
      menu_options();
    }

    // Play previous
    if (command == '<') {
      myDFPlayer.previous();
      Serial.println(F("Previous:"));
      Serial.print(F("Current track:"));
      Serial.println(myDFPlayer.readCurrentFileNumber());
      menu_options();
    }

    // Play next
    if (command == '>') {
      myDFPlayer.next();
      Serial.println(F("Next:"));
      Serial.print(F("Current track:"));
      Serial.println(myDFPlayer.readCurrentFileNumber());
      menu_options();
    }
  }
}

void printDetail(uint8_t type, int value) {
  switch (type) {
    case TimeOut:
      Serial.println(F("Time Out!"));
      break;
    case WrongStack:
      Serial.println(F("Stack Wrong!"));
      break;
    case DFPlayerCardInserted:
      Serial.println(F("Card Inserted!"));
      break;
    case DFPlayerCardRemoved:
      Serial.println(F("Card Removed!"));
      break;
    case DFPlayerCardOnline:
      Serial.println(F("Card Online!"));
      break;
    case DFPlayerUSBInserted:
      Serial.println(F("USB Inserted!"));
      break;
    case DFPlayerUSBRemoved:
      Serial.println(F("USB Removed!"));
      break;
    case DFPlayerPlayFinished:
      Serial.print(F("Number:"));
      Serial.print(value);
      Serial.println(F(" Play Finished!"));
      break;
    case DFPlayerError:
      Serial.print(F("DFPlayerError:"));
      switch (value) {
        case Busy:
          Serial.println(F("Card not found"));
          break;
        case Sleeping:
          Serial.println(F("Sleeping"));
          break;
        case SerialWrongStack:
          Serial.println(F("Get Wrong Stack"));
          break;
        case CheckSumNotMatch:
          Serial.println(F("Check Sum Not Match"));
          break;
        case FileIndexOut:
          Serial.println(F("File Index Out of Bound"));
          break;
        case FileMismatch:
          Serial.println(F("Cannot Find File"));
          break;
        case Advertise:
          Serial.println(F("In Advertise"));
          break;
        default:
          break;
      }
      break;
    default:
      break;
  }
}

void menu_options() {
  Serial.println();
  Serial.println(F("=================================================================================================================================="));
  Serial.println(F("Commands:"));
  Serial.println(F(" [1-9] To select the MP3 file"));
  Serial.println(F(" [t] Test all files 1-5"));
  Serial.println(F(" [fXsY] Play song from folder X, song Y"));
  Serial.println(F(" [s] stopping reproduction"));
  Serial.println(F(" [p] pause/continue music"));
  Serial.println(F(" [r] toggle repeat mode"));
  Serial.println(F(" [vX] set volume to X"));
  Serial.println(F(" [+ or -] increases or decreases the volume"));
  Serial.println(F(" [< or >] forwards or backwards the track"));
  Serial.println();
  Serial.println(F("================================================================================================================================="));
}