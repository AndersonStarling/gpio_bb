sudo mount /dev/sdb2 /mnt/sdcard
lsblk
sudo cp ../gpio_bb.ko /mnt/sdcard/home/root/gpio_bb.ko
sync
sudo umount /mnt/sdcard
lsblk

