#!/usr/bin/env bash
# Launch the Fulcrum Builds signage in Chromium kiosk mode.
set -euo pipefail

ROOT="$(cd "$(dirname "$0")" && pwd)"
PORT="${SIGNAGE_PORT:-8080}"
URL="http://127.0.0.1:${PORT}/"
PROFILE="${SIGNAGE_PROFILE:-$ROOT/.chromium-kiosk}"

chromium_bin() {
  if command -v chromium >/dev/null 2>&1; then
    command -v chromium
  elif command -v chromium-browser >/dev/null 2>&1; then
    command -v chromium-browser
  else
    echo "Chromium is not installed. On Raspberry Pi OS run: sudo apt install chromium" >&2
    exit 1
  fi
}

server_up() {
  python3 - "$PORT" <<'PY'
import sys, urllib.request
port = sys.argv[1]
try:
    urllib.request.urlopen(f"http://127.0.0.1:{port}/", timeout=1)
except Exception:
    raise SystemExit(1)
PY
}

if ! server_up; then
  python3 -m http.server "$PORT" --directory "$ROOT" --bind 127.0.0.1 >/tmp/fulcrum-signage-http.log 2>&1 &
  for _ in $(seq 1 30); do
    if server_up; then
      break
    fi
    sleep 0.2
  done
  if ! server_up; then
    echo "Signage server failed to start on port ${PORT}. See /tmp/fulcrum-signage-http.log" >&2
    exit 1
  fi
fi

mkdir -p "$PROFILE"

exec "$(chromium_bin)" \
  --kiosk \
  --password-store=basic \
  --noerrdialogs \
  --disable-infobars \
  --disable-session-crashed-bubble \
  --disable-restore-session-state \
  --disable-features=Translate,TranslateUI \
  --check-for-update-interval=31536000 \
  --user-data-dir="$PROFILE" \
  --app="$URL"
