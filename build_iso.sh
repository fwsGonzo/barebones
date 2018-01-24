#!/bin/bash
set -e
make -j
KERNEL=mykernel
LOCAL_DISK=temp_disk

# create grub.iso
mkdir -p $LOCAL_DISK/boot/grub
cp $KERNEL $LOCAL_DISK/boot/service
cp grub/grub.cfg $LOCAL_DISK/boot/grub
echo "=>"
grub-mkrescue -o grub.iso $LOCAL_DISK
echo "grub.iso constructed"
