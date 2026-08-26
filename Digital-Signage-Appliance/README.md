# Digital Signage Appliance

Animated convention signage for the Fulcrum Builds booth. The first display
target is a Raspberry Pi connected to a portable 14-inch OLED screen.

This project is independent of the BambuLabs VLC and Stream Deck work.

## Sign content

- Fulcrum Builds

The current design is intentionally stripped back to a mechanically styled,
top-centered company name and matte green circuit traces on a black silkscreen
PCB background.

## Preview

No build step or package installation is required:

```bash
cd Digital-Signage-Appliance
python3 -m http.server 8080
```

Open `http://localhost:8080` in a browser. Add `?static` to pause continuous
animation when taking screenshots.

## Raspberry Pi kiosk

The HDMI display stays white until Chromium actually loads this sign. Do not
open a regular browser window. From the Pi, run:

```bash
cd Digital-Signage-Appliance
chmod +x kiosk.sh
./kiosk.sh
```

That script starts a local server if needed and launches Chromium in kiosk
mode with `--password-store=basic`, which skips the white keyring password
dialog. If that dialog is already on screen, leave both fields blank, click
**Continue**, then confirm **Use Unsafe Storage** if asked.

To start it at login, copy the desktop entry:

```bash
mkdir -p ~/.config/autostart
cp kiosk.desktop ~/.config/autostart/
```

Edit `kiosk.desktop` so `Exec=` points at the absolute path of `kiosk.sh` on
the Pi.

If you previously launched Chromium by hand, close every Chromium window
first, then use `./kiosk.sh`. A leftover blank Chromium tab is solid white.

## Current implementation

- Responsive 16:9 HTML layout
- OLED-friendly black PCB background with charcoal silkscreen markings
- Top-centered mechanical wordmark using a self-hosted Oxanium font
- Matte green circuit traces and terminal pads distributed around the frame
- Periodic pixel drift to reduce static OLED wear
- Reduced-motion support
- No external web assets or signage runtime dependencies

Oxanium is distributed under the SIL Open Font License 1.1; its license is
included at `assets/fonts/OFL.txt`.

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
