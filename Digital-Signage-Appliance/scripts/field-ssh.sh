#!/bin/sh
# Connect without depending on a private-key file from the original dev Mac.

set -u

TARGET_USER="${SIGNAGE_USER:-signage}"
PRIMARY_HOST="${SIGNAGE_HOST:-xerogrid-signage.local}"
FALLBACK_HOST="${SIGNAGE_USB_IP:-10.12.194.1}"

for host in "$PRIMARY_HOST" "$FALLBACK_HOST"; do
  echo "Trying $TARGET_USER@$host..." >&2
  ssh \
    -o ConnectionAttempts=1 \
    -o ConnectTimeout=5 \
    -o StrictHostKeyChecking=accept-new \
    "$TARGET_USER@$host" "$@"
  status=$?
  if [ "$status" -ne 255 ]; then
    exit "$status"
  fi
done

echo "The Pi was not reachable by hostname or USB fallback address." >&2
exit 255
