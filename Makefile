# kernel binary
OUT = mykernel
# .c files (add your own!)
C_FILES = src/kernel/kernel_start.c \
					src/hw/serial1.c \
					src/crt/c_abi.c src/crt/heap.c src/crt/malloc.c \
				  src/prnt/print.c src/prnt/mini-printf.c
# .cpp files
CPP_FILES=src/main.cpp src/crt/cxxabi.cpp \
					src/kernel/tls.cpp src/kernel/panic.cpp
# .asm files for NASM
ASM_FILES=src/kernel/start.asm src/kernel/start64.asm
# includes
INCLUDE=-Isrc
# EASTL C++ library
#INCLUDE +=-Iext/EASTL/include -Iext/EASTL/test/packages/EABase/include/Common
#CPP_FILES += ext/EASTL/source/allocator_eastl.cpp ext/EASTL/source/assert.cpp \
#							ext/EASTL/source/fixed_pool.cpp ext/EASTL/source/hashtable.cpp \
#							ext/EASTL/source/intrusive_list.cpp ext/EASTL/source/numeric_limits.cpp \
#							ext/EASTL/source/red_black_tree.cpp ext/EASTL/source/string.cpp

GDEFS =
LIBS  =
OPTIMIZE = -Ofast -mfpmath=sse -msse3 #-march=native

## to enable LLVM / ThinLTO use these ##
#LD=ld.lld             # path to your LLD binary
#LTO_DEFS=-flto=full   # full or thin

OPTIONS=-m64 $(INCLUDE) $(GDEFS) $(OPTIMIZE) $(LTO_DEFS)
WARNS=-Wall -Wextra -pedantic
COMMON=-ffreestanding -nostdlib -MMD -fstack-protector-strong -fno-omit-frame-pointer $(OPTIONS) $(WARNS)
# inject some random numbers into a symbol so we can get some free random bits
SSP=$(shell hexdump -n 8 -e '4/4 "%08X" 1 "\n"' /dev/random)
LDFLAGS=-static -nostdlib -N -melf_x86_64 --strip-all --script=src/linker.ld --defsym __SSP__=0x$(SSP)
CFLAGS=-std=gnu11 $(COMMON)
CXXFLAGS=-std=c++14 -fno-exceptions -fno-rtti $(COMMON)

COBJ=$(C_FILES:.c=.o)
CXXOBJ=$(CPP_FILES:.cpp=.o)
ASMOBJ=$(ASM_FILES:.asm=.o)
DEPS=$(CXXOBJ:.o=.d) $(COBJ:.o=.d)

.PHONY: clean all executable

%.o: %.asm
	nasm -f elf64 -o $@ $<

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

all: $(COBJ) $(CXXOBJ) $(ASMOBJ)
	$(LD) $(LDFLAGS) $(COBJ) $(CXXOBJ) $(ASMOBJ) $(LIBS) -o $(OUT)

chainloader: $(COBJ) $(CXXOBJ) $(ASMOBJ)
	$(LD) $(LDFLAGS) $(COBJ) $(CXXOBJ) $(ASMOBJ) $(LIBS) -o chainloader

executable:
	$(info $(OUT))
	@true

clean:
	$(RM) $(OUT) $(COBJ) $(CXXOBJ) $(ASMOBJ) $(DEPS)

-include $(DEPS)
