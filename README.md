# ext2-fsck

Install ext2fs libarary (includes from linux)
```shell script
sudo apt-get install e2fslibs-dev
```

Working with filesystem
```shell script
# create file
mkfs.ext2 file 1000

# mount it
sudo mkdir /mnt/file
sudo mount -t ext2 -o loop file /mnt/file

# check if mounted
mount -t ext2

# unmount
sudo umount /mnt/file
```