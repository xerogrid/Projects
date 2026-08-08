# Droid Foundry Project Log

## 2026-08-07 — Convention planning and repository recovery

### Context

The Droid Factory and Droid Foundry GitHub work was recovered locally for an upcoming convention. The convention weekend will be used to demonstrate the static and robotic droids, catch up on the animatronic upgrade backlog, and build upgrades in public.

The IG-12 documented in `Assassin Droid/FinalVersionLondon.cpp` has been operating in production in London for over a year. It is no longer physically accessible and is not a current deployment target. Its code and the lessons learned from that build remain useful reference material for future droids.

### Active convention projects

#### Pit Droid demo head

- Arduino and wiring are installed and ready.
- Currently running the latest production version of the Pit Droid PIR firmware available at the time it was assembled.
- Intended for demonstration at the convention.
- Exact installed firmware version still needs to be confirmed against the repository when the hardware is available.

#### Battle Droid demo head

- Arduino and wiring are installed and ready.
- The behavior was never fully dialed in.
- Firmware and motion behavior need additional development before or during the convention.

#### IG head upgrade

- Still in the physical assembly stage.
- All required parts are on hand.
- Once assembly is complete, the proven London IG code can be used as the initial firmware baseline.

### Repository notes

- `xerogrid/Droid-Factory` contains the older Arduino-era Pit Droid and Battle Droid work.
- The active PlatformIO-era work is under `xerogrid/Projects/Droid-Foundry`.
- The London IG-12 code is a reference baseline, not a request to modify or redeploy the production London unit.

### Current priority

Preserve context across machines now. Detailed convention planning, firmware changes, and hardware validation will follow later.
