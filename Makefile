
OUT=mykernel
C_FILES=src/kernel_start.c src/serial1.c src/c_abi.c \
				 src/prnt/print.c src/prnt/mini-printf.c
CPP_FILES=src/test.cpp
ASM_FILES=src/start.asm

#CC=clang-4.0
#COPT=-target i686-linux-pc-gnu -march=native
COPT=-m32 -msse3

WARNS=-Wall -Wextra -pedantic
COMMON=-ffreestanding -nostdlib -MMD -fstack-protector-strong $(COPT) $(WARNS)
LDFLAGS=-static -nostdlib -melf_i386 -N --strip-all --script=linker.ld
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
