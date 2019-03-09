#!/bin/bash
set -e
make -j4
GRAPHICS=-nographic

for i in "$@"
do
case $i in
    --kvm)
    KVM="--enable-kvm -cpu host"
    shift # past argument with no value
    ;;
    --vga)
    GRAPHICS="-vga std"
    shift # past argument with no value
    ;;
    *)
          # unknown option
    ;;
esac
done

# NOTE: if building with -march=native, make sure to enable KVM,
# as emulated qemu only supports up to SSE3 instructions
sudo qemu-system-x86_64 $KVM -kernel tools/chainloader -initrd `make executable` $GRAPHICS
