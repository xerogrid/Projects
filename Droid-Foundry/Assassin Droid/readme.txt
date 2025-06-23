# Assassin Droid - FinalVersionLondon.cpp

<Explanation of various logic used in code should go here once overall readme is in a good state>

---

### General Notes


Servos:
Buttom (ChinPan) Servo is a 270° hobby servo. 
Mid-Drive Plate Servo is a 270° hobby servo. 
Turret Servo is a 90° servo. (FS90R)
Top Servo is a 270° servo.

LEDs:
11x Individual NeoPixels with 57mm wiring connects.


---

## Wiring Diagram (Head)

The following information includes the wiring color I used and terminal connect data. 
This can be whatever you prefer but ensure you wire consistent with this layout or modify the code accordingly.
Signal wires are direct to Nano pins/terminals, power go to Servo Control Board.

D2
Bottom Servo
White wire
26" wires

D3
Mid Drive Plate
Blue Wire
31" wires

D4
Turret
Yellow Wire
33" wires

D5
Top Servo
Green Wire
37" wires

D6
NeoPixels/LEDs
Pink wire
37"

Servo Power +/-
Each respective +/- from servos should be appropriately terminated and wired to the installed Servo Breakout Board on the power leads.
// Remember to include a diagram here




## Wiring Diagram (Cockpit Switches)

The following information includes the wiring color I used and terminal connect data. 
This can be whatever you prefer but ensure you wire consistent with this layout or modify the code accordingly.

A1
State Change Button (Momentary keyswitch)
Yellow wire
20" wire

A2
Stop All Systems (Momentary keyswitch)
Green Wire
20" wire

A3
Mute Toggle (Toggle Switch)
White Wire
20" wire

A4
Home Servos (Toggle Switch)
Blue Wire
20" wire

A5
Production Mode (Toggle Switch)
Pink wire
20"


## Physical Assembly Instructions

### Required Components
- Assassin Droid chassis (IG12 version, purchased from Droid Division)
- Microcontroller (An Arduino Nano was used for this code and build)
- Toggle switches (quantity depends on the number of functions)
- Momentary button switches (quantity depends on the number of functions)
- Brendan's custom IG-12 Powerpack (eventual download link will go here)


Head Wiring and Assembly

# Most of this is just follow existing Droid Division assembly instructions for Animatronic IG Head.

##

1. Install various servos in head components. DO NOT install servo horns/gears yet.
2. Pull all servo/LED wiring looms through center tube and pull relevant connector through relevant hole (wires noted above to relevant servo)
3. Begin assembling head, starting with the chin.
4. Wire servos as you go, ensuring ample wiring slack and no binding loops.
5. If done correctly the head should be assembled, and free spinning, with all wires coming out the bottom.


Powerpack Wiring and Assembly

1. 