#!/system/xbin/busybox ash

# enable su support for 4.3
if [[ `grep -c release=4.3 /system/build.prop` -ge 1 ]]; then
  [ -f "/system/xbin/su" ] && /system/xbin/su --daemon &
  [ -f "/system/xbin/daemonsu" ] && /system/xbin/daemonsu --auto-daemon &
fi
