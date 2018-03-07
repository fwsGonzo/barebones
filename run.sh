#!/bin/bash
set -e
make -j
qemu-system-x86_64 -kernel `make executable` -nographic
# alternatively you can run with hardware support:
#qemu-system-x86_64 --enable-kvm -kernel `make executable` -nographic
