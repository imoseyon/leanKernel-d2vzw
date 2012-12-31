#!/system/xbin/busybox ash

check="qmiproxy gsiff"

for i in $check; do
  etime=`busybox ps -o pid,time,args | grep $i | grep -v grep | awk '{ print $2 }' | awk -F: '{ print $2 }'`
  if [ $etime -gt 20 ]; then
    pkill -f $i
    echo "$i restarted due to cpu time of $etime" >> /sdcard/lk.log
  fi
done

# restart thermald to use new max freq set by cpu app
sleep 45
pkill thermald
