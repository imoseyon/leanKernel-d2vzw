#!/bin/bash
[[ $2 == "compile" ]] &&
  sed -i s/CONFIG_LOCALVERSION=\".*\"/CONFIG_LOCALVERSION=\"-leanKernel-${1}\"/ .config &&
  make ARCH=arm CROSS_COMPILE=/data/linaro/android-toolchain-eabi/bin/arm-linux-androideabi- -j2
find drivers -name "*.ko" | xargs /data/linaro/android-toolchain-eabi/bin/arm-linux-androideabi-strip --strip-unneeded
find drivers -name "*.ko" | xargs -i cp {} zip/system/lib/modules/
cp arch/arm/boot/zImage kexec/kexec/kernel
cd cm.ramdisk
chmod 750 init*
chmod 644 default* uevent* MSM*
find . | cpio -o -H newc | gzip > ../kexec/kexec/ramdisk.img
cd ../kexec
zip -r kexec-boot.zip *
mv kexec-boot.zip ../zip/system/etc
cd ../zip
zip -r lk_aosp_jb_beta-v${1}.zip *
mv lk_aosp_jb_beta-v${1}.zip /tmp
[[ $2 == "upload" ]] && /data/utils/s3_ftpupload2.sh $1
