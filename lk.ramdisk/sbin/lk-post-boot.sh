#!/system/bin/sh

# enable supersu support for 4.3
[ -f "/system/xbin/daemonsu" ] && /system/xbin/daemonsu --auto-daemon &

#
# replace (u)random with erandom
#
rm -f /dev/urandom
ln -s /dev/erandom /dev/urandom
rm -f /dev/random
ln -s /dev/erandom /dev/random

[ ! -d "/data/data/leankernel" ] && mkdir /data/data/leankernel
chmod 755 /data/data/leankernel
#
# panel temperature setting
#
PANELTMP="/data/data/leankernel/paneltemp"
PANELSF="/sys/class/lcd/panel/temperature"
[ -f $PANELTMP ] && echo `cat $PANELTMP` > $PANELSF
#
# panel color control
#
PANELCLR="/data/data/leankernel/panelcolor"
PANELSF="/sys/class/lcd/panel/panel_colors"
[ -f $PANELCLR ] && echo `cat $PANELCLR` > $PANELSF
#
# screen_off_maxfreq
#
CFILE="/data/data/leankernel/screen_off_maxfreq"
SFILE="/sys/devices/system/cpu/cpufreq"
if [ -f $CFILE ]; then
  echo `cat $CFILE` > $SFILE/ondemand/screen_off_maxfreq 
  echo `cat $CFILE` > $SFILE/interactive/screen_off_maxfreq
fi
#
# brightness tweak
#
CFILE="/data/data/leankernel/btweak"
SFILE="/sys/devices/virtual/graphics/fb0/btweak"
[ -f $CFILE ] && echo `cat $CFILE` > $SFILE
#
# gpu governor
#
CFILE="/data/data/leankernel/gpu_governor"
SFILE="/sys/devices/fdb00000.qcom,kgsl-3d0/devfreq/fdb00000.qcom,kgsl-3d0/governor"
[ -f $CFILE ] && echo `cat $CFILE` > $SFILE
>>>>>>> 81f8f52... ramdisk: replace random/urandom with erandom (from frandom)

stop thermald
sleep 2
start thermald
