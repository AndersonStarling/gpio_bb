echo "Overwrite dtb file"
./backup_dtb.sh
echo "Merge user dtb and system dtb"
./merge_dtb.sh
echo "Copy .ko to target board"
./build_ko.sh
