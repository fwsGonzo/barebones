# Barebones multiboot kernel

- Intended for beginner OS development
- Run the dependency installation script (or install the equivalent packages)
- Build and boot kernel in Qemu with `./run.sh default`.
	- You can run any of the machines in the `machines` folder this way.
- If building with the NATIVE option (or anything that requires AVX), remember to run with KVM enabled (./run.sh --kvm) so that Qemu will present the modern instruction set features to the guest operating system.
- VGA textmode using ./run.sh --vga (NOTE: you must implement VGA support yourself! You will (if you're lucky) see a black screen.)
- Use ./build_iso.sh to run on real hardware or a hypervisor like VirtualBox, but keep serial port logging in mind!

## Features

- 600kb stack and working GDT (with room for IST task)
- Multiboot parameters passed to kernel start
- assert(), kprintf() and snprintf()
- Very basic heap implementation (malloc/free, new/delete)
- C and C++ global constructors
- Stack protector support
- EASTL C++ support, which has many useful containers
- Produces tiny machine images
	- Machine image is 5992 bytes with minimal stripped build on GCC 9.1
	- Machine image is 8560 bytes with minimal stripped build on Clang 8
	- 14kb with all options normal and LTO enabled on Clang 8
	- 23kb with all options normal on GCC 9.1
	- Remember to disable stack protector to shave a few extra bytes off!
- LTO and ThinLTO support if you are using clang
- Undefined-sanitizer support to catch some problems during runtime
- Option to unmap zero page (for the very common null-pointer access bugs)
    - You have to enable this yourself, after CPU exception handling

## Running

Use `./run.sh <folder>` to build and run your kernel stored in machines folder.
- By default run.sh will build and run the `default` machine. The project is located in `machines/default/` and most of the output comes from `main.cpp`.
- Use argument --kvm if you want to use hardware-accelerated virtualization, or you have built with -march=native in which you must use that. Enable virtualization in your BIOS settings if you haven't.
- Use argument --vga to get a graphical window, which starts out in VGA textmode accessible at 0xb8000.
- Use argument --sanitize to enable the undefined-sanitizer, catching problems like misaligned accesses and overflows.

## Changing build options

Go into the build folder in your machines folder and type `ccmake ..`, which opens a GUI with some adjustable settings. After changing settings press C to configure the changes, and then E to exit the GUI. Changes will be rebuilt automatically using run.sh, or you could simply use `make` like normally in the build folder itself.

## Goal

The goal is to provide a barebones kernel project that implements the most basic C/C++ voodoo to let people start reading/writing to registers and devices immediately. The goal is also to provide people with a choice of using C or C++, or a mix of both.

## Future work

- Add TLS api and support for most thread local variables
- Add support for LTO with GCC (no ideas on how to do this yet)
- Optional 512b bootloader to avoid GRUB (although not recommended)

## Validate output

./run.sh output should match:
```
------------------
* Multiboot EAX: 0x2badb002
* Multiboot EBX: 0x9500
* SSE instructions ... work!
* Global constructors ... work!

Hello OSdev world!
my_kernel (This is a test kernel!)

Press Ctrl+A -> X to close
Returned from kernel_start! Halting...
```

## Common issues

- File format not recognized
    - You changed linker or ELF architecture without rebuilding all the files
- Nothing happens when I start the kernel
    - If you are building with -march=native, you need to enable KVM and run the kernel natively, because the machine code is full of extended instructions (like AVX)
- I can't use normal C/C++ library functions
    - Make sure you are using something that exists in EASTL, or you will have to implement it yourself
- I can write to the zero page
    - Yes you can. Welcome to (identity-mapped) kernel space. Set up your own pagetables!
- Do I really need a cross-compiler to work on this project?
    - No, that is a misconception that the OSdev community holds dear to its heart, but its not necessary. The Linux-native compilers will use FS/GS for TLS, and will require you to fake the table that (among other things) contain the stack sentinel value. Otherwise, you can reasonably expect normal library calls to be optimized (printf -> write).
- When booting the GRUB image nothing happens, it looks to be crashing etc.
	- Make sure you didn't compile with GCC and then link with LLD, and especially don't mix LTO into this.
	- Minimal builds can be risky, especially -Oz on clang.
	- There could be a genuine issue, so let me know.

## Undefined sanitizer

- Works on GCC and Clang
- Easy to support, but has some caveats
    - It's miles better if you can resolve symbol addresses to names
- Has not been extensively verified

## Link-Time Optimization

- Works on Clang out of the box, just make sure you use the correct LLD that comes with your compiler
    - Both ThinLTO and full LTO works!
	- Enable LTO option with ccmake and set the LINKER_EXE option to point to a lld executable that is compatible with your Clang version. For example `ld.lld-8`.
- Does not work on GCC at all due to how complicated it is to call the LTO-helpers manually

## Thread-Local Storage

- The basic foundation has been laid down
- Linker script needs to be amended to store .tbss and .tdata sections
- Functionality for creating new thread storage needs to be created

## Qemu defaults

Qemu provides you with some default devices to start with
- IDE for disk devices, but you can use PCI to get information
- e1000-compatible network card
- If you add -vga std you will get VGA graphics area at 0xb8000
- In graphics mode: PS/2 keyboard and mouse

As an example, use this function to clear the VGA textmode screen:
```
uint16_t* vga = (uint16_t*) 0xb8000;
for (int i = 0; i < 25*80; i++)
{
  vga[i] = ' ' | (7 << 8);
}
```
What it's actually doing is filling the text buffer with spaces. Spaces that have a gray color. You just can't see them, because spaces are spaces.
