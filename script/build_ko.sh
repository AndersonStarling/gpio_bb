cd ../
make clean
make all
sudo mount /dev/sdb2 /mnt/sdcard
lsblk
echo "Copy .ko file"
sudo cp gpio_bb.ko /mnt/sdcard/home/root/gpio_bb.ko
echo "Copy test app"
chmod +x ./test/test_gpio/test_gpio.sh
sudo cp ./test/test_gpio/test_gpio.sh /mnt/sdcard/home/root/test_gpio.sh
sync
sudo umount /mnt/sdcard
lsblk
