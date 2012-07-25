#make ARCH=arm CROSS_COMPILE=/root/CodeSourcery/Sourcery_G++_Lite/bin/arm-none-linux-gnueabi- -j2
#make ARCH=arm CROSS_COMPILE=/data/linaro/android-toolchain-eabi/bin/arm-linux-androideabi- -j2
cp arch/arm/boot/zImage kexec/kexec/kernel
cd lk.ramdisk
chmod 750 init*
chmod 644 default* uevent* MSM*
find . | cpio -o -H newc | gzip > ../kexec/kexec/ramdisk.img
cd ../kexec
zip -r kexec-boot.zip *
mv kexec-boot.zip ../zip/system/etc
cd ../zip
zip -r lk_tw_testv${1}.zip *
mv lk_tw_testv${1}.zip /tmp

#/data/unpack-mkbootimg/mkbootimg --cmdline 'console=null androidboot.hardware=qcom user_debug=31' --base 0x80200000 --ramdiskaddr 0x81500000 --kernel arch/arm/boot/zImage --ramdisk /tmp/lk-ramdisk.cpio.gz -o /tmp/lkboot6.img
#bspatch /tmp/lkboot.img /tmp/lkboot4.img imagediff.patch 
