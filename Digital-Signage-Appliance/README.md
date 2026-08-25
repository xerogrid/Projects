# Digital Signage Appliance

Animated convention signage for the Fulcrum Builds booth. The first display
target is a Raspberry Pi connected to a portable 14-inch OLED screen.

This project is independent of the BambuLabs VLC and Stream Deck work.

## Sign content

- Fulcrum Builds
- Ask me about my robots
- Photo ops welcome
- 3D Printing · Making · Arduino Robotics

The design combines industrial machinery, gears, and circuit traces with an
original neon-grid aesthetic. It does not use franchise logos or visual assets.

## Preview

No build step or package installation is required:

```bash
cd Digital-Signage-Appliance
python3 -m http.server 8080
```

Open `http://localhost:8080` in a browser. Add `?static` to pause continuous
animation when taking screenshots.

## Raspberry Pi kiosk preview

With Chromium installed, start the local server and open it fullscreen:

```bash
chromium-browser \
  --kiosk \
  --noerrdialogs \
  --disable-infobars \
  http://localhost:8080
```

Automatic startup, display power management, and offline recovery will be
added once the target Raspberry Pi OS version is confirmed.

## Current implementation

- Responsive 16:9 HTML layout
- CSS perspective grid and OLED-friendly dark palette
- Animated SVG gears, rotor, circuits, and status lights
- Periodic pixel drift to reduce static OLED wear
- Reduced-motion support
- No external web assets or runtime dependencies

## Decisions still needed

1. Confirm the OLED's native resolution and orientation.
2. Confirm the Raspberry Pi model and Raspberry Pi OS version.
3. Decide whether booth photos or individual droid profiles should rotate in.
4. Establish unattended startup and recovery behavior.
5. Decide whether the BUSY Bar becomes a secondary display.
