#!/bin/bash
[[ `diff arch/arm/configs/lk_defconfig .config ` ]] && \
        { echo "Unmatched defconfig!"; exit -1; }
[[ $2 == "compile" ]] &&
  sed -i s/CONFIG_LOCALVERSION=\".*\"/CONFIG_LOCALVERSION=\"-leanKernel-${1}\"/ .config &&
  make ARCH=arm CROSS_COMPILE=/data/linaro/android-toolchain-eabi/bin/arm-linux-androideabi- -j2 && cp .config arch/arm/configs/lk_defconfig
find drivers -name "*.ko" | xargs /data/linaro/android-toolchain-eabi/bin/arm-linux-androideabi-strip --strip-unneeded
find drivers -name "*.ko" | xargs -i cp {} zip/system/lib/modules/
cd cm.ramdisk
chmod 750 init*
chmod 644 default* uevent* MSM*
find . | cpio -o -H newc | gzip > /tmp/ramdisk.img
cd ../
/data/unpack-mkbootimg/mkbootimg --cmdline 'console=null androidboot.hardware=qcom user_debug=31' --base 0x80200000 --ramdiskaddr 0x81500000 --kernel arch/arm/boot/zImage --ramdisk /tmp/ramdisk.img -o zip/boot.img
cd zip
zip -r lk_aosp_jb_beta-v${1}.zip *
mv lk_aosp_jb_beta-v${1}.zip /tmp
[[ $2 == "upload" ]] && /data/utils/s3_ftpupload2.sh $1
