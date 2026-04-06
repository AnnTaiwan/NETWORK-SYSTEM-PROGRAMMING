#!/bin/bash
FSTYPE=${1:-ext4}
IMAGE=${2:-disk.img}
MOUNT_POINT="./mnt"

mkdir -p "$MOUNT_POINT"

sudo mount -t "$FSTYPE" -o loop "$IMAGE" "$MOUNT_POINT"
