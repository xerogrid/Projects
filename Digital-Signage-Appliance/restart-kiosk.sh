#!/bin/sh
set -eu
sudo install -m 0755 /tmp/kiosk.sh /opt/digital-signage/bin/kiosk.sh
sudo install -m 0755 /tmp/start-kiosk.sh /opt/digital-signage/bin/start-kiosk.sh
if command -v systemctl >/dev/null 2>&1 && systemctl --user is-enabled signage-kiosk.service >/dev/null 2>&1; then
  systemctl --user restart signage-http.service || true
  systemctl --user restart signage-kiosk.service
else
  ps -eo pid=,cmd= | awk '/\/opt\/digital-signage\/bin\/kiosk.sh/ && !/awk/ { print $1 }' | xargs -r kill
  killall chromium 2>/dev/null || true
  sleep 1
  nohup /opt/digital-signage/bin/start-kiosk.sh >/tmp/kiosk.log 2>&1 &
fi
sleep 5
echo KIOSK
ps -eo pid,etime,cmd | awk '/\/opt\/digital-signage\/bin\/kiosk.sh/ && !/awk/'
echo CHROMIUM
ps -eo pid,etime,cmd | awk '/\/usr\/lib\/chromium\/chromium / && !/--type=/ && !/awk/'
echo LOG
if command -v systemctl >/dev/null 2>&1; then
  systemctl --user --no-pager --full status signage-kiosk.service || true
else
  cat /tmp/kiosk.log
fi
echo PAGE
curl --max-time 3 --fail --silent http://127.0.0.1:4173/ | grep -F 'Fulcrum Builds' | head -n 3
