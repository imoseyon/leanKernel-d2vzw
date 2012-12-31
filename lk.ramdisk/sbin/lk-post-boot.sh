#!/system/xbin/busybox ash

check="qmiproxy gsiff"

for i in $check; do
  etime=`busybox ps -o pid,time,args | grep $i | grep -v grep | awk '{ print $2 }' | awk -F: '{ print $2 }'`
  [ $etime -gt 10 ] && pkill -f $i
done

# restart thermald to use new max freq set by cpu app
sleep 45
pkill thermald
