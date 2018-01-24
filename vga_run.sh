#!/bin/bash
set -e
make -j
qemu-system-x86_64 -kernel mykernel -vga std
# alternatively you can run with hardware support:
#qemu-system-x86_64 --enable-kvm -kernel mykernel -vga std
