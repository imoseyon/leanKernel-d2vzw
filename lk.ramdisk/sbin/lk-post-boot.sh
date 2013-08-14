#!/system/bin/sh

# enable supersu support for 4.3
[ -f "/system/xbin/daemonsu" ] && /system/xbin/daemonsu --auto-daemon &
