#!/bin/sh
# Configure the appliance's stable SSH identity and authorized field keys.

set -eu

TARGET_USER="${SIGNAGE_USER:-signage}"
TARGET_HOSTNAME="${SIGNAGE_HOSTNAME:-xerogrid-signage}"
USB_SERVICE="xerogrid-usb-gadget-network.service"

if [ "$(id -u)" -ne 0 ]; then
  echo "Run this script as root (for example: sudo $0)." >&2
  exit 1
fi

if ! getent passwd "$TARGET_USER" >/dev/null 2>&1; then
  echo "User '$TARGET_USER' does not exist." >&2
  exit 1
fi

TARGET_HOME="$(getent passwd "$TARGET_USER" | cut -d: -f6)"
TARGET_GROUP="$(id -gn "$TARGET_USER")"
SSH_DIR="$TARGET_HOME/.ssh"
AUTHORIZED_KEYS="$SSH_DIR/authorized_keys"
KEYS_TMP="$(mktemp)"
KEYS_DEDUPED="$(mktemp)"
trap 'rm -f "$KEYS_TMP" "$KEYS_DEDUPED"' EXIT HUP INT TERM

install -d -o "$TARGET_USER" -g "$TARGET_GROUP" -m 0700 "$SSH_DIR"
if [ -f "$AUTHORIZED_KEYS" ]; then
  cat "$AUTHORIZED_KEYS" >"$KEYS_TMP"
fi

# Private key item: "Mac SSH — brendan@Mac.lan" in the 1Password Private vault.
printf '%s\n' \
  'ssh-ed25519 AAAAC3NzaC1lZDI1NTE5AAAAIBH5cdUPI3q0wDiCPE0REPdukBpURNOKDJZr/s+laCKZ brendan@Mac.lan' \
  >>"$KEYS_TMP"

# Private key item: "Digital Signage Appliance — Backup SSH Key" in 1Password.
printf '%s\n' \
  'ssh-ed25519 AAAAC3NzaC1lZDI1NTE5AAAAICpd3+g/yKKBK0ZJ9URaa27R8X64QyXhlkv64nPVKftp Digital Signage Appliance backup' \
  >>"$KEYS_TMP"

awk 'NF { identity = $1 " " $2; if (!seen[identity]++) print }' \
  "$KEYS_TMP" >"$KEYS_DEDUPED"
install -o "$TARGET_USER" -g "$TARGET_GROUP" -m 0600 \
  "$KEYS_DEDUPED" "$AUTHORIZED_KEYS"

if command -v hostnamectl >/dev/null 2>&1; then
  hostnamectl set-hostname "$TARGET_HOSTNAME"
fi

systemctl enable --now ssh.service
if systemctl list-unit-files avahi-daemon.service --no-legend 2>/dev/null \
  | grep -q '^avahi-daemon.service'; then
  systemctl enable --now avahi-daemon.service
fi

if systemctl cat "$USB_SERVICE" >/dev/null 2>&1; then
  systemctl enable --now "$USB_SERVICE"
else
  echo "WARNING: $USB_SERVICE is not installed; USB networking was left unchanged." >&2
fi

echo "Field SSH access provisioned for $TARGET_USER@$TARGET_HOSTNAME.local."
echo "Authorized key fingerprints:"
ssh-keygen -lf "$AUTHORIZED_KEYS"
echo "USB field address: 192.168.7.2 (managed by $USB_SERVICE when installed)."
