echo "Build overlay"
cd ../test/overlay
dtc -@ -I dts -O dtb -o gpio-bb.dtbo gpio-bb.dts

echo "Flash overlay to SD card"
lsblk
sudo mount /dev/sdb1 /mnt/sdcard-boot
sudo cp gpio-bb.dtbo /mnt/sdcard-boot/overlays
sync
cd /mnt/sdcard-boot
sudo fdtoverlay -i am335x-boneblack.dtb -o am335x-boneblack.dtb overlays/gpio-bb.dtbo
cd /home/ntai
sudo umount /mnt/sdcard-boot
echo "Success"