#!/system/bin/sh

sleep 45
stop thermald
sleep 1
max=`cat /sys/devices/system/cpu/cpu0/cpufreq/thermal_max_freq`
echo $max > /sys/devices/system/cpu/cpu0/cpufreq/scaling_max_freq
start thermald
