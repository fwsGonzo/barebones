
OUT=mykernel
C_FILES=src/main.c src/serial1.c
ASM_FILES=src/start.asm

CC=clang-4.0
WARNS=-Wall -Wextra -pedantic
COPT=-target i686-linux-pc-gnu -march=native
CFLAGS=-ffreestanding -std=gnu11 -MMD $(COPT) $(WARNS)
LDFLAGS=-static -nostdlib -melf_i386 -N --strip-all --script=linker.ld

COBJ=$(C_FILES:.c=.o)
ASMOBJ=$(ASM_FILES:.asm=.o)
DEPS=$(ASMOBJ:.o=.d) $(COBJ:.o=.d)

.PHONY: clean all

%.o: %.asm
	nasm -f elf -o $@ $<

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

all: $(COBJ) $(ASMOBJ)
	ld $(LDFLAGS) $(COBJ) $(ASMOBJ) -o $(OUT)

clean:
	$(RM) $(DEPS) $(OUT) $(COBJ) $(ASMOBJ)

-include $(DEPS)
