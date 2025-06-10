# Assassin Droid - SwitchesRunning.cpp

## Overview
The `SwitchesRunning.cpp` file is responsible for managing the logic and behavior of the switches that control various functions of the Assassin Droid. This includes monitoring the state of physical switches, executing corresponding actions, and ensuring proper operation of the droid's systems.

### Functions in SwitchesRunning.cpp
1. **initializeSwitches()**  
    Sets up the pins and initializes the switches for operation. This function ensures that all switches are ready for use when the droid is powered on.

2. **readSwitchStates()**  
    Continuously monitors the state of each switch and updates the internal logic accordingly. This function is critical for detecting user input and triggering appropriate actions.

3. **executeSwitchActions()**  
    Executes specific actions based on the current state of the switches. For example, activating weapons, changing movement modes, or powering down the droid.

4. **debounceSwitch()**  
    Implements a debounce mechanism to prevent false readings caused by mechanical noise or rapid toggling of switches.

5. **shutdownSequence()**  
    Handles the shutdown process when a specific switch is activated, ensuring all systems are safely powered down.

---

## Physical Assembly Instructions

### Required Components
- Assassin Droid chassis
- Microcontroller (e.g., Arduino or similar)
- Toggle switches (quantity depends on the number of functions)
- Jumper wires
- Resistors (for pull-down or pull-up configurations)
- Breadboard or soldering tools (optional)

### Assembly Steps
1. **Mount the Switches**  
    Securely attach the toggle switches to the droid's chassis in accessible locations. Ensure they are firmly fixed to avoid accidental disconnections during operation.

2. **Wire the Switches**  
    - Connect one terminal of each switch to the microcontroller's digital input pins.
    - Use pull-up or pull-down resistors to stabilize the signal. For pull-up configuration, connect the resistor between the input pin and the microcontroller's power supply. For pull-down, connect it between the input pin and ground.

3. **Connect Power**  
    Ensure the microcontroller and switches are powered correctly. Double-check all connections to avoid short circuits.

4. **Test the Wiring**  
    Before running the code, test each switch manually to confirm proper wiring and functionality. Use a multimeter if necessary.

---

## Wiring Diagram