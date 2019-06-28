#!/bin/bash
set -e
GRAPHICS=-nographic
BUILD_DIR=build

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
	--clean)
    rm -rf $BUILD_DIR/
    exit 0
    ;;
    *)
          # unknown option
		  echo "--kvm, --vga, --sanitize, --clean"
    ;;
esac
done

mkdir -p $BUILD_DIR
pushd $BUILD_DIR
cmake ..
make -j4 $OPTION
BINARY=$BUILD_DIR/`cat binary.txt`
popd

# NOTE: if building with -march=native, make sure to enable KVM,
# as emulated qemu only supports up to SSE3 instructions
qemu-system-x86_64 $KVM -kernel tools/chainloader -initrd $BINARY $GRAPHICS
