#!/bin/bash
MOUNT_POINT=${1:-./temp_mnt}
sudo umount "$MOUNT_POINT" 
