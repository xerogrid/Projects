# Digital Signage Appliance

Animated convention signage for the Fulcrum Builds booth.

## Hardware

- Raspberry Pi 4 Model B
- Raspberry Pi OS 64-bit with desktop autologin
- Portable 14-inch OLED, native **2560×1600** (16:10), landscape
- Optional BUSY Bar on USB at `10.0.4.20`

The current PCB wordmark layout stays. Chromium kiosk fills the native panel.

This project is independent of the BambuLabs VLC and Stream Deck work.

## Sign content

- Fulcrum Builds brand slide (black PCB, green traces, Oxanium wordmark)
- Optional photos of the builds, listed in `assets/photos/manifest.json`
- Away slide when the operator steps away from the booth

## Preview

No build step or package installation is required:

```bash
cd Digital-Signage-Appliance
python3 serve.py --port 8080
```

Open `http://localhost:8080` in a browser.

- Add `?static` to pause animation when taking screenshots.
- Add `?away` to preview the “stepping away” slide.

`python3 -m http.server 8080` still serves the files. Use `serve.py` when you
want the `/api/mode` control endpoint.

## Photos

Keep the brand slide as it is. Photos rotate in after each etch loop, then the
brand slide returns.

1. Copy JPEG or PNG files into `assets/photos/`.
2. List them in `assets/photos/manifest.json`:

```json
{
  "holdMs": 12000,
  "photos": [
    { "src": "pit-droid.jpg", "caption": "Pit Droid" },
    { "src": "battle-droid.jpg", "caption": "Battle Droid" }
  ]
}
```

An empty `photos` array keeps the current PCB-only loop. After you add files,
run `./install.sh` on the Pi so the kiosk picks them up.

## Raspberry Pi kiosk

Unattended boot uses three user systemd units:

| Unit | Role | Recovery |
| --- | --- | --- |
| `signage-http.service` | Serves the site on `127.0.0.1:4173` | Restarts in 2s |
| `signage-kiosk.service` | Chromium kiosk, screen blanking off | Restarts in 5s; `kiosk.sh` also relaunches Chromium |
| `signage-busybar.service` | Booth preset plus Start-button away toggle | Restarts in 15s |

### First install on the Pi

1. Enable desktop autologin for the `signage` user.
2. Copy this directory onto the Pi.
3. From the directory, run:

```bash
chmod +x install.sh
./install.sh
```

`install.sh` copies files to `/opt/digital-signage`, provisions recoverable
SSH-over-USB access, enables linger, and starts the core HTTP and kiosk units.
It installs and starts the optional BUSY Bar integration when its Python
dependency is available; an offline package index does not block the display.
See [Field SSH access](docs/field-access.md) for the 1Password keys, device
inventory, and clean-laptop recovery procedure.

Confirm:

```bash
systemctl --user status signage-http.service signage-kiosk.service signage-busybar.service
curl --fail --silent http://127.0.0.1:4173/ | grep -F 'Fulcrum Builds'
```

A reboot must bring the sign back without a keyboard. If Chromium is missing,
the kiosk unit stays in restart until you install it.

### Display power

`kiosk.sh` turns off screen blanking (`xset`, `swayidle`) before it opens
Chromium. The OLED stays on for the booth.

## Current implementation

- Responsive 16:10 HTML layout matching the 2560×1600 panel
- OLED-friendly black PCB background with charcoal silkscreen markings
- Top-centered mechanical wordmark using a self-hosted Oxanium font
- Matte green circuit traces and terminal pads distributed around the frame
- Looping edge-to-pad trace etch that holds the current still, then retracts
- Optional photo rotation with the wordmark kept on top
- Periodic pixel drift to reduce static OLED wear
- Reduced-motion support
- No external web assets or signage runtime dependencies

Oxanium is distributed under the SIL Open Font License 1.1; its license is
included at `assets/fonts/OFL.txt`.

## Remote BUSY Bar control

`busybar_control.py` controls a BUSY Bar linked over USB (`BUSYBAR_ADDRESS`)
or the BUSY cloud service. The API token is read only from
`BUSYBAR_API_TOKEN`; it is never accepted as a command-line argument.

The bar stays in the booth. The large top **Start/Pause** button toggles
operator-away mode on both the bar and the OLED.

### Device and token setup

1. Connect the BUSY Bar to Wi-Fi and link it to a BUSY account.
2. At [cloud.busy.app](https://cloud.busy.app), create an API token with the
   **BUSY Bar** scope. The token controls the linked device and is shown once.
3. Add it to the execution environment as a secret named
   `BUSYBAR_API_TOKEN`. Never commit it to this repository.

On the Pi, USB is enough:

```bash
export BUSYBAR_ADDRESS=10.0.4.20
```

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

# Operator away (also switches the OLED when serve.py is running)
.venv/bin/python busybar_control.py away
.venv/bin/python busybar_control.py away "BACK IN 5"

# Listen for the top Start button and toggle booth/away
.venv/bin/python busybar_control.py watch

# Agent activity (outranks the booth preset)
.venv/bin/python busybar_control.py agent "MAKING CHANGES"

# Idle / waiting (picks a stock line, or pass one explicitly)
.venv/bin/python busybar_control.py sleeping
.venv/bin/python busybar_control.py sleeping "Waiting for input"

# Other controls
.venv/bin/python busybar_control.py message "PHOTO OPS WELCOME"
.venv/bin/python busybar_control.py brightness 75
.venv/bin/python busybar_control.py clear
```

Use `--dry-run` before a mutating command to inspect its payload without a
token or network request:

```bash
.venv/bin/python busybar_control.py --dry-run signage
.venv/bin/python busybar_control.py --dry-run away
```

Press the top Start/Pause button once to show **AWAY / BACK SOON** on the bar
and the OLED. Press it again to restore the booth preset.

## Tests

```bash
python3 -m unittest discover -s tests
```
