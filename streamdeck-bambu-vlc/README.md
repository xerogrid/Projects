# BambuLabs VLC Quadrant Streams – Stream Deck Plugin

Press one Stream Deck button to instantly open every active BambuLabs 3D printer RTSP stream in its own VLC window, tiled automatically in quadrant layout on your Windows 11 display.

---

## How it works

1. The button press triggers the Python script `bambu_vlc_quadrant.py`.
2. The script scans **all running processes** (ffmpeg, OrcaSlicer, Bambu Studio, etc.) for BambuLab RTSP stream URLs matching the pattern:
   ```
   rtsps://bblp:<access_code>@<printer_ip>/streaming/live/1
   ```
3. Each stream URL found is opened in its own VLC window.
4. VLC windows are automatically positioned in quadrants on your primary screen:
   | Count | Layout |
   |-------|--------|
   | 1     | Full screen |
   | 2     | Side by side |
   | 3     | Top full-width + 2 bottom halves |
   | 4     | 2 × 2 grid |
   | 5+    | 2-column grid |

---

## Requirements

| Requirement | Notes |
|---|---|
| Windows 11 | Win32 APIs used for window positioning |
| Python 3.10+ | `py` or `python` must be in PATH |
| VLC Media Player | Default: `C:\Program Files\VideoLAN\VLC\vlc.exe` |
| Elgato Stream Deck software 6.4+ | For the full plugin install |
| Active printer streams | At least one process must already be pulling an RTSP feed from a printer |

---

## Quick Start (no Stream Deck needed)

1. Install Python dependencies:
   ```
   pip install psutil pywin32
   ```
2. Double-click `scripts\launch.bat`  
   — or run `python scripts\bambu_vlc_quadrant.py` directly.

---

## Stream Deck Plugin Install

### Method 1 – Double-click install (recommended)
1. Make sure Elgato Stream Deck software 6.4+ is running.
2. Double-click `com.bambu.vlcstreams.sdPlugin` in Explorer.  
   Stream Deck will prompt to install it automatically.

### Method 2 – Manual copy
Copy `com.bambu.vlcstreams.sdPlugin` to:
```
%APPDATA%\Elgato\StreamDeck\Plugins\
```
Then restart Stream Deck software.

### Method 3 – npm (for development)
```
cd com.bambu.vlcstreams.sdPlugin
npm install
```

### Add the button
1. Open Stream Deck software.
2. Find **BambuLabs VLC Streams** in the action list (category: *3D Printing*).
3. Drag **Open Printer Streams** onto any key.
4. Optionally open the Property Inspector to set a custom VLC path or tweak timing.

---

## Configuration

### Via Property Inspector (recommended)
Open the button's settings in Stream Deck software.

### Via Environment Variables (script-level)
| Variable | Default | Description |
|---|---|---|
| `BAMBU_VLC_PATH` | Auto-detect | Full path to `vlc.exe` |

### Edit the script directly
Top of `bambu_vlc_quadrant.py`:

```python
VLC_SEARCH_PATHS = [...]   # Locations to search for VLC
LAUNCH_DELAY     = 0.6     # Seconds between launching each window
REPOSITION_DELAY = 2.5     # Seconds to wait before repositioning windows
TASKBAR_HEIGHT   = 48      # Subtract from screen height (taskbar)
```

---

## Troubleshooting

**"No BambuLabs streams found"**  
The script only detects streams that are *already running* in another process. Open Bambu Studio / OrcaSlicer and start the camera view, or start an ffmpeg process pulling from the printer before pressing the button.

**Windows appear in wrong position**  
Increase `REPOSITION_DELAY` (default 2.5 s) if VLC hasn't finished loading when repositioning runs.

**VLC not found**  
Set `BAMBU_VLC_PATH` environment variable or edit `VLC_SEARCH_PATHS` in the script.

**Python not found**  
Install Python 3 from https://www.python.org/ and ensure `py` or `python` is in your system PATH.

---

## File structure

```
streamdeck-bambu-vlc/
├── com.bambu.vlcstreams.sdPlugin/   # Stream Deck plugin bundle
│   ├── manifest.json                # Plugin metadata & action definitions
│   ├── package.json                 # Node.js dependencies
│   ├── bin/
│   │   └── plugin.js                # Stream Deck key-press handler (ESM)
│   ├── scripts/
│   │   └── bambu_vlc_quadrant.py    # Core Python launcher (bundled copy)
│   ├── imgs/                        # SVG icons for Stream Deck UI
│   └── property_inspector/
│       └── index.html               # Settings panel in Stream Deck software
└── scripts/                         # Standalone scripts (use without Stream Deck)
    ├── bambu_vlc_quadrant.py        # Main Python script
    ├── requirements.txt             # pip dependencies
    └── launch.bat                   # Double-click launcher / Stream Deck target
```
