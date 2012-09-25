#!/bin/bash
prefix="lk_tw_ics"
device="vzw"
filename="${prefix}_${device}-v${1}.zip"

[[ `diff arch/arm/configs/lk_defconfig .config ` ]] && \
        { echo "Unmatched defconfig!"; exit -1; }
[[ $2 == "compile" ]] &&
  sed -i s/CONFIG_LOCALVERSION=\".*\"/CONFIG_LOCALVERSION=\"-leanKernel-${1}\"/ .config &&
  make ARCH=arm CROSS_COMPILE=/data/linaro/android-toolchain-eabi/bin/arm-linux-androideabi- -j2 && cp .config arch/arm/configs/lk_defconfig
find drivers -name "*.ko" | xargs /data/linaro/android-toolchain-eabi/bin/arm-linux-androideabi-strip --strip-unneeded
find drivers -name "*.ko" | xargs -i cp {} zip/system/lib/modules/
cd lk.ramdisk
chmod 750 init*
chmod 644 default* uevent* MSM*
chmod 755 sbin
find . | cpio -o -H newc | gzip > /tmp/ramdisk.img
cd ../
/data/unpack-mkbootimg/mkbootimg --cmdline 'console=null androidboot.hardware=qcom user_debug=31' --base 0x80200000 --ramdiskaddr 0x81500000 --kernel arch/arm/boot/zImage --ramdisk /tmp/ramdisk.img -o zip/boot.img
cd zip
zip -r $filename *
mv $filename /tmp
[[ $2 == "upload" ]] && /data/utils/s3_ftpupload.sh $1
echo
md5sum /tmp/$filename
