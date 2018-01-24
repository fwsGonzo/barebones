# Barebones multiboot kernel

- Install qemu for qemu-system-x86_64
- Run with run.sh
- Intended for beginner OS development

## Features

- 600kb stack and working GDT
- Multiboot parameters passed to kernel start
- assert(), kprintf() and snprintf()
- C and C++ global constructors
- Stack protector support



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
