#!/sbin/sh
mount -o remount,rw /system
/sbin/cp -a /lk/install-recovery.sh /system/etc
mount -o remount,ro /system
