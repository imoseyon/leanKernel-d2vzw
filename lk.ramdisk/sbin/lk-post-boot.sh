#!/system/bin/sh

# enable supersu support for 4.3
[ -f "/system/xbin/daemonsu" ] && /system/xbin/daemonsu --auto-daemon &

echo "0,0,0,0,0,30" > /sys/module/lowmemorykiller/parameters/minfree
