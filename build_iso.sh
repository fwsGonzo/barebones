#!/bin/bash
set -e
MACHINE=machines/${1-default}
BUILD_DIR=$MACHINE/build

mkdir -p $BUILD_DIR
pushd $BUILD_DIR
cmake ..
make -j4 $OPTION
popd

KERNEL=$BUILD_DIR/`cat $BUILD_DIR/binary.txt`
LOCAL_DISK=temp_disk

# create grub.iso
mkdir -p $LOCAL_DISK/boot/grub
cp $KERNEL $LOCAL_DISK/boot/mykernel
cp grub/grub.cfg $LOCAL_DISK/boot/grub
echo "=>"
grub-mkrescue -o grub.iso $LOCAL_DISK
echo "grub.iso constructed"
