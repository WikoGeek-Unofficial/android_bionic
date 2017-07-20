Download:
=========
If you are not already using an AOSP toolchain (included in an AOSP build tree), download the corresponding official android toolchain for the arm-eabi specified above for this device:
        
git clone https://android.googlesource.com/platform/prebuilts
(use darwin-x86 in place of linux-x86 for mac)




Build the kernel:
=================
set the following environment variables:

source build/envsetup.sh
lunch l5421-user

build kernel:
make -j8 kernel


Output Binary Files:
====================
After the build process is finished, there should be a file boot.img

If you have already built and installed a boot.img with root access you can also fastboot boot.img into the device using:
adb reboot bootloader 
fastboot flash boot out/target/product/l5421/boot.img
fastboot reboot

