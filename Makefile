# kernel binary
OUT = mykernel
# .c files (add your own!)
C_FILES = src/kernel/kernel_start.c \
					src/hw/serial1.c \
					src/crt/c_abi.c src/crt/heap.c src/crt/malloc.c \
				  src/prnt/print.c src/prnt/mini-printf.c
# .cpp files
CPP_FILES=src/test.cpp src/crt/cxxabi.cpp
# .asm files for NASM
ASM_FILES=src/kernel/start.asm

OPTIONS=-m32 -msse3 -Isrc
WARNS=-Wall -Wextra -pedantic
COMMON=-ffreestanding -nostdlib -MMD -fstack-protector-strong $(OPTIONS) $(WARNS)
LDFLAGS=-static -nostdlib -melf_i386 -n --strip-all --script=linker.ld
CFLAGS=-std=gnu11 $(COMMON)
CXXFLAGS=-std=c++14 -fno-exceptions -fno-rtti $(COMMON)

COBJ=$(C_FILES:.c=.o)
CXXOBJ=$(CPP_FILES:.cpp=.o)
ASMOBJ=$(ASM_FILES:.asm=.o)
DEPS=$(CXXOBJ:.o=.d) $(COBJ:.o=.d)

.PHONY: clean all

%.o: %.asm
	nasm -f elf -o $@ $<

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

all: $(COBJ) $(CXXOBJ) $(ASMOBJ)
	$(LD) $(LDFLAGS) $(COBJ) $(CXXOBJ) $(ASMOBJ) -o $(OUT)

clean:
	$(RM) $(OUT) $(COBJ) $(CXXOBJ) $(ASMOBJ) $(DEPS)

-include $(DEPS)
