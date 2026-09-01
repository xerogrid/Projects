# Third-Party Notices

The repository-level MIT license does not replace the terms of third-party
components used to build this firmware.

Production environments in `platformio.ini` download these libraries at build
time; they are not vendored in this repository:

- Arduino Servo (`arduino-libraries/Servo`)
- Adafruit NeoPixel (`adafruit/Adafruit NeoPixel`)
- SoftwareSerial (`featherfly/SoftwareSerial`)
- DFRobot DFPlayer Mini (`dfrobot/DFRobotDFPlayerMini`)

Some archived and WIP sketches also use IRremote (`z3t0/IRremote`) and
ServoSmooth (`gyverlibs/ServoSmooth`). Add those to a named environment
before compiling those files.

Each package keeps its own copyright and license. Review those terms before
redistributing a compiled hex.

`WIP Sketches/DeviceOnlyCommands.ino` is derived from Adafruit's NeoPixel
"simple" example and remains under the GPLv3 license in that file's header.

Any future third-party asset or vendored source must retain its original
copyright and license notice and be added here when the component's own
directory does not make the terms obvious.
