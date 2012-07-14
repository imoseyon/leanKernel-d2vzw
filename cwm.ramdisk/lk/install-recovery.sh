#!/system/bin/sh
if test -f /data/lk_perm; then
  touch /data/lk_once
  rm /data/lk_counter
  sync
  reboot recovery
fi
