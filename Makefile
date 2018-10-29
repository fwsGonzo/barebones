# kernel binary
OUT = mykernel
# .c files (add your own!)
C_FILES = src/kernel/kernel_start.c \
					src/hw/serial1.c \
					src/crt/c_abi.c src/crt/heap.c src/crt/malloc.c \
				  src/prnt/print.c src/prnt/mini-printf.c
# .cpp files
CPP_FILES=src/main.cpp src/crt/cxxabi.cpp
# .asm files for NASM
ASM_FILES=src/kernel/start.asm
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
OPTIMIZE = -Ofast -mfpmath=sse -march=native

## to enable LLVM / ThinLTO use these ##
#LD=ld.lld-6.0         # path to your LLD binary
#LTO_DEFS=-flto=full   # full or thin

OPTIONS=-m32 $(INCLUDE) $(GDEFS) $(OPTIMIZE) $(LTO_DEFS)
WARNS=-Wall -Wextra -pedantic
COMMON=-ffreestanding -nostdlib -MMD -fstack-protector-strong $(OPTIONS) $(WARNS)
LDFLAGS=-static -nostdlib -melf_i386 --strip-all --script=src/linker.ld
CFLAGS=-std=gnu11 $(COMMON)
CXXFLAGS=-std=c++14 -fno-exceptions -fno-rtti $(COMMON)

COBJ=$(C_FILES:.c=.o)
CXXOBJ=$(CPP_FILES:.cpp=.o)
ASMOBJ=$(ASM_FILES:.asm=.o)
DEPS=$(CXXOBJ:.o=.d) $(COBJ:.o=.d)

.PHONY: clean all executable

%.o: %.asm
	nasm -f elf -o $@ $<

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
