#!/bin/sh
# Emit the network facts needed for the field access record.

set -eu

echo "hostname=$(hostname)"
echo "fqdn=$(hostname -f 2>/dev/null || hostname)"

for interface in usb0 wlan0 eth0; do
  if [ ! -d "/sys/class/net/$interface" ]; then
    continue
  fi
  mac="$(cat "/sys/class/net/$interface/address")"
  state="$(cat "/sys/class/net/$interface/operstate")"
  ipv4="$(ip -4 -o address show dev "$interface" 2>/dev/null | awk '{print $4}' | paste -sd, -)"
  echo "$interface.mac=$mac"
  echo "$interface.state=$state"
  echo "$interface.ipv4=${ipv4:-none}"
done
