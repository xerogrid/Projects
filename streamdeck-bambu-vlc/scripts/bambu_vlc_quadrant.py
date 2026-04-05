"""
BambuLabs VLC Quadrant Viewer
-------------------------------
Scans all running processes for active BambuLabs 3D printer RTSP streams
(from ffmpeg or any other process), then opens each stream in a VLC window
arranged in quadrant layout on the primary Windows 11 display.

Dependencies:  pip install psutil pywin32
"""

import os
import re
import sys
import time
import subprocess
import ctypes
from ctypes import wintypes

try:
    import psutil
except ImportError:
    print("ERROR: psutil not installed. Run:  pip install psutil")
    sys.exit(1)

# ---------------------------------------------------------------------------
# Configuration – edit these to match your setup
# ---------------------------------------------------------------------------

# Common VLC install locations (searched in order)
VLC_SEARCH_PATHS = [
    r"C:\Program Files\VideoLAN\VLC\vlc.exe",
    r"C:\Program Files (x86)\VideoLAN\VLC\vlc.exe",
]

# Override VLC path via environment variable BAMBU_VLC_PATH
VLC_PATH = os.environ.get("BAMBU_VLC_PATH", "")

# How long (seconds) to wait between launching each VLC window
LAUNCH_DELAY = 0.6

# How long (seconds) to wait after launching before repositioning windows
REPOSITION_DELAY = 2.5

# Taskbar height estimate used when positioning windows (pixels)
TASKBAR_HEIGHT = 48

# ---------------------------------------------------------------------------
# RTSP URL patterns used by BambuLab printers
# rtsps://bblp:<access_code>@<ip>[:port]/streaming/live/<n>
# Also matches plain rtsp:// in case of stripped TLS
# ---------------------------------------------------------------------------
BAMBU_STREAM_RE = re.compile(
    r"rtsps?://(?:bblp:[^@\s]+@)?(?:\d{1,3}\.){3}\d{1,3}(?::\d+)?/streaming/live/\d+",
    re.IGNORECASE,
)


# ---------------------------------------------------------------------------
# Win32 helpers (no pywin32 required – pure ctypes)
# ---------------------------------------------------------------------------

user32 = ctypes.windll.user32
user32.SetProcessDPIAware()  # Avoid DPI scaling issues on Win 11


def get_primary_screen() -> tuple[int, int]:
    """Return (width, height) of the primary monitor in physical pixels."""
    w = user32.GetSystemMetrics(0)   # SM_CXSCREEN
    h = user32.GetSystemMetrics(1)   # SM_CYSCREEN
    return w, h


def find_vlc_exe() -> str:
    if VLC_PATH and os.path.isfile(VLC_PATH):
        return VLC_PATH
    for p in VLC_SEARCH_PATHS:
        if os.path.isfile(p):
            return p
    return ""


# EnumWindows callback type
WNDENUMPROC = ctypes.WINFUNCTYPE(ctypes.c_bool, wintypes.HWND, wintypes.LPARAM)


def _enum_windows_cb(hwnd, lParam):
    """Collect all visible top-level windows into a list passed via lParam."""
    if user32.IsWindowVisible(hwnd):
        pid = wintypes.DWORD()
        user32.GetWindowThreadProcessId(hwnd, ctypes.byref(pid))
        lst = ctypes.cast(lParam, ctypes.py_object).value
        lst.append((hwnd, pid.value))
    return True


def get_windows_for_pids(pids: set) -> list:
    """Return list of (hwnd, pid) for all visible windows belonging to pids."""
    all_wins = []
    cb = WNDENUMPROC(_enum_windows_cb)
    ctypes.windll.user32.EnumWindows(cb, ctypes.cast(ctypes.py_object(all_wins), wintypes.LPARAM))
    return [(h, p) for h, p in all_wins if p in pids]


def set_window_pos(hwnd, x: int, y: int, w: int, h: int):
    """Move and resize a window. SWP flags: no-zorder, no-activate."""
    SWP_NOZORDER = 0x0004
    SWP_NOACTIVATE = 0x0010
    user32.SetWindowPos(hwnd, 0, x, y, w, h, SWP_NOZORDER | SWP_NOACTIVATE)


def remove_window_decorations(hwnd):
    """Strip title bar / borders for a cleaner quadrant look."""
    GWL_STYLE = -16
    WS_OVERLAPPEDWINDOW = 0x00CF0000
    WS_VISIBLE = 0x10000000
    style = user32.GetWindowLongW(hwnd, GWL_STYLE)
    style = (style & ~WS_OVERLAPPEDWINDOW) | WS_VISIBLE
    user32.SetWindowLongW(hwnd, GWL_STYLE, style)


# ---------------------------------------------------------------------------
# Stream detection
# ---------------------------------------------------------------------------

def find_bambu_streams() -> list[str]:
    """
    Scan every running process's command line for BambuLabs RTSP URLs.
    Returns a deduplicated, ordered list of stream URLs found.
    """
    seen: set[str] = set()
    streams: list[str] = []

    for proc in psutil.process_iter(["pid", "name", "cmdline"]):
        try:
            cmdline = proc.info.get("cmdline") or []
            if not cmdline:
                continue
            full_cmd = " ".join(str(a) for a in cmdline)
            for match in BAMBU_STREAM_RE.findall(full_cmd):
                url = match.strip()
                if url not in seen:
                    seen.add(url)
                    streams.append(url)
        except (psutil.NoSuchProcess, psutil.AccessDenied, psutil.ZombieProcess):
            pass

    return streams


