#!/bin/sh
export XDG_RUNTIME_DIR=/run/user/1000
export WAYLAND_DISPLAY=wayland-0
export DBUS_SESSION_BUS_ADDRESS=unix:path=/run/user/1000/bus
export GDK_BACKEND=wayland
exec /opt/digital-signage/bin/kiosk.sh
