# Digital Signage Appliance

Animated convention signage for the Fulcrum Builds booth. The first display
target is a Raspberry Pi connected to a portable 14-inch OLED screen.

This project is independent of the BambuLabs VLC and Stream Deck work.

## Sign content

- Fulcrum Builds

The current design is intentionally stripped back to a mechanically styled,
top-centered company name and matte green circuit traces on a pure black
background.

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
- Pure black OLED-friendly background
- Top-centered mechanical wordmark
- Matte green circuit traces and terminal pads distributed around the frame
- Periodic pixel drift to reduce static OLED wear
- Reduced-motion support
- No external web assets or signage runtime dependencies

## Remote BUSY Bar control

`busybar_control.py` controls a BUSY Bar linked to the BUSY cloud service.
The API token is read only from `BUSYBAR_API_TOKEN`; it is never accepted as a
command-line argument.

### Device and token setup

1. Connect the BUSY Bar to Wi-Fi and link it to a BUSY account.
2. At [cloud.busy.app](https://cloud.busy.app), create an API token with the
   **BUSY Bar** scope. The token controls the linked device and is shown once.
3. Add it to the execution environment as a secret named
   `BUSYBAR_API_TOKEN`. Never commit it to this repository.

Cloud environment secrets are applied to newly started agents. For a local
terminal, enter the token without putting its value in shell history:

```bash
read -rsp "BUSY Bar token: " BUSYBAR_API_TOKEN && echo
export BUSYBAR_API_TOKEN
```

### Install and use

```bash
python3 -m venv .venv
.venv/bin/python -m pip install -r requirements.txt

# Read-only connectivity check
.venv/bin/python busybar_control.py status

# Fulcrum Builds preset on both displays
.venv/bin/python busybar_control.py signage

# Other controls
.venv/bin/python busybar_control.py message "PHOTO OPS WELCOME"
.venv/bin/python busybar_control.py brightness 75
.venv/bin/python busybar_control.py clear
```

Use `--dry-run` before a mutating command to inspect its payload without a
token or network request:

```bash
.venv/bin/python busybar_control.py --dry-run signage
```

## Decisions still needed

1. Confirm the OLED's native resolution and orientation.
2. Confirm the Raspberry Pi model and Raspberry Pi OS version.
3. Decide whether booth photos or individual droid profiles should rotate in.
4. Establish unattended startup and recovery behavior.
5. Decide whether the BUSY Bar remains part of the final convention setup.
