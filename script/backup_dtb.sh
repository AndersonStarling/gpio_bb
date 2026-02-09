sudo mount /dev/sdb1 /mnt/sdcard-boot
cd /mnt/sdcard-boot
sudo cp /mnt/sdcard-boot/overlays/am335x-boneblack.dtb am335x-boneblack.dtb
sync
cd ~
sync
sudo umount /mnt/sdcard-boot
