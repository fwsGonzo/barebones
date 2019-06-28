#!/bin/bash
set -e
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
    --sanitize)
    OPTION="sanitize"
    shift # past argument with no value
    ;;
    *)
          # unknown option
    ;;
esac
done

mkdir -p build
pushd build
cmake ..
make -j4 $OPTION
BINARY=build/`cat binary.txt`
popd

# NOTE: if building with -march=native, make sure to enable KVM,
# as emulated qemu only supports up to SSE3 instructions
qemu-system-x86_64 $KVM -kernel tools/chainloader -initrd $BINARY $GRAPHICS
#qemu-system-x86_64 $KVM -kernel tools/chainloader -initrd mykernel $GRAPHICS