# ---------------------------------------------------------------------------
# Quadrant layout calculator
# ---------------------------------------------------------------------------

def compute_layout(screen_w: int, screen_h: int, count: int) -> list[tuple[int, int, int, int]]:
    """
    Return a list of (x, y, width, height) rects that tile `count` windows
    across the usable screen area (minus taskbar at bottom).
    Supports 1–4 streams; >4 streams fall back to a 2-column grid.
    """
    usable_h = screen_h - TASKBAR_HEIGHT
    hw = screen_w // 2
    hh = usable_h // 2

    if count == 1:
        return [(0, 0, screen_w, usable_h)]

    if count == 2:
        # Side by side
        return [(0, 0, hw, usable_h), (hw, 0, hw, usable_h)]

    if count == 3:
        # Top full-width + two bottom halves
        return [
            (0,  0,  screen_w, hh),
            (0,  hh, hw,       hh),
            (hw, hh, hw,       hh),
        ]

    if count == 4:
        return [
            (0,  0,  hw, hh),
            (hw, 0,  hw, hh),
            (0,  hh, hw, hh),
            (hw, hh, hw, hh),
        ]

    # >4: two-column grid
    rows = (count + 1) // 2
    cell_h = usable_h // rows
    rects = []
    for i in range(count):
        row, col = divmod(i, 2)
        cols_in_row = 2 if (i + 1 < count or count % 2 == 0) else 1
        cell_w = screen_w // cols_in_row
        rects.append((col * cell_w, row * cell_h, cell_w, cell_h))
    return rects


# ---------------------------------------------------------------------------
# VLC launcher
# ---------------------------------------------------------------------------

def launch_vlc(vlc_exe: str, stream_url: str, index: int) -> subprocess.Popen:
    """
    Launch a VLC instance for one stream.
    Window geometry is set by VLC itself via --video-x/y/width/height;
    we also reposition via Win32 after launch for reliability.
    """
    cmd = [
        vlc_exe,
        stream_url,
        "--no-video-title-show",
        "--rtsp-tcp",                  # Prefer TCP for BambuLab RTSPS
        "--no-embedded-video",         # Separate window per instance
        "--no-qt-fs-controller",       # Hide fullscreen control bar
        "--no-qt-privacy-ask",         # Skip first-run dialog
        "--qt-minimal-view",           # Hide toolbar/menu in view
        f"--meta-title=Printer {index + 1}",
    ]
    return subprocess.Popen(cmd, stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)


# ---------------------------------------------------------------------------
# Main
# ---------------------------------------------------------------------------

def main():
    print("=== BambuLabs VLC Quadrant Viewer ===")
    print()

    # 1. Find VLC
    vlc_exe = find_vlc_exe()
    if not vlc_exe:
        print("ERROR: VLC not found.\n"
              "Install VLC from https://www.videolan.org/ or set BAMBU_VLC_PATH.")
        input("\nPress Enter to exit...")
        sys.exit(1)
    print(f"VLC : {vlc_exe}")

    # 2. Detect streams
    print("Scanning processes for BambuLabs RTSP streams...")
    streams = find_bambu_streams()

    if not streams:
        print("\nNo BambuLabs streams found in any running process.\n"
              "Make sure at least one ffmpeg (or other) process is actively\n"
              "pulling an RTSP stream from your printer(s).")
        input("\nPress Enter to exit...")
        sys.exit(0)

    print(f"\nFound {len(streams)} stream(s):")
    for i, s in enumerate(streams):
        print(f"  [{i + 1}] {s}")
    print()

    # 3. Screen layout
    sw, sh = get_primary_screen()
    rects = compute_layout(sw, sh, len(streams))
    print(f"Screen : {sw}x{sh}  |  Layout : {len(streams)}-up quadrant")
    print()

    # 4. Launch VLC instances
    procs: list[subprocess.Popen] = []
    pids: set[int] = set()

    for i, stream in enumerate(streams):
        print(f"  Launching stream {i + 1}/{len(streams)} …")
        proc = launch_vlc(vlc_exe, stream, i)
        procs.append(proc)
        pids.add(proc.pid)
        time.sleep(LAUNCH_DELAY)

    # 5. Wait for windows to appear, then reposition
    print(f"\nWaiting {REPOSITION_DELAY}s for VLC windows to open …")
    time.sleep(REPOSITION_DELAY)

    wins = get_windows_for_pids(pids)
    if not wins:
        print("WARNING: Could not locate VLC windows for repositioning.\n"
              "VLC may still open correctly via its own --video-x/y flags.")
    else:
        positioned = 0
        for (hwnd, pid), (x, y, w, h) in zip(wins, rects):
            try:
                remove_window_decorations(hwnd)
                set_window_pos(hwnd, x, y, w, h)
                positioned += 1
            except Exception as e:
                print(f"  WARNING: Could not reposition hwnd {hwnd}: {e}")
        print(f"Positioned {positioned}/{len(streams)} window(s).")

    print("\nAll streams open. Close this window at any time.")
    print("Press Ctrl+C or close this console to leave streams running.")

    try:
        for proc in procs:
            proc.wait()
    except KeyboardInterrupt:
        pass


if __name__ == "__main__":
    main()
