#!/bin/sh
# Install the appliance onto this Raspberry Pi and enable unattended kiosk boot.

set -eu

ROOT="$(CDPATH= cd -- "$(dirname "$0")" && pwd)"
PREFIX="${SIGNAGE_PREFIX:-/opt/digital-signage}"
UNIT_DIR="${XDG_CONFIG_HOME:-$HOME/.config}/systemd/user"

echo "Installing signage into $PREFIX"

if [ "${SIGNAGE_SKIP_FIELD_ACCESS:-0}" != "1" ]; then
  sudo "$ROOT/scripts/provision-field-access.sh"
fi

sudo mkdir -p "$PREFIX/bin" "$PREFIX/www" "$PREFIX/state"
sudo chown "$(id -u):$(id -g)" "$PREFIX/state"

sudo install -m 0755 "$ROOT/kiosk.sh" "$PREFIX/bin/kiosk.sh"
sudo install -m 0755 "$ROOT/start-kiosk.sh" "$PREFIX/bin/start-kiosk.sh"
sudo install -m 0755 "$ROOT/restart-kiosk.sh" "$PREFIX/bin/restart-kiosk.sh"
sudo install -m 0755 "$ROOT/serve.py" "$PREFIX/bin/serve.py"
sudo install -m 0755 "$ROOT/busybar_control.py" "$PREFIX/bin/busybar_control.py"
sudo install -m 0644 "$ROOT/requirements.txt" "$PREFIX/bin/requirements.txt"

# Copy the static site without Python tooling or packaging files.
sudo rm -rf "$PREFIX/www"
sudo mkdir -p "$PREFIX/www/assets/fonts" "$PREFIX/www/assets/photos"
sudo install -m 0644 "$ROOT/index.html" "$ROOT/app.js" "$ROOT/minimal.css" "$PREFIX/www/"
sudo install -m 0644 "$ROOT/assets/fbuilds.svg" "$PREFIX/www/assets/fbuilds.svg"
sudo install -m 0644 "$ROOT/assets/fonts/"* "$PREFIX/www/assets/fonts/"
if [ -e "$ROOT/assets/photos/manifest.json" ]; then
  sudo install -m 0644 "$ROOT/assets/photos/manifest.json" "$PREFIX/www/assets/photos/manifest.json"
fi
if [ -d "$ROOT/assets/photos" ]; then
  for photo in "$ROOT/assets/photos/"*; do
    [ -f "$photo" ] || continue
    sudo install -m 0644 "$photo" "$PREFIX/www/assets/photos/"
  done
fi

mkdir -p "$UNIT_DIR"
install -m 0644 "$ROOT/packaging/signage-http.service" "$UNIT_DIR/signage-http.service"
install -m 0644 "$ROOT/packaging/signage-kiosk.service" "$UNIT_DIR/signage-kiosk.service"
install -m 0644 "$ROOT/packaging/signage-busybar.service" "$UNIT_DIR/signage-busybar.service"

if command -v loginctl >/dev/null 2>&1; then
  sudo loginctl enable-linger "$(id -un)" || true
fi

systemctl --user daemon-reload
systemctl --user enable signage-http.service signage-kiosk.service
systemctl --user restart signage-http.service
systemctl --user restart signage-kiosk.service || true

busybar_ready=0
if [ "${SIGNAGE_SKIP_BUSYBAR:-0}" = "1" ]; then
  echo "Skipping optional BUSY Bar dependency installation."
else
  if [ ! -x "$PREFIX/venv/bin/python" ]; then
    sudo python3 -m venv "$PREFIX/venv"
  fi

  if sudo "$PREFIX/venv/bin/python" -c 'import busylib' 2>/dev/null || \
    sudo "$PREFIX/venv/bin/python" -m pip install \
      --disable-pip-version-check \
      --retries 1 \
      --timeout 5 \
      -r "$PREFIX/bin/requirements.txt"; then
    busybar_ready=1
  else
    echo "WARNING: BUSY Bar dependencies are unavailable; core signage remains enabled." >&2
  fi
fi

if [ "$busybar_ready" = "1" ]; then
  systemctl --user enable signage-busybar.service
  systemctl --user restart signage-busybar.service
else
  systemctl --user disable --now signage-busybar.service 2>/dev/null || true
fi

echo "Installed."
echo "HTTP:   systemctl --user status signage-http.service"
echo "Kiosk:  systemctl --user status signage-kiosk.service"
if [ "$busybar_ready" = "1" ]; then
  echo "Bar:    systemctl --user status signage-busybar.service"
else
  echo "Bar:    optional dependency unavailable; rerun install.sh when online"
fi
