#!/bin/bash
set -e
make -j
KERNEL=`make executable`
LOCAL_DISK=temp_disk

# create grub.iso
mkdir -p $LOCAL_DISK/boot/grub
cp $KERNEL $LOCAL_DISK/boot/mykernel
cp grub/grub.cfg $LOCAL_DISK/boot/grub
echo "=>"
grub-mkrescue -o grub.iso $LOCAL_DISK
echo "grub.iso constructed"
