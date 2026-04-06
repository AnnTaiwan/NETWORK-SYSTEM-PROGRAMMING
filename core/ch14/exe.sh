#!/bin/bash
FSTYPE=${1:-ext4}
IMAGE=${2:-disk.img}
MOUNT_POINT="./temp_mnt"

if mountpoint -q "$MOUNT_POINT"; then
    echo "First umount the $MOUNT_POINT"
    sudo umount "$MOUNT_POINT"

fi

make
mkdir -p "$MOUNT_POINT"
mkfs."$FSTYPE" "$IMAGE"
sudo mount -t "$FSTYPE" -o loop "$IMAGE" "$MOUNT_POINT"
sudo chown $USER:$USER "$MOUNT_POINT"

echo "Setup complete. $MOUNT_POINT is ready for $USER."
