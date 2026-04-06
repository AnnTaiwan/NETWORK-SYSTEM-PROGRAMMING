* Create an image file.
```
# Create an image which is 100M
dd if=/dev/zero of=disk.img bs=1M count=100
```
* Set the file system type
```
# See available file system types
ls /sbin/mkfs.*

mkfs.ext2 disk.img
mkfs.ext3 disk.img
mkfs.ext4 disk.img
```
* Mount the filesystem at `./temp_mnt`
```
./mymount.sh ext2 disk.img
# Create a file by root
sudo sh -c 'echo hello > t'
echo "hello" | sudo tee t
```
* Check remaining loop files
```
losetup -a
```
>Remember to umount the mount point and do `losetup -d <loop_file_name>`.
* **End of experiment, must do:**
```
./myumount.sh
# or
sudo umount ./temp_mnt

sudo losetup -d /dev/loop0 # check the loop file by `losetup -a` to find disk.img linked to which loop file
```
