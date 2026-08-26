#!/bin/sh

set -u

SIGNAGE_URL="${SIGNAGE_URL:-http://127.0.0.1:4173}"
USER_DATA_DIR="${KIOSK_USER_DATA_DIR:-$HOME/.config/chromium-signage}"

pkill swayidle 2>/dev/null || true
xset s off 2>/dev/null || true
xset -dpms 2>/dev/null || true
xset s noblank 2>/dev/null || true

if command -v chromium >/dev/null 2>&1; then
  browser="chromium"
elif command -v chromium-browser >/dev/null 2>&1; then
  browser="chromium-browser"
else
  exit 1
fi

mkdir -p "$USER_DATA_DIR"

while true; do
  "$browser" \
    --user-data-dir="$USER_DATA_DIR" \
    --kiosk \
    --no-first-run \
    --no-default-browser-check \
    --noerrdialogs \
    --disable-infobars \
    --disable-session-crashed-bubble \
    --disable-translate \
    --disable-features=Translate,InfiniteSessionRestore \
    --check-for-update-interval=31536000 \
    --disable-component-update \
    --disable-pinch \
    --autoplay-policy=no-user-gesture-required \
    --overscroll-history-navigation=0 \
    --ozone-platform=wayland \
    "$SIGNAGE_URL"
  sleep 5
done
