# Barebones multiboot kernel

- Intended for beginner OS development
- Install qemu for qemu-system-x86_64 and gcc-multilib for a 32-bit compiler
- Run with ./run.sh
- Use ./build_iso.sh to run on real hardware or a hypervisor like VirtualBox, but keep serial port logging in mind!

## Features

- 600kb stack and working GDT
- Multiboot parameters passed to kernel start
- assert(), kprintf() and snprintf()
- C and C++ global constructors
- Stack protector support

## Future work

- Build a proper cross-compiler locally and use it to build this project with
- Create simple page tables and directly enter long mode (64-bit)

## Validate output

Output should match
```
------------------
Hello OSdev world!
Multiboot EAX: 0x2badb002
Multiboot EBX: 0x9500
Hello from freestanding C++!
Press Ctrl+A -> X to close
Returned from kernel_start! Halting...
```
