#!/system/bin/sh
mount -o remount,rw /system
busybox cp -a /lk/install-recovery.sh /system/etc
mount -o remount,ro /system
