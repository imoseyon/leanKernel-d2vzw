#!/system/bin/sh

check="qmiproxy gsiff"

sleep 45
stop thermald
sleep 1
max=`cat /sys/devices/system/cpu/cpu0/cpufreq/thermal_max_freq`
echo $max > /sys/devices/system/cpu/cpu0/cpufreq/scaling_max_freq
start thermald

for i in $check; do
  etime=`busybox ps -o pid,time,args | grep $i | grep -v grep | awk '{ print $2 }' | awk -F: '{ print $2 }'`
  if [ $etime -gt 20 ]; then
    pkill -f $i
    echo "$i restarted due to cpu time of $etime" >> /sdcard/lk.log
  fi
done
